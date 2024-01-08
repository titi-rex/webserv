/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 16:32:26 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/05 18:21:15 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"


//  OFC
Socket::Socket(void) : _fd(-1), _flags(0), _name("nameless") 
{
	std::memset(&_sin, 0, sizeof(_sin));
};

Socket::Socket(const Socket& src) {*this = src;};

Socket&	Socket::operator=(const Socket& src) 
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

Socket::~Socket(void) {};


// custom const
Socket::Socket(int family, uint32_t haddr, uint16_t hport, int flags, int protocol)
{
	const int	l = 1; //not short ?
	
	std::memset(&_sin, 0, sizeof(_sin));
	_flags = flags;
	setSin(family, haddr, hport);
	setName();
	_fd = socket(_sin.sin_family, flags, protocol);
	if (_fd == -1)
		throw std::runtime_error("599: cannot create SocketServer");
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &l, sizeof(l)) == -1)
		throw std::runtime_error("599: cannot change SocketServer option");
};


//  set
void	Socket::setFd(int fd) { this->_fd = fd; };

void	Socket::setSin(int family, uint32_t addr, uint16_t port) 
{
	_sin.sin_family = family;
	_sin.sin_addr.s_addr = htonl(addr);
	_sin.sin_port = htons(port);
};

void	Socket::setName(void) 
{
	std::stringstream	strs;
	uint32_t	host = ntohl(_sin.sin_addr.s_addr);
	uint16_t	port = ntohs(_sin.sin_port);

	strs << (host >> 24) << "." << ((host << 8) >> 24) << "." << ((host << 16) >> 24) << "." << ((host << 24) >> 24) << ":" << port;
	_name = strs.str(); 
};

// get
int	Socket::getFd(void) const { return (this->_fd); };
const std::string&			Socket::getName(void) const { return (this->_name); };
const struct sockaddr_in&	Socket::getSin(void) const { return (this->_sin); };


// operator
bool	Socket::operator==(Socket& ref) 
{
	if (_sin.sin_family != ref._sin.sin_family)
		return (false);
	if (_sin.sin_addr.s_addr != ref._sin.sin_addr.s_addr)
		return (false);
	if (_sin.sin_port != ref._sin.sin_port)
		return (false);
	return (true);
};

// custom func
void	Socket::bind(void)
{
	if (::bind(_fd, (struct sockaddr*) &_sin, sizeof(_sin)) == -1)
		throw std::runtime_error("599: socket error bind");
}

void	Socket::connect(void)
{
	if (::connect(_fd, (struct sockaddr*) &_sin, sizeof(_sin)) == -1)
		throw std::runtime_error("599: socket error connect");
}

void	Socket::listen(int backlog)
{
	if (::listen(_fd, backlog) == -1)
		throw std::runtime_error("599: socket error listen");
}

void	Socket::accept(int sock_fd)
{
	socklen_t len = sizeof(_sin);

	_fd = ::accept(sock_fd, (struct sockaddr*) &_sin, &len);
	if (_fd == -1)
		throw std::runtime_error("599: socket error accept");
}

// static function
uint32_t Socket::hstrtoint(std::string host)
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

std::string	Socket::hintostr(uint32_t raw)
{
	std::stringstream	strs;

	strs << (raw >> 24) << "." << ((raw << 8) >> 24) << "." << ((raw << 16) >> 24) << "." << ((raw << 24) >> 24);
	return (strs.str());
}

std::string	Socket::str_sock_family(const struct sockaddr_in& sock)
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

std::ostream& operator<<(std::ostream& os, const Socket& socket)
{
	os << "fd : " << socket.getFd() << ", host : " << socket.getName() << std::endl;
	return (os);
}


std::ostream&	operator<<(std::ostream &os, const struct sockaddr_in& sock)
{
	os << "Socket family : " << Socket::str_sock_family(sock) << std::endl;
	os << "Socket host : " << Socket::hintostr(ntohl(sock.sin_addr.s_addr)) << std::endl;
	os << "Socket port : " << ntohs(sock.sin_port) << std::endl;
	return (os);
}
