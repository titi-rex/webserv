/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_init.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 22:41:44 by tlegrand          #+#    #+#             */
/*   Updated: 2024/02/13 12:58:42 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"


bool	sk_used(MapFdSockServ_t& SocketServersList, vHostPtr v_host, uint32_t addr, uint16_t port)
{
	for (MapFdSockServ_t::iterator it = SocketServersList.begin(); it != SocketServersList.end(); ++it)
	{
		if (it->second.getSin().sin_addr.s_addr == addr && it->second.getSin().sin_port == port)
		{
			it->second.v_hosts.push_back(v_host);
			return (true);
		}
	}
	return (false);
};


/**
 * @brief create all needed SocketServer per couple (host/port)
 * 	then store SocketServer fd as keymap with coresponding SocketServer as value 
 * @throw fatal
 */
void	WebServer::_SocketServerList_init(void)
{
	for (size_t i = 0; i < _virtualHost.size(); ++i)
	{
		uint32_t	haddr = Socket::hstrtoint(_virtualHost[i].getHostPort().first);
		uint16_t	hport = _virtualHost[i].getHostPort().second;

		if (sk_used(_SocketServersList, &_virtualHost[i], htonl(haddr), htons(hport)))
			continue ;

		SocketServer	tmp(haddr, hport);
		int				key = tmp.getFd();

		_SocketServersList[key] = tmp;
		_SocketServersList[key].bind();
		_SocketServersList[key].listen(BACKLOG);
		_SocketServersList[key].v_hosts.push_front(&_virtualHost[i]);
		
		logINFO << "added: " << _SocketServersList[key];
	}
}


/**
 * @brief wrapper for epoll_ctl
 * @throw fatal
 * @param fd target to monitor, if fd < 0, event.data.fd = fd and then fd = -fd
 */
void	WebServer::modEpollList(int fd, int op, uint32_t events)
{
	struct epoll_event	e;

	std::memset(&e, 0, sizeof(e));
	e.events = events;
	e.data.fd = fd;
	if (fd < 0)
		fd = -fd;
	if (epoll_ctl(_efd, op, fd, &e) == 0)
		return ;
	
	switch (op)
	{
		case EPOLL_CTL_ADD:
			throw std::runtime_error("611: EPOLL_CTL_ADD");
		case EPOLL_CTL_MOD:
			throw std::runtime_error("612: EPOLL_CTL_MOD");
		case EPOLL_CTL_DEL:
			throw std::runtime_error("613: EPOLL_CTL_DEL");
		default:
			throw std::runtime_error("698: epoll_ctl wrong op");
	}
}

/**
 * @brief initiate epoll instance and populate it with SocketServer fd
 * @throw fatal
 */
void	WebServer::_epoll_init(void)
{
// create epoll instance
	_efd = epoll_create(1);
	if (_efd == -1)
		throw std::runtime_error("610: cannot create epoll instance");

// add interest to epoll
	for (MapFdSockServ_t::iterator it = _SocketServersList.begin(); it != _SocketServersList.end(); ++it)
	{
		modEpollList(it->first, EPOLL_CTL_ADD, EPOLLIN);		
	}
}

/**
 * @brief accept client connection, add it to epoll list and _clientList
 * @throw fatal
 */
void	WebServer::addClient(int socketServerFd)
{
	Client	cl(_bodySizeLimit);

	try
	{
		cl.accept(socketServerFd);
		modEpollList(cl.getFd(), EPOLL_CTL_ADD, EPOLLIN);
		_ClientList[cl.getFd()] = cl;
		logINFO << "added: " << cl;
	}
	catch(const std::exception& e)
	{
		wrap_close(cl.getFd());
		throw std::runtime_error(e.what());
	}
}

/**
 * @brief close connection (socket) and remove client from epoll list and server
 * @throw fatal
 */
void	WebServer::deleteClient(int client_fd)
{
	Client	cl = _ClientList[client_fd];

	modEpollList(client_fd, EPOLL_CTL_DEL, 0);	//del from epoll
	close(_ClientList[client_fd].getFd());	// close socket fd
	_ClientList.erase(client_fd);			// delete client from list
	_readyToProceedList.erase(client_fd);	//delete from ready list
	logINFO << "deleted: " << cl;
}
