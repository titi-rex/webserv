/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_connect.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 23:11:38 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/13 13:41:18 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

extern sig_atomic_t	g_status;

/**
 * @brief accept wrapper 
 * throw: should not close server
 */
int	WebServer::_recept_request(int sock_listen)
{
	int		client_fd;

	client_fd = accept(sock_listen, NULL, NULL);
	if (client_fd == -1)
		throw std::runtime_error("500 cannot accept client socket");
	return (client_fd);
}

/**
 * @brief read wrapper 
 * 	throw: should not close server
 */
std::string	WebServer::_read_request(int client_fd)
{
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
	{
		close(client_fd);
		throw std::runtime_error("500 recv fail");
	}
	return(res);	
}


/**
 * @brief send wrapper 
 * 	throw: should not close server
 */
void	WebServer::_send_response(int client_fd, std::string response)
{
	if (send(client_fd, response.c_str() , response.length(), MSG_DONTWAIT) == -1)
	{
		close(client_fd);
		throw std::runtime_error("500 send fail");
	}
	close(client_fd);
}


/**
 * @brief main function
 * 	wait for epoll event, get request and proceed it
 * 	throw: should not throw or only to quit properly programme
 */
void	WebServer::run(void)
{
// wait for event
	struct epoll_event	revents[MAX_EVENTS];
	std::memset(revents, 0, sizeof(revents));
	while (g_status)
	{
		std::clog << "Waiting for event.." << std::endl;
		int n_event = epoll_wait(_efd, revents, MAX_EVENTS, TIMEOUT);
		if (n_event == -1)
			throw std::runtime_error("fatal: epoll");
		if (n_event)
			std::clog << n_event << " events ready" << std::endl;
	// process event
		for (int i = 0; i < n_event; ++i)
		{
			int	client_fd = _recept_request(revents[i].data.fd);

			try
			{
			// read and parse request
			
				std::string	raw = _read_request(client_fd);
				Request	rq(raw);
				
			std::clog << rq << std::endl;

			// special instruction : execute shutdown
				if (rq.getUri() == "/shutdown")
					g_status = 0;
				v_host_ptr	host = _selectServer(_socketsList[revents[i].data.fd], rq);
				std::cout << host << std::endl;
			// prepare response based on request, there should be GET/HEAD/POST
				// std::string	response = GET("data/default_page/index.html");
				std::string	response = Method(rq, host);

			//	send response to client
				_send_response(client_fd, response);
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

				_send_response(client_fd, response);
			}
		}
	}
}
