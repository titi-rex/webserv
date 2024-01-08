/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_connect.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 23:11:38 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/08 14:18:18 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

extern sig_atomic_t	g_status;


void	WebServer::addClient(int socketServerFd)
{
	Client	cl;

	try
	{
		cl.accept(socketServerFd);
		_ClientList.push_back(cl);
		// add to epoll
		
	}
	catch(const std::exception& e)
	{
		cl.cstatus = FATAL;
		std::cerr << e.what() << '\n';
		std::cerr << strerror(errno) << std::endl;
	}
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
		std::cerr << "nrec : " << n_rec << ", max : " << MAXLINE << std::endl;
		rec_buffer[n_rec] = 0;
		res += rec_buffer;
		if (n_rec < MAXLINE)
			break ;
	}
	// std::cerr << std::endl << "raw : " << std::endl << res << std::endl << std::endl;
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
			Client cl;
			
			cl.accept(revents[i].data.fd);

			try
			{
			// read and parse request
			
				std::string	raw = _read_request(cl.getFd());
				cl.request.build(raw);
			std::clog << cl.request << std::endl;

			// special instruction : execute shutdown
				if (cl.request.getUri() == "/shutdown")
					g_status = 0;
				if (cl.request.getUri() == "/throw")
					throw std::runtime_error("404 Bof");
				if (cl.request.getUri() == "/fatal")
					throw std::runtime_error("415 Bof");
				v_host_ptr	host = _selectServer(_SocketServersList[revents[i].data.fd], cl.request);
			// std::cout << host << std::endl;
			// prepare response based on request, there should be GET/HEAD/POST
				// std::string	response = GET("data/default_page/index.html");
				std::string	response = "response\r\n\r\n"; //Method(rq, host);

			//	send response to client
				_send_response(cl.getFd(), response);
			}
			catch (std::exception & e)
			{
			//	exception here should be interpreted :
			//	and proper response should be send to client
			//	aka GET to proper error file
			//	plus maybe error should be log into log file
				std::clog << e.what() << std::endl;

				std::cerr << "errno value: " << errno << std::endl;
				std::cerr << strerror(errno) << std::endl;
				
				std::string	status(e.what(), 3);
				// if (status == 0)
				// 	status = 500;
				std::string response = GET_error2(status);

				_send_response(cl.getFd(), response);
			}
		}
	}
}
