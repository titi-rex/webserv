/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketServer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 12:15:27 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/05 18:32:51 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketServer.hpp"
# include <string>


SocketServer::SocketServer(void) 
{
	_name = "snameless";
};

SocketServer::SocketServer(const SocketServer& src) : Socket(src)
{
	*this = src;
};

SocketServer&	SocketServer::operator=(const SocketServer& src) 
{
	if (this == &src)
		return (*this);
	this->Socket::operator=(src);
	v_hosts = src.v_hosts; 
	return (*this);
};

SocketServer::~SocketServer(void) {};


SocketServer::SocketServer(uint32_t haddr, uint16_t hport) : Socket(AF_INET, haddr, hport, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC) {};


/**
 * @brief format server/fd/host
 */
std::ostream& operator<<(std::ostream& os, const SocketServer& SocketServer)
{
	os << "s/" << SocketServer.getFd() << "/" << SocketServer.getName() << "/";
	for (size_t i = 0; i < SocketServer.v_hosts.size(); ++i)
		os << SocketServer.v_hosts[i]->getServerNames().at(0) << " ";
	return (os);
}


