/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_connect.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 23:11:38 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/10 12:50:39 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

extern sig_atomic_t	g_status;


void	WebServer::addClient(int socketServerFd)
{
	Client	cl;
	struct epoll_event	e;

	cl.accept(socketServerFd);//can throw (FATAL)
	e.data.fd = cl.getFd();
	e.events = EPOLLIN;
	if (epoll_ctl(_efd, EPOLL_CTL_ADD, cl.getFd(), &e) == -1)
		throw std::runtime_error("610: fail to add client to epoll");
	_ClientList[cl.getFd()] = cl;
}

		




void	WebServer::handle_epollerr(int event_id)
{
	std::cerr << "error happen " << std::endl;
}

void	WebServer::handle_epollhup(int event_id)
{
	std::cerr << "hang up happen " << std::endl;
}

void	WebServer::modClientEpoll(Client* cl, uint32_t events)
{
	struct epoll_event	e;

	e.events = events;
	e.data.fd = cl->getFd();
	if (epoll_ctl(_efd, EPOLL_CTL_MOD, cl->getFd(), &e) == -1)
		throw std::runtime_error("611: add to epollout fail");
}


void	WebServer::handle_epollin(int event_id)
{
	if (event_id <= _highSocket)
		addClient(event_id);	//throw FATAL 
	else if (event_id > 0)
	{
		Client*	cl = &_ClientList[event_id];
		
		if(cl->readRequest())	//throw ERROR or FATAL
		{
			modClientEpoll(cl, EPOLLOUT);	//thow FATAL
			_readyToProceedList.push_back(cl);
		}
	}
	else
		std::cout << "cgiwait.." << std::endl;
}

void	WebServer::handle_epollout(int event_id)
{
	Client*	cl = &_ClientList[event_id];

	if (cl->cstatus == PROCEEDED)
	{
		cl->sendRequest();	//throw FATAL
		// _readyToProceedList.erase(cl);
		if (cl->keepConnection)	//keep client
			{
				cl->reset();
				modClientEpoll(cl, EPOLLIN);	//Throw FATAL
			}
		else	//delete client
			_ClientList.erase(event_id);
	}
}


void	WebServer::process_rq(Client &cl)
{
// special instruction : execute shutdown
	if (cl.request.getUri() == "/shutdown")
		g_status = 0;
	if (cl.request.getUri() == "/throw")
		throw std::runtime_error("404 Bof error");
	if (cl.request.getUri() == "/fatal")
		throw std::runtime_error("699 Bof fatal");
	v_host_ptr	host = _selectServer(_SocketServersList[cl.getServerEndPoint()], cl.request);
std::cout << host << std::endl;
// prepare response based on request, there should be GET/HEAD/POST
	cl.request.response = GET("data/default_page/index.html");
}

void	WebServer::process_rq_error(Client &cl)
{
	cl.request.response = ERROR_500_MSG;
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
			try 
			{
				if (revents[i].events & EPOLLERR)
					handle_epollerr(revents[i].data.fd);
				if (revents[i].events & EPOLLHUP)
					handle_epollhup(revents[i].data.fd);
				if (revents[i].events & EPOLLIN)
					handle_epollin(revents[i].data.fd);
				if (revents[i].events & EPOLLOUT)
					handle_epollout(revents[i].data.fd);
			}
			catch (std::exception & e)
			{	
				std::cerr << e.what() << std::endl;
				std::cerr << strerror(errno) << std::endl;
				
				int	err = std::atoi(e.what());
				if (err == 0)
					err = 699;
				std::cerr << "cerror code : " << err << std::endl;
				if (revents[i].data.fd > _highSocket)
					_ClientList.erase(revents[i].data.fd);
			}
			
		}
		// change and use a deaueu to client* for client to procced
		for (std::map<int, Client>::iterator it = _ClientList.begin() ; it != _ClientList.end(); ++it)
		{
			if (it->second.cstatus == GATHERED)
				process_rq(it->second);
			else if (it->second.cstatus == ERROR)
				process_rq_error(it->second);
		}
	}
}


// process event
	// 	for (int i = 0; i < n_event; ++i)
	// 	{
	// 		Client cl;
			
	// 		cl.accept(revents[i].data.fd);

	// 		try
	// 		{
	// 		// read and parse request
			
	// 			std::string	raw = _read_request(cl.getFd());
	// 			cl.request.build(raw);
	// 		std::clog << cl.request << std::endl;

	// 		// special instruction : execute shutdown
	// 			if (cl.request.getUri() == "/shutdown")
	// 				g_status = 0;
	// 			if (cl.request.getUri() == "/throw")
	// 				throw std::runtime_error("404 Bof");
	// 			if (cl.request.getUri() == "/fatal")
	// 				throw std::runtime_error("415 Bof");
	// 			v_host_ptr	host = _selectServer(_SocketServersList[revents[i].data.fd], cl.request);
	// 		// std::cout << host << std::endl;
	// 		// prepare response based on request, there should be GET/HEAD/POST
	// 			// std::string	response = GET("data/default_page/index.html");
	// 			std::string	response = "response\r\n\r\n"; //Method(rq, host);

	// 		//	send response to client
	// 			_send_response(cl.getFd(), response);
	// 		}
	// 		catch (std::exception & e)
	// 		{
	// 		//	exception here should be interpreted :
	// 		//	and proper response should be send to client
	// 		//	aka GET to proper error file
	// 		//	plus maybe error should be log into log file
	// 			std::clog << e.what() << std::endl;

	// 			std::cerr << "errno value: " << errno << std::endl;
	// 			std::cerr << strerror(errno) << std::endl;
				
	// 			std::string	status(e.what(), 3);
	// 			// if (status == 0)
	// 			// 	status = 500;
	// 			std::string response = GET_error2(status);

	// 			_send_response(cl.getFd(), response);
	// 		}
	// 	}