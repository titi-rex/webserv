/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connect.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 23:11:38 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/12 10:54:31 by tlegrand         ###   ########.fr       */
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
 * @brief send wrapper to respond to client , should close client_fd if successful
 * 	throw: should not close server
 * @param client_fd, response
 */
void	send_response(int client_fd, std::string response)
{
	if (send(client_fd, response.c_str() , response.length(), MSG_DONTWAIT) == -1)
		throw std::runtime_error("500 send fail");
	close(client_fd);
}


void	epoll_error_handler(void)
{
	if (errno == EBADF || errno == EINVAL)
		throw std::runtime_error("fatal: wrong epoll fd");
	if (errno == EFAULT)
		throw std::runtime_error("fatal: epoll cant write events");
	throw std::runtime_error("fatal: unknow");
};



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
			epoll_error_handler() ;
		std::clog << n_event << " events ready" << std::endl;
	// process event
		for (int i = 0; i < n_event; ++i)
		{
			int	client_fd;

			try
			{
			// read and parse request
				Request	rq(recept_request(revents[i].data.fd, client_fd));
				std::clog << rq << std::endl;

			// special instruction : execute shutdown
				if (rq.getUri() == "/shutdown")
					g_status = 0;

			// prepare response based on request, there should be GET/HEAD/POST
				std::string	response = GET("data/default_page/index.html");

			//	send response to client
				send_response(client_fd, response);
			}
			catch (std::exception & e)
			{
				//	exception here should be interpreted :
				//	and proper response should be send to client
				//	aka GET to proper error file
				//	plus maybe error should be log into log file
				std::clog << e.what() << std::endl;
				int	status = std::atoi(e.what());
				if (status == 0)
					status = 500;
				std::string response = GET_error(status);

				send_response(client_fd, response);
			}
		}
	}
}
