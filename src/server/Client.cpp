/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 16:16:09 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/25 14:30:17 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void) : _serverEndPoint(-1), _sizeLimit(1024), clientStatus(CREATED), keepConnection(false)
{
	_fd_cgi[0] = -1;
	_fd_cgi[1] = -1;
	_name = "cnameless";
};

Client::Client(size_t bodyLimit) : _serverEndPoint(-1), _sizeLimit(bodyLimit), clientStatus(CREATED), keepConnection(false)
{
	_fd_cgi[0] = -1;
	_fd_cgi[1] = -1;
	_name = "cnameless";
};

Client::Client(const Client& src) : Socket(src), Request(src)
{
	*this = src;
};

Client&	Client::operator=(const Client& src) 
{
	if (this == &src)
		return (*this);
	this->Socket::operator=(src);
	this->Request::operator=(src);
	_serverEndPoint = src._serverEndPoint;
	_fd_cgi[0] = src._fd_cgi[0];
	_fd_cgi[1] = src._fd_cgi[1];
	_sizeLimit = src._sizeLimit;
	clientStatus = src.clientStatus;
	keepConnection = src.keepConnection;
	return (*this);
};

Client::~Client(void) {};

const std::string	Client::getStatusStr(void) const
{
	switch (this->clientStatus)
	{
		CLIENT_ENUM(CLIENT_ENUM_CASE)
		default:
			return ("UNKNOW");
	}	
}

int		Client::getServerEndPoint(void) const { return (this->_serverEndPoint); };
int*	Client::getFd_cgi(void) const { 
	return const_cast<int*>(_fd_cgi);
};

void	Client::setFd_cgi(int fd_cgi[2]) { 
	this->_fd_cgi[0] = fd_cgi[0]; 
	this->_fd_cgi[1] = fd_cgi[1]; 
};

void	Client::accept(int socketServerFd)
{
	_serverEndPoint = socketServerFd;
	this->Socket::accept(socketServerFd);
	this->setName();
	clientStatus = ACCEPTED;
}

bool	Client::readRequest(void)
{
	logDEBUG << "client reading request";

	char	buf[BUFFER_SIZE + 1];
	int		n_rec = 0;
	bool	end = true;

	std::memset(buf, 0, BUFFER_SIZE + 1);
	n_rec = recv(_fd, &buf, BUFFER_SIZE, MSG_DONTWAIT | MSG_CMSG_CLOEXEC);
	if (n_rec == -1)
		throw std::runtime_error("620: recv");
	else if (n_rec != 0)
	{
		end = build(buf);// throw ERROR or FATAL
		if (_isChunk == true && getBody().size() > _sizeLimit)
			throw std::runtime_error("413: Request Actual Body Too Large");
		else if (_isChunk == false && getBodySizeExpected() > _sizeLimit)
			throw std::runtime_error("413: Request Body Length Too Large");
	}
	if (end)
		clientStatus = GATHERED;
	return (end);
}

bool	Client::readCgi(void)
{
	logDEBUG << "client reading Cgi";
	
	char	buf[BUFFER_SIZE + 1];
	int		n_rec = 0;
	bool	end = false;

	std::memset(buf, 0, BUFFER_SIZE + 1);
	n_rec = read(_fd_cgi[0], &buf, BUFFER_SIZE);
	if (n_rec == -1)
		throw std::runtime_error("699: read cgi");
	else if (n_rec == 0)
		throw std::runtime_error("500: cgi send empty data");
	if (n_rec < BUFFER_SIZE || buf[n_rec] == '\0')
		end = true;
	buf[n_rec] = 0;
	if (addCgi(buf) || end)//throw ERROR or FATAL
	{
		clientStatus = CGIOK;
		return (true);
	}
	return (false);
}


void	Client::sendRequest(void)
{
	if (send(_fd, response.c_str() , response.size(), MSG_DONTWAIT) == -1)
		throw std::runtime_error("621: send");
	clientStatus = SENT;
}

void	Client::reset(void)
{
	clear();
	clientStatus = ACCEPTED;
}


/**
 * @brief format client/fd/host
 */
std::ostream&	operator<<(std::ostream& os, const Client& Client)
{
	os << "c/" << Client.getFd() << "/" << Client.getName();
	return (os);
};
