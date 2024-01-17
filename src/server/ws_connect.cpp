/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_connect.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 23:11:38 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/17 21:45:08 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

extern sig_atomic_t	g_status;

void	WebServer::handle_epoll_error(int event_id, uint32_t event_mask)
{
	logWARNING << "fd: " << event_id;
	if (event_mask & EPOLLERR)
		logWARNING << "error happen ";
	else
		logWARNING << "unexpected close of fd";

	if (event_id > _highSocket)
		deleteClient(event_id);
	else if (event_id < 0)
		close(-event_id);
}


void	WebServer::handle_epollin(int event_id)
{
	logDEBUG << "epollin ";

	if (event_id <= _highSocket && event_id > 0)
		addClient(event_id);	//throw FATAL 

	else if (event_id > 0)
	{
		Client*	cl = &_ClientList[event_id];
		
		if(cl->readRequest())	//throw ERROR or FATAL
		{
			logDEBUG << "end read rq";
		// std::cout << cl->request << std::endl;

			modEpollList(cl->getFd(), EPOLL_CTL_MOD, EPOLLOUT);	//thow FATAL
			_readyToProceedList[cl->getFd()] = cl;				//add to ready list
		}
		else
			logDEBUG << "read rq continue";
	}
	else
	{
		event_id = -event_id;
		
		logDEBUG << "reading cgi.." << event_id;

		Client*	cl = NULL;

		// find client from cgi fd, should use a cgi_fd to client map insteed
		for (MapFdClient_t::iterator it = _ClientList.begin(); it != _ClientList.end(); ++it)
		{
			if (it->second.getFd_cgi()[0] == event_id)
			{
				cl = &it->second;
				break ;
			}
		}
		if (cl->readCgi())	//read cgi output, if end, deregister cgi from epoll
		{
			logDEBUG << "all cgi has been read";
			modEpollList(event_id,	EPOLL_CTL_DEL, 0);
			close(event_id);
		}
	}
}


void	WebServer::handle_epollout(int event_id)
{
	logDEBUG << "epollout";

	Client*	cl = &_ClientList[event_id];

	logDEBUG << cl->getStatusStr();
	if (cl->cstatus == PROCEEDED)
	{
		cl->sendRequest();	//throw FATAL
		if (cl->keepConnection)	//keep client
		{
			logDEBUG << "keep client";
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
	logDEBUG << "request proceed";
	cl.host = _selectServer(_SocketServersList[cl.getServerEndPoint()], cl.request);

	
// special instruction : execute shutdown
	if (cl.request.getUri() == "/shutdown")
	{
		std::string	shutPage = "data/default_page/index.html";

		g_status = 0;
		cl.request.response = methodGet(cl.request, cl.host, shutPage);
		cl.sendRequest();
	}

	// logDEBUG << "host: " << cl.host;


// prepare response based on request, there should be GET/HEAD/POST
	cl.request.response = Method(cl, cl.request, cl.host);
	if (cl.request.getMid() == POST && cl.cstatus == GATHERED)
		cl.cstatus = CGIWAIT;
	else
		cl.cstatus = PROCEEDED;
	
	// std::clog << "response : " << std::endl << cl.request.response << std::endl;
}

void	WebServer::process_rq_error(Client &cl)
{
	logDEBUG << "error proceed";
	
	try
	{
		getError(cl.request.getRStrStatus(), cl.request);
		// cl.request.response = ERROR_500_MSG;
		cl.cstatus = PROCEEDED;
	}
	catch(const std::exception& e)
	{
		logERROR << "ERROR FATAL, ABANDON CLIENT";
		logERROR << strerror(errno);
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
	int	deb = 10;
// wait for event
	struct epoll_event	revents[MAX_EVENTS];
	std::memset(revents, 0, sizeof(revents));
	while (g_status && --deb > 0)
	{
		logINFO << "Waiting for event..";
		int n_event = epoll_wait(_efd, revents, MAX_EVENTS, TIMEOUT);
		if (n_event == -1)
			throw std::runtime_error("614: epoll_wait failed");


	// process event
		for (int i = 0; i < n_event; ++i)
		{
			logDEBUG << "event id: " << i;
			try 
			{
				if (revents[i].events & EPOLLERR || revents[i].events & EPOLLHUP)
					handle_epoll_error(revents[i].data.fd, revents[i].events);
				else if (revents[i].events & EPOLLIN)
					handle_epollin(revents[i].data.fd);
				else if (revents[i].events & EPOLLOUT)
					handle_epollout(revents[i].data.fd);
			}
			catch (std::exception & e)// 450: truc pas bien 
			{	
				usleep(500);
				logWARNING << "epoll catch";
				
				logWARNING << e.what();
				logWARNING << strerror(errno);
				
				int	err = std::atoi(e.what());
				if (err == 0)
					err = 699;
				logWARNING << "cerror code : " << err;
				
				if (err >= 600) //temp delete client
				{
					if (revents[i].data.fd > 0)
					{
						logERROR << "ERROR FATAL, ABANDON CLIENT";
						deleteClient(revents[i].data.fd);
					}
					else
					{
						logERROR << "PIPE ERROR FATAL, ABANDON PIPE";
						Client*	cl = NULL;

						for (MapFdClient_t::iterator it = _ClientList.begin(); it != _ClientList.end(); ++it)
						{
							if (it->second.getFd_cgi()[0] == -revents[i].data.fd)
							{
								cl = &it->second;
								break ;
							}
						}
						cl->cstatus = ERROR;
						cl->request.setRStrStatus("500");
					}
				}
				else
				{
					_ClientList[revents[i].data.fd].cstatus = ERROR;
					modEpollList(revents[i].data.fd, EPOLL_CTL_MOD, EPOLLOUT);
					_readyToProceedList[revents[i].data.fd] = &_ClientList[revents[i].data.fd];
				}
			}
		}

		// change and use a list to client* for client to procced
		for (MapFdClientPtr_t::iterator it = _readyToProceedList.begin() ; it != _readyToProceedList.end(); ++it)
		{
			try
			{
				if (it->second->cstatus == GATHERED || it->second->cstatus == CGIOK)
					process_rq(*it->second);
				else if (it->second->cstatus == ERROR)
					process_rq_error(*it->second);
			}
			catch(const std::exception& e)
			{
				usleep(500);
				logWARNING << "process catch";
				logWARNING << e.what();
				logWARNING << strerror(errno);

				int	err = std::atoi(e.what());
				if (err == 0)
					err = 699;
				logWARNING << "cerror code : " << err;
				it->second->request.setRStrStatus(e.what());
				it->second->request.setRstatus(err);
				
				process_rq_error(*it->second);
			}
		}
	}
}

