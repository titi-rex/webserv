/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 21:59:05 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/11 13:02:12 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

WebServer::WebServer(void) : _bodySizeLimit(-1) {};

WebServer::WebServer(const WebServer& src) 
{
	this->setVirtualHost(src.getVirtualHost());
	// this->setErrorPage(src.getErrorPage());
	this->setDirErrorPage(src.getDirErrorPage());
	this->setBodySizeLimit(src.getBodySizeLimit());
	
};

WebServer&	WebServer::operator=(const WebServer& src) 
{
	if (this == &src)
		return (*this);
	this->setVirtualHost(src.getVirtualHost());
	// this->setErrorPage(src.getErrorPage());
	this->setDirErrorPage(src.getDirErrorPage());
	this->setBodySizeLimit(src.getBodySizeLimit());
	return (*this);
};

WebServer::~WebServer(void) 
{
	for (std::map<int, v_host_ptr>::iterator it = _socketsList.begin(); it != _socketsList.end(); ++it)
	{
		close(it->first);
	}
	close(_efd);
};


void	WebServer::setVirtualHost(std::vector<t_virtual_host> virtualHost) { 
	this->_virtualHost = virtualHost; 
};

void	WebServer::setErrorPage(std::string key, std::string value) { 
	this->_errorPage[key] = value;
};

void	WebServer::setDirErrorPage(std::string dirErrorPage) {
	this->_dirErrorPage = dirErrorPage; 
};

void	WebServer::setBodySizeLimit(size_t bodySizeLimit) { 
	this->_bodySizeLimit = bodySizeLimit;
};

std::vector<t_virtual_host>	WebServer::getVirtualHost(void) const { 
	return (this->_virtualHost); 
};

std::map<std::string,std::string>&	WebServer::getErrorPage(void) { 
	return (this->_errorPage); 
};

std::string	WebServer::getDirErrorPage(void) const { 
	return (this->_dirErrorPage); 
};

size_t	WebServer::getBodySizeLimit(void) const {
	 return (this->_bodySizeLimit); 
};

WebServer::WebServer(std::string path) 
{
	(void)path;
	
	/*
	t_virtual_host	tmp;

	tmp.sId = 0;
	tmp.host = "127.0.0.1";
	tmp.ports.push_back(8080);
	tmp.ports.push_back(8081);
	_virtualHost.push_back(tmp);
	tmp.sId = 1;
	tmp.host = "100.85.0.1";
	tmp.ports.clear();
	tmp.ports.push_back(8541);
	_virtualHost.push_back(tmp);
	
	try
	{
		_socketList_init();
		_epoll_init();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		throw std::runtime_error(e.what());
	}	
	*/
};
	
void WebServer::addVirtualHost(const t_virtual_host& virtualHost) 
{
	_virtualHost.push_back(virtualHost);
}


template <typename T>
std::ostream&	operator<<(std::ostream &os, const std::vector<T>& vec)
{
	for (size_t i = 0; i < vec.size(); ++i)
	{
		os << vec.at(i) << " ";
	}
	return (os);
}

std::ostream&	operator<<(std::ostream &os, const v_host_ptr v_host)
{
	os << "v_host id : " << v_host->sId << std::endl;
	os << "v_host host : " << v_host->host << std::endl;
	os << "v_host ports : " << v_host->ports << std::endl;
	return (os);
}

std::ostream&	operator<<(std::ostream &os, const t_virtual_host& v_host)
{
	os << "v_host id : " << v_host.sId << std::endl;
	os << "v_host host : " << v_host.host << std::endl;
	os << "v_host ports : " << v_host.ports << std::endl;
	return (os);
}
