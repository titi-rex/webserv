/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 16:16:09 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/09 12:38:49 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void) : _serverEndPoint(-1), _fd_cgi(-1), cstatus(CREATED) 
{
	_name = "cnameless";
};

Client::Client(const Client& src) : Socket(src)
{
	this->setFd_cgi(src.getFd_cgi());
	_serverEndPoint = src._serverEndPoint;
	request = src.request;
	cstatus = src.cstatus;
	
};

Client&	Client::operator=(const Client& src) 
{
	if (this == &src)
		return (*this);
	this->Socket::operator=(src);
	this->setFd_cgi(src.getFd_cgi());
	_serverEndPoint = src._serverEndPoint;
	request = src.request;
	cstatus = src.cstatus;
	return (*this);
};

Client::~Client(void) {};



int		Client::getServerEndPoint(void) const { return (this->_serverEndPoint); };
int		Client::getFd_cgi(void) const { return (this->_fd_cgi); };

void	Client::setFd_cgi(int fd_cgi) { this->_fd_cgi = fd_cgi; };


void	Client::accept(int socketServerFd)
{
	_serverEndPoint = socketServerFd;
	this->Socket::accept(socketServerFd);
	this->setName();
	cstatus = ACCEPTED;
}

bool	Client::readRequest(void)
{
	char	buf[BUFFER_SIZE + 1] = {0};
	int		n_rec = 0;
	
	n_rec = recv(_fd, &buf, BUFFER_SIZE, MSG_DONTROUTE | MSG_CMSG_CLOEXEC);
	if (n_rec == -1)
		throw std::runtime_error("recv");
	buf[n_rec] = 0;
	request.build(buf);
	if (true)
	{
		cstatus = GATHERED;
		return (true);
	}
	return (false);
}


void	Client::sendRequest(void)
{

}

void	Client::reset(void)
{
	
}

std::ostream&	operator<<(std::ostream& os, const Client& Client)
{
	os << "Client socket, fd : " << (Socket) Client << std::endl;
	return (os);
};
