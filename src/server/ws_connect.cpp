/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_connect.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 23:11:38 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/17 14:12:11 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

extern sig_atomic_t	g_status;

void	WebServer::handle_epollerr(int event_id)
{
	std::cerr << "error happen " << std::endl;
	
	if (event_id > _highSocket)
		deleteClient(event_id);
	else if (event_id < 0)
		close(-event_id);
}

void	WebServer::handle_epollhup(int event_id)
{
	std::cerr << "unexpected close of socket" << std::endl;
	if (event_id > _highSocket)
		deleteClient(event_id);
	else if (event_id < 0)	//pipe, if client still exist, should set cstatus to ERROR with request status 500
		close(-event_id);
}


void	WebServer::handle_epollin(int event_id)
{
	std::cout << "epollin " << std::endl;

	if (event_id <= _highSocket)
		addClient(event_id);	//throw FATAL 

	else if (event_id > 0)
	{
		Client*	cl = &_ClientList[event_id];
		
		if(cl->readRequest())	//throw ERROR or FATAL
		{
			std::cout << "end read rq" << std::endl;
		std::cout << cl->request << std::endl;

			modEpollList(cl->getFd(), EPOLL_CTL_MOD, EPOLLOUT);	//thow FATAL
			_readyToProceedList[cl->getFd()] = cl;				//add to ready list
		}
		else
			std::cout << "read rq continue" << std::endl;
	}
	else
	{
		std::cout << "reading cgi.." << std::endl;

		Client*	cl = NULL;

		// find client from cgi fd, should use a cgi_fd to client map insteed
		for (MapFdClient_t::iterator it = _ClientList.begin(); it != _ClientList.end(); ++it)
		{
			if (it->second.getFd_cgi()[0] == -event_id)
			{
				cl = &it->second;
				break ;
			}
		}
		if (cl->readCgi())	//read cgi output, if end, deregister cgi from epoll
		{
			modEpollList(-event_id,	EPOLL_CTL_DEL, 0);
			close(-event_id);
		}
	}
}


void	WebServer::handle_epollout(int event_id)
{
	std::cout << "epollout" << std::endl;

	Client*	cl = &_ClientList[event_id];

	if (cl->cstatus == PROCEEDED)
	{
		cl->sendRequest();	//throw FATAL
		if (cl->keepConnection)	//keep client
		{
			std::clog << "keep client" << std::endl;
			cl->reset();
			modEpollList(cl->getFd(), EPOLL_CTL_MOD, EPOLLIN);	//Throw FATAL
			_readyToProceedList.erase(cl->getFd());
		}
		else	//delete client
			deleteClient(cl->getFd());	//throw fatal
	}
}


void	WebServer::process_rq(Client &cl)
{
	std::cout << "request proceed" << std::endl;
	cl.host = _selectServer(_SocketServersList[cl.getServerEndPoint()], cl.request);

	
// special instruction : execute shutdown
	if (cl.request.getUri() == "/shutdown")
	{
		std::string	shutPage = "data/default_page/index.html";

		g_status = 0;
		cl.request.response = methodGet(cl.request, cl.host, shutPage);
		cl.sendRequest();
	}
	if (cl.request.getUri() == "/throw")
		throw std::runtime_error("404 Bof error");
	if (cl.request.getUri() == "/fatal")
		throw std::runtime_error("699 Bof fatal");

std::cout << "host: " << cl.host << std::endl;


// prepare response based on request, there should be GET/HEAD/POST
	cl.request.response = Method(cl, cl.request, cl.host);
	cl.cstatus = PROCEEDED;
	std::clog << " response : " << std::endl << cl.request.response << std::endl;
}

void	WebServer::process_rq_error(Client &cl)
{
	std::cout << "error proceed" << std::endl;
	
	try
	{
		getError(cl.request.getRStrStatus(), cl.request);
		// cl.request.response = ERROR_500_MSG;
		cl.cstatus = PROCEEDED;
	}
	catch(const std::exception& e)
	{
		std::cerr << "ERROR FATAL, ABANDON CLIENT" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		deleteClient(cl.getFd());
	}
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
			throw std::runtime_error("614: epoll_wait failed");
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
				else if (revents[i].events & EPOLLOUT)
					handle_epollout(revents[i].data.fd);
			}
			catch (std::exception & e)// 450: truc pas bien 
			{	
				std::cerr << e.what() << std::endl;
				std::cerr << strerror(errno) << std::endl;
				
				int	err = std::atoi(e.what());
				if (err == 0)
					err = 699;
				std::cerr << "cerror code : " << err << std::endl;
				
				if (err >= 600) //temp delete client
				{
					std::cerr << "ERROR FATAL, ABANDON CLIENT" << std::endl;
					deleteClient(revents[i].data.fd);
				}
				else
				{
					_ClientList[revents[i].data.fd].cstatus = ERROR;
					modEpollList(revents[i].data.fd, EPOLL_CTL_MOD, EPOLLOUT);
					_readyToProceedList[revents[i].data.fd] = &_ClientList[revents[i].data.fd];
				}
			}
		}

		if (_readyToProceedList.size())
			std::clog << "n client ready :" << _readyToProceedList.size() << std::endl;

		// change and use a list to client* for client to procced
		for (MapFdClientPtr_t::iterator it = _readyToProceedList.begin() ; it != _readyToProceedList.end(); ++it)
		{
			std::clog << "ready: " << *it->second << std::endl;
			try
			{
				if (it->second->cstatus == GATHERED)
					process_rq(*it->second);
				else if (it->second->cstatus == ERROR)
					process_rq_error(*it->second);
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << std::endl;
				std::cerr << strerror(errno) << std::endl;
				
				int	err = std::atoi(e.what());
				if (err == 0)
					err = 699;
				std::cerr << "cerror code : " << err << std::endl;
				it->second->request.setRStrStatus(e.what());
				it->second->request.setRstatus(err);
				
				process_rq_error(*it->second);
			}
		}
	}
}

