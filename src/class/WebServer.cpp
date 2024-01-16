/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 21:59:05 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/16 15:48:15 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

WebServer::WebServer(void) : _efd(-1), _bodySizeLimit(1024), _dirErrorPage("/data/default_pages") {};

WebServer::WebServer(const WebServer& src) 
{
	*this = src;
	
};

WebServer&	WebServer::operator=(const WebServer& src) 
{
	if (this == &src)
		return (*this);
	_efd = src._efd;
	_bodySizeLimit = src._bodySizeLimit;
	_dirErrorPage = src._dirErrorPage;
	_virtualHost = src._virtualHost;
	_errorPage = src._errorPage;
	_SocketServersList = src._SocketServersList;
	_highSocket = src._highSocket;
	_ClientList = src._ClientList;
	_readyToProceedList = src._readyToProceedList;
	_envCGI = src._envCGI;	
	return (*this);
};

WebServer::~WebServer(void) 
{
	for (std::map<int, SocketServer>::iterator it = _SocketServersList.begin(); it != _SocketServersList.end(); ++it)
	{
		close(it->first);
		std::clog << "deleted: " << it->second << std::endl;
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

WebServer::WebServer(std::string path) : _efd(-1), _bodySizeLimit(1024), _dirErrorPage("/data/default_pages")
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
	file.close();

	this->debugServ();
	
	try
	{
		_SocketServerList_init();
		_epoll_init();
	}
	catch(const std::exception& e)
	{
		// std::cerr << e.what() << std::endl;
		throw std::runtime_error(e.what());
	}	
};
	
void WebServer::addVirtualHost(const t_virtual_host& virtualHost) 
{
	_virtualHost.push_back(virtualHost);
}

/**
 * @brief format id/names/host
 */
std::ostream&	operator<<(std::ostream &os, const v_host_ptr v_host)
{
	
	os << v_host->sId << "/" << v_host->serverNames << "/" << v_host->host_port.first << ":" << v_host->host_port.second;
	return (os);
}


/**
 * @brief format id/names/host
 */
std::ostream&	operator<<(std::ostream &os, const t_virtual_host& v_host)
{
	os << v_host.sId << "/" << v_host.serverNames << "/" << v_host.host_port.first << ":" << v_host.host_port.second;
	return (os);
}
