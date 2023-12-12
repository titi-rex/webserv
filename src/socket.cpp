/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 22:41:44 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/12 10:54:48 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

uint32_t hstrtoint(std::string host)
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

std::string	hintostr(uint32_t raw)
{
	std::stringstream	strs;

	strs << (raw >> 24) << "." << ((raw << 8) >> 24) << "." << ((raw << 16) >> 24) << "." << ((raw << 24) >> 24);
	return (strs.str());
}

std::string	str_sock_family(const struct sockaddr_in& sock)
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

std::ostream&	operator<<(std::ostream &os, const struct sockaddr_in& sock)
{
	os << "Socket family : " << str_sock_family(sock) << std::endl;
	os << "Socket host : " << hintostr(ntohl(sock.sin_addr.s_addr)) << std::endl;
	os << "Socket port : " << ntohs(sock.sin_port) << std::endl;
	return (os);
}

void	close_n_throw(int fd, std::string str)
{
	close(fd);
	throw std::runtime_error(str);
}

int	sock_init(std::string host, in_port_t port, int backlog)
{
	struct sockaddr_in	sin;
	const int			l = 1;
	int					new_sock_fd;
	
	new_sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	if (new_sock_fd == -1)
		throw std::runtime_error("fatal: cannot create socket");
	if (setsockopt(new_sock_fd, SOL_SOCKET, SO_REUSEADDR, &l, sizeof(int)) == -1)
		close_n_throw(new_sock_fd, "fatal: cannot change socket option");

	bzero(&sin, sizeof(sin));
	sin = (struct sockaddr_in){AF_INET, htons(port), {htonl(hstrtoint(host))}, {0}};
std::clog << sin << std::endl;

	if (bind(new_sock_fd, (struct sockaddr*) &sin, sizeof(sin)) == -1)
		close_n_throw(new_sock_fd, "fatal: cannot bind socket");
	if (listen(new_sock_fd, backlog) == -1)
		close_n_throw(new_sock_fd, "fatal: socket cannot listen");
	return(new_sock_fd);
}

/**
 * @brief create all needed socket per couple (host/port)
 * 	then store socket fd as keymap with coresponding server as value 
 * 	throw: fatal
 */
void	WebServer::_socketList_init(void)
{
	for (size_t i = 0; i < _virtualHost.size(); ++i)
	{
	std::clog << _virtualHost[i] << std::endl;
		for (size_t k = 0; k < _virtualHost[i].ports.size(); ++k)
		{
			int	new_sock_fd = sock_init(_virtualHost[i].host, _virtualHost[i].ports[k], BACKLOG);
			_socketsList[new_sock_fd] = &_virtualHost[i];
		}
	}
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
	event.events = EPOLLIN;
	for (std::map<int, v_host_ptr>::iterator it = _socketsList.begin(); it != _socketsList.end(); ++it)
	{
		event.data.fd = it->first;
		if (epoll_ctl(_efd, EPOLL_CTL_ADD, event.data.fd, &event) == -1)
			throw std::runtime_error("fatal: cannot add interest to epoll instance");
		std::clog << "fd : " << it->first << " for " << _socketsList[it->first]->host << ":" << _socketsList[it->first]->ports << " add to epoll list" << std::endl;
	}
}
