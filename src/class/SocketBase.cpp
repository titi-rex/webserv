/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketBase.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 16:32:26 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/05 17:24:51 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketBase.hpp"


//  OFC
SocketBase::SocketBase(void) : _fd(-1), _flags(0), _name("nameless") 
{
	std::memset(&_sin, 0, sizeof(_sin));
};

SocketBase::SocketBase(const SocketBase& src) 
{
	*this = src;
};

SocketBase&	SocketBase::operator=(const SocketBase& src) 
{
	if (this == &src)
		return (*this);
	_fd = src._fd;
	_sin.sin_family = src._sin.sin_family;
	_sin.sin_addr.s_addr = src._sin.sin_addr.s_addr;
	_sin.sin_port = src._sin.sin_port;
	this->setName();
	return (*this);
};

SocketBase::~SocketBase(void) {};


// socket + bind 

// custom const
SocketBase::SocketBase(int family, uint32_t addr, uint16_t port, int flags = SOCK_STREAM, int protocol = 0)
{
	const short int	l = 1; //not short ?
	
	std::memset(&_sin, 0, sizeof(_sin));
	setSin(family, addr, port);
	_flags = flags;
	_fd = socket(_sin.sin_family, flags, protocol);
	if (_fd == -1)
		throw std::runtime_error("599: cannot create socket");
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &l, sizeof(l)) == -1)
		throw std::runtime_error("599: cannot change socket option");
};



//  set
void	SocketBase::setFd(int fd) { this->_fd = fd; };

void	SocketBase::setSin(int family, uint32_t addr, uint16_t port) 
{
	_sin.sin_family = family;
	_sin.sin_addr.s_addr = htonl(addr);
	_sin.sin_port = htons(port);
};

void	SocketBase::setName(void) 
{
	std::stringstream	strs;
	uint32_t	host = ntohl(_sin.sin_addr.s_addr);
	uint16_t	port = ntohs(_sin.sin_port);

	strs << "c" << (host >> 24) << "." << ((host << 8) >> 24) << "." << ((host << 16) >> 24) << "." << ((host << 24) >> 24) << ":" << port;
	_name = strs.str(); 
};

// get
int	SocketBase::getFd(void) const { return (this->_fd); };
const std::string&			SocketBase::getName(void) const { return (this->_name); };
const struct sockaddr_in&	SocketBase::getSin(void) const { return (this->_sin); };

// 

bool	SocketBase::operator==(SocketBase& ref) 
{
	if (_sin.sin_family != ref._sin.sin_family)
		return (false);
	if (_sin.sin_addr.s_addr != ref._sin.sin_addr.s_addr)
		return (false);
	if (_sin.sin_port != ref._sin.sin_port)
		return (false);
	return (true);
};
	


// static function
uint32_t SocketBase::hstrtoint(std::string host)
{
	std::istringstream	iss(host);
	uint32_t	byte[4] = {0, 0, 0, 0};
	char		trash;

	iss >> byte[0] >> trash;
	iss >> byte[1] >> trash;
	iss >> byte[2] >> trash;
	iss >> byte[3];
	return ((byte[0] << 24) | (byte[1] << 16) | (byte[2] << 8) | byte[3]);
}

std::string	SocketBase::hintostr(uint32_t raw)
{
	std::stringstream	strs;

	strs << (raw >> 24) << "." << ((raw << 8) >> 24) << "." << ((raw << 16) >> 24) << "." << ((raw << 24) >> 24);
	return (strs.str());
}

std::string	SocketBase::str_sock_family(const struct sockaddr_in& sock)
{
	switch (sock.sin_family)
	{
		case AF_UNIX:
			return ("AF_UNIX");
		case AF_INET:
			return ("AF_INET");
		case AF_INET6:
			return ("AF_INET6");
		default:
			return ("UNKNOW");
	}
}

