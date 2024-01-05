/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 16:16:09 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/05 16:52:45 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void) : _fd_server(0), _fd_cgi(0), _status(0) {};

Client::Client(const Client& src) 
{
	this->set_socket(src.get_socket());
	this->set_rq(src.get_rq());
	this->setFd_server(src.getFd_server());
	this->setFd_cgi(src.getFd_cgi());
	this->setStatus(src.getStatus());
	
};

Client&	Client::operator=(const Client& src) 
{
	if (this == &src)
		return (*this);
	this->set_socket(src.get_socket());
	this->set_rq(src.get_rq());
	this->setFd_server(src.getFd_server());
	this->setFd_cgi(src.getFd_cgi());
	this->setStatus(src.getStatus());
	return (*this);
};

Client::~Client(void) {};


Request	Client::get_rq(void) const { return (this->_rq); };
void	Client::set_rq(Request _rq) { this->_rq = _rq; };

int	Client::getFd_server(void) const { return (this->_fd_server); };
void	Client::setFd_server(int fd_server) { this->_fd_server = fd_server; };

int	Client::getFd_cgi(void) const { return (this->_fd_cgi); };
void	Client::setFd_cgi(int fd_cgi) { this->_fd_cgi = fd_cgi; };

int	Client::getStatus(void) const { return (this->_status); };
void	Client::setStatus(int status) { this->_status = status; };


