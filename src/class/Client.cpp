/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 16:16:09 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/15 15:25:15 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void) : _serverEndPoint(-1), _fd_cgi(-1), _sizeLimit(1024), cstatus(CREATED), keepConnection(false)
{
	_name = "cnameless";
};

Client::Client(size_t bodyLimit) : _serverEndPoint(-1), _fd_cgi(-1), _sizeLimit(bodyLimit), cstatus(CREATED), keepConnection(false)
{
	_name = "cnameless";
};

Client::Client(const Client& src) : Socket(src)
{
	this->setFd_cgi(src.getFd_cgi());
	_serverEndPoint = src._serverEndPoint;
	request = src.request;
	cstatus = src.cstatus;
	keepConnection = src.keepConnection;
	
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
	keepConnection = src.keepConnection;
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

void	Client::_checkRequestSize(Request& rq)
{
	if (rq.getPstatus() == BODYCHUNK)
	{
		if (rq.getBody().size() > _sizeLimit)
			throw std::runtime_error("413: Request Entity Too Large");
		
	}
	else if (rq.getPstatus() == BODYCLENGTH)
	{	
		if (rq._bodySizeExpected > _sizeLimit)
			throw std::runtime_error("413: Request Entity Too Large");
	}
}

bool	Client::readRequest(void)
{
	std::cout << "client reading request" << std::endl;
	
	char	buf[BUFFER_SIZE + 1] = {0};
	int		n_rec = 0;
	
	n_rec = recv(_fd, &buf, BUFFER_SIZE, MSG_DONTWAIT | MSG_CMSG_CLOEXEC);
	if (n_rec == -1)
		throw std::runtime_error("620: recv");
	buf[n_rec] = 0;
	_checkRequestSize(request);
	if (request.build(buf))// throw ERROR or FATAL
	{
		cstatus = GATHERED;
		return (true);
	}
	return (false);
}

bool	Client::readCgi(void)
{
	std::cout << "client reading Cgi" << std::endl;
	
	char	buf[BUFFER_SIZE + 1] = {0};
	int		n_rec = 0;
	bool	end = false;
	
	n_rec = read(_fd_cgi, &buf, BUFFER_SIZE);
	if (n_rec == -1)
		throw std::runtime_error("699: read cgi");
	if (n_rec < BUFFER_SIZE || buf[n_rec] == '\0')
		end = true;
	buf[n_rec] = 0;
	if (request.addCgi(buf) || end)//throw ERROR or FATAL
	{
		cstatus = CGIOK;
		return (true);
	}
	return (false);
}


void	Client::proceedRequest(void)
{
	std::cout << "method happen here" << std::endl;
}



void	Client::sendRequest(void)
{
	if (send(_fd, request.response.c_str() , request.response.length(), MSG_DONTWAIT) == -1)
		throw std::runtime_error("621: send");
	cstatus = SENT;
}

void	Client::reset(void)
{
	this->request.clear();
	cstatus = ACCEPTED;
}


/**
 * @brief format client/fd/host
 */
std::ostream&	operator<<(std::ostream& os, const Client& Client)
{
	os << "c/" << Client.getFd() << "/" << Client.getName();
	return (os);
};
