/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_connect.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 23:11:38 by tlegrand          #+#    #+#             */
/*   Updated: 2024/02/05 15:49:04 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

extern sig_atomic_t	g_status;

void	WebServer::handle_epollerr(int event_id, uint32_t event_mask)
{
	logWARNING << "fd: " << event_id;
	if (event_mask & EPOLLERR)
		logWARNING << "unexpected error";
	else
		logWARNING << "unexpected close of fd";
	logWARNING << strerror(errno);
	throw std::runtime_error("615: epollerr or hang up");
}

void	WebServer::handle_epollin(int event_id)
{
	logDEBUG << "epollin ";

	if (_SocketServersList.count(event_id))
		addClient(event_id);	//throw FATAL 
	else if (_fdCgi.count(event_id))
	{
		logDEBUG << "reading cgi.." << event_id;

		Client*	cl = _fdCgi[event_id];
		if (cl->readCgi())	//read cgi output, if end, deregister cgi from epoll
		{
			logDEBUG << "all cgi has been read";
			modEpollList(event_id,	EPOLL_CTL_DEL, 0);
			close(event_id);
			_fdCgi.erase(event_id);
		}
	}
	else
	{
		Client*	cl = &_ClientList[event_id];
		if(cl->readRequest())	//throw ERROR or FATAL
		{
			logDEBUG << "end read rq";
			if (cl->getPstatus() == RL)
			{
				logDEBUG << "empty request";
				deleteClient(cl->getFd());
				return ;
			}
			cl->setKeepAlive();
			_readyToProceedList[cl->getFd()] = cl;	//add to ready list
		}
		else
			logDEBUG << "read rq continue";
	}
}

void	WebServer::handle_epollout(int event_id)
{
	logDEBUG << "epollout";

	Client*	cl = &_ClientList[event_id];

	logDEBUG << cl->getStatusStr();
	if (cl->clientStatus == PROCEEDED)
	{
		logWARNING << "request" << (Request) *cl;
		cl->sendRequest();	//throw FATAL
		if (cl->keepConnection)	//keep client
		{
			logINFO << "keep:" << *cl;
			cl->reset();
			modEpollList(cl->getFd(), EPOLL_CTL_MOD, EPOLLIN);	//Throw FATAL
			_readyToProceedList.erase(cl->getFd());
		}
		else
			deleteClient(cl->getFd());	//throw fatal
	}
}

void	WebServer::error_epoll(std::string& status, int event_id)
{
	if (_fdCgi.count(event_id))
	{
		logERROR << "PIPE ERROR FATAL, ABANDON PIPE";
		Client*	cl =  _fdCgi[event_id];
		close(event_id);
		cl->clientStatus = ERROR;
		cl->setRStrStatus("500");
	}
	else if (_SocketServersList.count(event_id))
	{
		logERROR << "couldn't accept new client";
	}
	else if (_httpStatus.count(status)) 	
	{
		_ClientList[event_id].clientStatus = ERROR;
		_ClientList[event_id].setRStrStatus(status);
		modEpollList(event_id, EPOLL_CTL_MOD, EPOLLOUT);
		_readyToProceedList[event_id] = &_ClientList[event_id];
	}
	else
	{
		logERROR << "ERROR FATAL, ABANDON CLIENT";
		deleteClient(event_id);
	}
}

void	WebServer::process_rq(Client &cl)
{
	logDEBUG << "request proceed";
	_selectServer(_SocketServersList[cl.getServerEndPoint()], cl);
	logWARNING << "request" << (Request)cl;
	Method(cl);
	if (cl.clientStatus == PROCEEDED)
		modEpollList(cl.getFd(), EPOLL_CTL_MOD, EPOLLOUT);
}

void	WebServer::process_rq_error(Client &cl)
{
	logDEBUG << "error proceed";
	try
	{
		getError(cl.getRStrStatus(), cl);	//throw fatal 
		cl.clientStatus = PROCEEDED;
		modEpollList(cl.getFd(), EPOLL_CTL_MOD, EPOLLOUT);
	}
	catch(const std::exception& e)
	{
		logERROR << e.what();
		logERROR << "ERROR FATAL, ABANDON CLIENT";
		try
		{
			modEpollList(cl.getFd(), EPOLL_CTL_DEL, 0);	// throw fatal
			close(_ClientList[cl.getFd()].getFd());
			_readyToProceedList[cl.getFd()] = NULL;
			logINFO << "deleted: " << cl;
			_ClientList.erase(cl.getFd());
		}
		catch(const std::exception& ef)
		{
			std::cerr << ef.what() << ", there is nothing left to do.." << std::endl;
		}
	}
}


/**
 * @brief main function
 * 	wait for epoll event, get request and proceed it
 * 	throw: should not throw or only to quit properly programme
 */
void	WebServer::run(void)
{
	struct epoll_event	revents[MAX_EVENTS];
	std::memset(revents, 0, sizeof(revents));
	while (g_status)
	{
		logINFO << "Waiting for event..";
		int n_event = epoll_wait(_efd, revents, MAX_EVENTS, TIMEOUT);
		if (g_status == 0)
			break ;
		if (n_event == -1)
			throw std::runtime_error("614: epoll_wait failed");
	// process event
		for (int i = 0; i < n_event; ++i)
		{
			try 
			{
				if (revents[i].events & EPOLLERR || revents[i].events & EPOLLHUP)
					handle_epollerr(revents[i].data.fd, revents[i].events);
				else if (revents[i].events & EPOLLIN)
					handle_epollin(revents[i].data.fd);
				else if (revents[i].events & EPOLLOUT)
					handle_epollout(revents[i].data.fd);
			}
			catch (std::exception & e) 
			{	
				logDEBUG << "epoll catch" << e.what();
				std::string	status(e.what());
				status.erase(3, status.size());
				error_epoll(status, revents[i].data.fd);
			}
		}
	// process request
		for (MapFdClientPtr_t::iterator it = _readyToProceedList.begin() ; it != _readyToProceedList.end(); ++it)
		{
			try
			{
				if (it->second == NULL)
					continue;
				else if (it->second->clientStatus == GATHERED || it->second->clientStatus == CGIOK)
					process_rq(*it->second);
				else if (it->second->clientStatus == ERROR)
					process_rq_error(*it->second);
			}
			catch(const std::exception& e)
			{
				logDEBUG << "process catch" << e.what();
				std::string	status(e.what());
				status.erase(3, status.size());
				it->second->setRStrStatus(status, &_httpStatus);
				process_rq_error(*it->second);
			}
		}
	}
}

