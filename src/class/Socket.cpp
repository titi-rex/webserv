/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 12:15:27 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/05 16:39:06 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
# include <string>


Socket::Socket(void) : _fd(-1), _host(0), _port(0) {};

Socket::Socket(const Socket& src) : _fd(src._fd), _host(src._host), _port(src._port), _name(src._name), v_hosts(src.v_hosts) {};

Socket&	Socket::operator=(const Socket& src) 
{
	if (this == &src)
		return (*this);
	_fd = src._fd;
	_host = src._host;
	_port = src._port;
	_name = src._name;
	v_hosts = src.v_hosts; 
	return (*this);
};

Socket::~Socket(void) {};

Socket::Socket(uint32_t host, uint16_t port) : _fd(-1), _host(host), _port(port) { setName(); };



int			Socket::getFd(void) const { return (this->_fd); };
void		Socket::setFd(int fd)  { _fd = fd; };
uint32_t	Socket::getHost(void) const { return (this->_host); };
uint16_t	Socket::getPort(void) const { return (this->_port); };
std::string	Socket::getName(void) const { return (_name); };

void	Socket::setName(void) 
{
	std::stringstream	strs;

	strs << (_host >> 24) << "." << ((_host << 8) >> 24) << "." << ((_host << 16) >> 24) << "." << ((_host << 24) >> 24) << ":" << _port;
	_name = strs.str();
};


bool	Socket::operator==(Socket& ref) 
{
	if (_host == ref._host && _port == ref._port)
		return (true);
	return (false);
};

bool	Socket::is_already_used(std::map<int, Socket>& socketsList, v_host_ptr v_host)
{
	for (std::map<int, Socket>::iterator it = socketsList.begin(); it != socketsList.end(); ++it)
	{
		if (it->second == *this)
		{
			it->second.v_hosts.push_back(v_host);
			return (true);
		}
	}
	return (false);
};
	
void	Socket::sockInit(int backlog)
{
	struct sockaddr_in	sin;
	const int			l = 1;
	
	_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	if (_fd == -1)
		throw std::runtime_error("fatal: cannot create socket");
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &l, sizeof(int)) == -1)
		throw std::runtime_error("fatal: cannot change socket option");

	std::memset(&sin, 0, sizeof(sin));
	sin = (struct sockaddr_in){AF_INET, htons(_port), {htonl(_host)}, {0}};
std::clog << sin << std::endl;

	if (bind(_fd, (struct sockaddr*) &sin, sizeof(sin)) == -1)
		throw std::runtime_error("fatal: cannot bind socket");
	if (listen(_fd, backlog) == -1)
		throw std::runtime_error("fatal: socket cannot listen");
}


std::ostream& operator<<(std::ostream& os, const Socket& socket)
{
	os << "fd : " << socket.getFd() << ", host : " << socket.getName() << ", for server : ";
	for (size_t i = 0; i < socket.v_hosts.size(); ++i)
		os << socket.v_hosts[i]->serverNames[0] << " ";
	os << std::endl;
	return (os);
}

std::ostream&	operator<<(std::ostream &os, const struct sockaddr_in& sock)
{
	os << "Socket family : " << SocketBase::str_sock_family(sock) << std::endl;
	os << "Socket host : " << SocketBase::hintostr(ntohl(sock.sin_addr.s_addr)) << std::endl;
	os << "Socket port : " << ntohs(sock.sin_port) << std::endl;
	return (os);
}
