/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 21:59:05 by tlegrand          #+#    #+#             */
/*   Updated: 2024/02/13 13:46:27 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

WebServer::WebServer(void) : _efd(-1), _bodySizeLimit(1024), _dirPrefix("/") 
{
	_initHttpStatus();
	_initContentTypeMap();
};

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
	_dirPrefix = src._dirPrefix;
	_virtualHost = src._virtualHost;
	_errorPage = src._errorPage;
	_SocketServersList = src._SocketServersList;
	_ClientList = src._ClientList;
	_readyToProceedList = src._readyToProceedList;
	_envCGI = src._envCGI;	
	return (*this);
};

WebServer::~WebServer(void) 
{
	_closeAllFd(true);
};

WebServer::WebServer(std::string path) : _efd(-1), _bodySizeLimit(1024), _dirPrefix("/") 
{
	VecStr_t 	fileVec;
	uintptr_t	i = 0;

	_initHttpStatus();
	_initContentTypeMap();
	std::ifstream file(path.c_str());
	if (!file.is_open()) 
		throw std::runtime_error("WebServer : could not open configuration file");
	std::string line;
	while (std::getline(file, line)) 
	{
		fileVec.push_back(line);
	}
	while (i < fileVec.size())
	{
		if (this->parseConf(fileVec[i]) == 1)
			this->findServ(fileVec, &i);
		++i;
	}
	file.close();
	if (_virtualHost.empty())
		throw std::runtime_error("WebServer: no server supplied");
};

void	WebServer::initSocket(void)
{
	_SocketServerList_init();
	_epoll_init();
}

void	WebServer::setVirtualHost(const VecVHost_t& vHost) 
{
	this->_virtualHost = vHost;
};

void	WebServer::setDirPrefix(const VecStr_t& sLine) 
{
	setDir(this->_dirPrefix, sLine, "Webserver", "");
	if (this->_dirPrefix.at(0) != '/')
		throw std::runtime_error("Webserver: dir_prefix not an absolut path \'" + sLine.at(1) + "\'");
};

void	WebServer::setErrorPage(const VecStr_t& sLine) 
{
	if (sLine.size() < 3)
		throw std::runtime_error("WebServer: error_page supplied but value is missing");
	if (access(sLine.at(2).c_str(), F_OK | R_OK))
		throw std::runtime_error("Webserver: error_page \'" + sLine.at(2) + "\' not accessible");
	this->_errorPage[sLine.at(1)] = sLine.at(2);
};

void	WebServer::setBodySizeLimit(const VecStr_t& sLine) 
{
	if (sLine.size() < 2)
		throw std::runtime_error("WebServer: body_size_limit supplied but value is missing");
	if (sLine.at(1).find_first_not_of("0123456789*") != std::string::npos)
		throw std::runtime_error("WebServer: body_size_limit value incorrect");
	if (sLine.at(1) == "*")
		this->_bodySizeLimit = -1;
	else
		this->_bodySizeLimit = std::strtoul(sLine.at(1).c_str(), NULL, 10);	
};

VecVHost_t	WebServer::getVirtualHost(void) const 
{
	return (this->_virtualHost); 
};

const std::string&	WebServer::getDirPrefix(void) const 
{
	return (this->_dirPrefix); 
};

const MapStrStr_t&	WebServer::getErrorPage(void) const 
{
	return (this->_errorPage); 
};

const MapStrStr_t&	WebServer::getHttpStatus(void) const 
{
	return (this->_httpStatus); 
};

const MapStrStr_t&	WebServer::getContentType(void) const 
{
	return (this->_contentType); 
};

size_t	WebServer::getBodySizeLimit(void) const 
{
	 return (this->_bodySizeLimit); 
};

void	WebServer::addVirtualHost(const VirtualHost& vHost)
{
	_virtualHost.push_back(vHost);
}

void	WebServer::_closeAllFd(bool log)
{
	for (MapFdSockServ_t::iterator it = _SocketServersList.begin(); it != _SocketServersList.end(); ++it)
	{
		wrap_close(it->first);
		if (log)
			logINFO << "closed: " << it->second;
	}
	for (MapFdClient_t::iterator it = _ClientList.begin(); it != _ClientList.end(); ++it)
	{
		wrap_close(it->second.getFd_cgi()[0]);
		wrap_close(it->second.getFd_cgi()[1]);
		wrap_close(it->first);
		if (log)
			logINFO << "closed: " << it->second;
	}
	wrap_close(_efd);
}

void	WebServer::_initHttpStatus(void)
{
	// Informative Code
	_httpStatus["100"] = "Continue";
	_httpStatus["101"] = "Switching Protocols";
	_httpStatus["102"] = "Processing";

	// Ok Code
	_httpStatus["200"] = "OK";
	_httpStatus["201"] = "Created";
	_httpStatus["202"] = "Accepted";
	_httpStatus["203"] = "Non-authoritative Information";
	_httpStatus["204"] = "No Content";
	_httpStatus["205"] = "Reset Content";
	_httpStatus["206"] = "Partial Content";
	_httpStatus["226"] = "IM Used";

	// Redirection Code
	_httpStatus["300"] = "Multiple Choices";
	_httpStatus["301"] = "Moved Permanently";
	_httpStatus["302"] = "Found";
	_httpStatus["303"] = "See Other";
	_httpStatus["304"] = "Not Modified";
	_httpStatus["305"] = "Use Proxy";
	_httpStatus["307"] = "Temporary Redirect";
	_httpStatus["308"] = "Permanent Redirect";
	_httpStatus["310"] = "Too many Redirects";

	// Client-Side Error Code
	_httpStatus["400"] = "Bad Request";
	_httpStatus["401"] = "Unauthorized";
	_httpStatus["402"] = "Payment Required";
	_httpStatus["403"] = "Forbidden";
	_httpStatus["404"] = "Not Found";
	_httpStatus["405"] = "Method Not Allowed";
	_httpStatus["406"] = "Not Acceptable";
	_httpStatus["407"] = "Proxy Authentication Required";
	_httpStatus["408"] = "Request Timeout";
	_httpStatus["409"] = "Conflict";
	_httpStatus["410"] = "Gone";
	_httpStatus["411"] = "Length Required";
	_httpStatus["412"] = "Precondition Failed";
	_httpStatus["413"] = "Payload Too Large";
	_httpStatus["414"] = "Request-URI Too Long";
	_httpStatus["415"] = "Unsupported Media Type";
	_httpStatus["416"] = "Requested Range Not Satisfiable";
	_httpStatus["417"] = "Expectation Failed";
	_httpStatus["418"] = "I’m a teapot";
	_httpStatus["419"] = "Page expired";
	_httpStatus["421"] = "Misdirected Request";
	_httpStatus["424"] = "Failed Dependency";
	_httpStatus["426"] = "Upgrade Required";
	_httpStatus["428"] = "Precondition Required";
	_httpStatus["429"] = "Too Many Requests";
	_httpStatus["431"] = "Request Header Fields Too Large";
	_httpStatus["451"] = "Unavailable For Legal Reasons";

	// Server-Side Error Code
	_httpStatus["500"] = "Internal Server Error";
	_httpStatus["501"] = "Not Implemented";
	_httpStatus["502"] = "Bad Gateway";
	_httpStatus["503"] = "Service Unavailable";
	_httpStatus["504"] = "Gateway Timeout";
	_httpStatus["505"] = "HTTP Version Not Supported";
	_httpStatus["506"] = "Variant Also Negotiates";
	_httpStatus["507"] = "Insufficient Storage";
	_httpStatus["508"] = "Loop Detected";
	_httpStatus["509"] = "Bandwidth Limit Exceeded";
	_httpStatus["510"] = "Not Extended";
	_httpStatus["511"] = "Network Authentication Required";	
};

void		WebServer::_initContentTypeMap( void )
{
	logDEBUG << "_initContentTypeMap started";

	std::ifstream	file("./data/MIME");
	std::string		line;

	while (std::getline(file, line, '\n'))
	{
		std::size_t	found = line.find(' ');
		std::string	extention;
		std::string	type;

		if (found != 0)
		{
			extention = line.substr(0, found);
			type = line.substr(found + 1, line.length() - found - 1);
			_contentType[extention] = type;
		}
	}
	logDEBUG << "_initContentTypeMap finished";
};
