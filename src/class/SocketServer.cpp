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


SocketServer::SocketServer(void) {};

SocketServer::SocketServer(const SocketServer& src) {*this = src;};

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



std::ostream& operator<<(std::ostream& os, const SocketServer& SocketServer)
{
	os << "fd : " << SocketServer.getFd() << ", host : " << SocketServer.getName() << ", for server : ";
	for (size_t i = 0; i < SocketServer.v_hosts.size(); ++i)
		os << SocketServer.v_hosts[i]->serverNames[0] << " ";
	os << std::endl;
	return (os);
}

std::ostream&	operator<<(std::ostream &os, const struct sockaddr_in& sock)
{
	os << "SocketServer family : " << Socket::str_sock_family(sock) << std::endl;
	os << "SocketServer host : " << Socket::hintostr(ntohl(sock.sin_addr.s_addr)) << std::endl;
	os << "SocketServer port : " << ntohs(sock.sin_port) << std::endl;
	return (os);
}
