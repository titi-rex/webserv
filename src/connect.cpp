/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   communication.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 23:11:38 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/09 01:00:41 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "webserv.hpp"

extern sig_atomic_t	g_status;

/**
 * @brief receive one client request and return it
 * throw: should not close server
 * 
 * @param sock_listen 
 * @param client_fd 
 * @return std::string 
 */
std::string	recept_request(int sock_listen, int& client_fd)
{
	struct sockaddr_in	client_sin;
	socklen_t			client_len = sizeof(client_sin);

	bzero(&client_sin, sizeof(sockaddr_in));
	client_fd = accept(sock_listen, (struct sockaddr*) &client_sin, &client_len);
	if (client_fd == -1)
		throw std::runtime_error("cannot accept client socket");

	std::clog << client_sin << std::endl;

	char		rec_buffer[MAXLINE + 1] = {0};
	int			n_rec;
	std::string	res;

	while ((n_rec = recv(client_fd, &rec_buffer, MAXLINE, 0)) > 0) //MSG_DONTWAIT | MSG_CMSG_CLOEXEC
	{
		rec_buffer[n_rec] = 0;
		res += rec_buffer;
		if (res.find("\r\n\r\n", 4) != std::string::npos)
			break ;
	}
	if (n_rec == -1)
		throw std::runtime_error("recv fail");
	return(res);
}


/**
 * @brief temporary func to respond to client
 * 	throw: should not close server
 * @param client_fd 
 */
void	WebServer::send_response(int client_fd)
{
	std::string		response;
	std::ifstream	indexPage("data/default_page/index.html");

	if (indexPage.fail())
		throw std::runtime_error("error closing file");
	std::getline(indexPage, response, '\0');
	response= "HTTP/1.0 200 OK\r\n\r\n" + response + "\r\n\r\n";
	int n = send(client_fd, response.c_str() , response.length(), MSG_DONTWAIT);
	indexPage.close();
	if (n < 0)
		throw std::runtime_error("error send");
}

/**
 * @brief main function
 * 	wait for epoll event and act accordly to it
 * 	throw: should not throw or only to quit properly programme
 */
void	WebServer::run(void)
{
// wait for event
	struct epoll_event	revents[MAX_EVENTS];
	bzero(revents, sizeof(revents));
	while (g_status)
	{
		std::clog << "Waiting for event.." << std::endl;
		int n_event = epoll_wait(_efd, revents, MAX_EVENTS, TIMEOUT);
		if (n_event == -1)
			throw std::runtime_error("erro epoll wait");
		std::clog << n_event << " events ready" << std::endl;
	// process event
		for (int i = 0; i < n_event; ++i)
		{
			int	client_fd;
		// read and parse request
			try
			{
				Request	rq(recept_request(revents[i].data.fd, client_fd));
				std::clog << rq << std::endl;
			// special instruction : execute shutdown
				if (rq.getUri() == "/shutdown")
					g_status = 0;
			// respond to request, there should be GET/HEAD/POST
				send_response(client_fd);
				close(client_fd);
				
			}
			catch (std::exception & e)
			{
				//	exception here should be interpreted :
				//	and proper response should be send to client
				//	aka GET to proper error file
				//	plus error should be loginto log file
				
				close(client_fd);
			}
		}
	}
}
