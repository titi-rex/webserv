/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_init.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 22:41:44 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/05 16:51:28 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

/**
 * @brief create all needed socket per couple (host/port)
 * 	then store socket fd as keymap with coresponding Socket as value 
 * 	throw: fatal
 */
void	WebServer::_socketList_init(void)
{
	for (size_t i = 0; i < _virtualHost.size(); ++i)
	{
	std::clog << _virtualHost[i] << std::endl;
		Socket	new_socket(SocketBase::hstrtoint(_virtualHost[i].host_port.first), _virtualHost[i].host_port.second);
		
		if (new_socket.is_already_used(_socketsList, &_virtualHost[i]))
			continue ;
		new_socket.sockInit(BACKLOG);
		new_socket.v_hosts.push_front(&_virtualHost[i]);
		_socketsList[new_socket.getFd()] = new_socket;
			
	}
	std::clog << "sokList: " << _socketsList << std::endl;
}

/**
 * @brief initiate epoll instance and populate it with socket fd
 * 	throw: fatal
 */
void	WebServer::_epoll_init(void)
{
// create epoll instance
	_efd = epoll_create(1);
	if (_efd == -1)
		throw std::runtime_error("fatal: cannot create epoll instance");

// add interest to epoll
	struct epoll_event event;
	std::memset(&event, 0, sizeof(event));
	event.events = EPOLLIN;
	for (std::map<int, Socket>::iterator it = _socketsList.begin(); it != _socketsList.end(); ++it)
	{
		event.data.fd = it->first;
		if (epoll_ctl(_efd, EPOLL_CTL_ADD, event.data.fd, &event) == -1)
			throw std::runtime_error("fatal: cannot add interest to epoll instance");
		std::clog << "fd : " << it->first << " for " << _socketsList[it->first].getName() << " add to epoll list" << std::endl;
	}
}
