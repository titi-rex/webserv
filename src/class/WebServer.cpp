/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 21:59:05 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/13 13:22:33 by jmoutous         ###   ########lyon.fr   */
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
	for (std::map<int, Socket>::iterator it = _socketsList.begin(); it != _socketsList.end(); ++it)
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
	std::vector<std::string> 	fileVec;
	uintptr_t					i = 0;
	
	std::ifstream file(path.c_str());
    if (!file.is_open()) 
		throw std::runtime_error("Error : could not open configuration file");
	
	std::string line;
    while (std::getline(file, line)) {
		fileVec.push_back(line);
    }
	
	while (i < fileVec.size())
   	{
		if (this->parseConf(fileVec[i]) == 1)
			this->findServ(fileVec, &i);
		++i;
		
   	}
	this->debugServ();
	
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
};
	
void WebServer::addVirtualHost(const t_virtual_host& virtualHost) 
{
	_virtualHost.push_back(virtualHost);
}


std::ostream&	operator<<(std::ostream &os, const v_host_ptr v_host)
{
	os << "id : " << v_host->sId << ", default name: " << v_host->serverNames[0];
	os << ", listen on : " << v_host->host_port.first << ":" << v_host->host_port.second << std::endl;
	return (os);
}

std::ostream&	operator<<(std::ostream &os, const t_virtual_host& v_host)
{
	os << "id : " << v_host.sId << ", default name: " << v_host.serverNames[0];
	os << ", listen on : " << v_host.host_port.first << ":" << v_host.host_port.second << std::endl;
	return (os);
}
