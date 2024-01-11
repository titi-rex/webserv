/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_init.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 22:41:44 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/11 18:02:05 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"


bool	sk_used(std::map<int, SocketServer>& SocketServersList, v_host_ptr v_host, uint32_t addr, uint16_t port)
{
	for (std::map<int, SocketServer>::iterator it = SocketServersList.begin(); it != SocketServersList.end(); ++it)
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
 * @throw: fatal
 */
void	WebServer::_SocketServerList_init(void)
{
	for (size_t i = 0; i < _virtualHost.size(); ++i)
	{
	std::clog << "v_host " <<  _virtualHost[i] << std::endl;
		uint32_t	haddr = Socket::hstrtoint(_virtualHost[i].host_port.first);
		uint16_t	hport = _virtualHost[i].host_port.second;
		
		if (sk_used(_SocketServersList, &_virtualHost[i], htonl(haddr), htons(hport)))
			continue ;

		SocketServer	tmp(haddr, hport);
		int				key = tmp.getFd();

		_SocketServersList[key] = tmp;
		_SocketServersList[key].bind();
		_SocketServersList[key].listen(BACKLOG);
		_SocketServersList[key].v_hosts.push_front(&_virtualHost[i]);
		
	std::clog << "added: " << _SocketServersList[key] << std::endl << std::endl;
		_highSocket = key;
	}
// std::clog << "sokList: " << _SocketServersList << std::endl;
std::clog << "hightsocket: " << _highSocket << std::endl;
}


/**
 * @brief wrapper for epoll_ctl
 * @throw: fatal
 * @param fd target to monitor
 */
void	WebServer::modEpollList(int fd, int op, uint32_t events)
{
	struct epoll_event	e;

	e.events = events;
	e.data.fd = fd;
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
 * @throw: fatal
 */
void	WebServer::_epoll_init(void)
{
// create epoll instance
	_efd = epoll_create(1);
	if (_efd == -1)
		throw std::runtime_error("610: cannot create epoll instance");

// add interest to epoll
	for (std::map<int, SocketServer>::iterator it = _SocketServersList.begin(); it != _SocketServersList.end(); ++it)
	{
		modEpollList(it->first, EPOLL_CTL_ADD, EPOLLIN);		
		std::clog << "fd : " << it->first << " for " << _SocketServersList[it->first].getName() << " add to epoll list" << std::endl;
	}
}
