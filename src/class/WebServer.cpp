#include "WebServer.hpp"

WebServer::WebServer(void) : _bodySizeLimit(-1) {};

WebServer::WebServer(const WebServer& src) 
{
	this->setVirtualHost(src.getVirtualHost());
	this->setErrorPage(src.getErrorPage());
	this->setDirErrorPage(src.getDirErrorPage());
	this->setBodySizeLimit(src.getBodySizeLimit());
	
};

WebServer&	WebServer::operator=(const WebServer& src) 
{
	if (this == &src)
		return (*this);
	this->setVirtualHost(src.getVirtualHost());
	this->setErrorPage(src.getErrorPage());
	this->setDirErrorPage(src.getDirErrorPage());
	this->setBodySizeLimit(src.getBodySizeLimit());
	return (*this);
};

WebServer::~WebServer(void) {};

std::vector<t_virtual_host>	WebServer::getVirtualHost(void) const { return (this->_virtualHost); };
void	WebServer::setVirtualHost(std::vector<t_virtual_host> virtualHost) { this->_virtualHost = virtualHost;; };

std::map<std::string,std::string>	WebServer::getErrorPage(void) const { return (this->_errorPage); };
void	WebServer::setErrorPage(std::map<std::string,std::string> errorPage) { this->_errorPage = errorPage;; };

std::string	WebServer::getDirErrorPage(void) const { return (this->_dirErrorPage); };
void	WebServer::setDirErrorPage(std::string dirErrorPage) { this->_dirErrorPage = dirErrorPage; };

size_t	WebServer::getBodySizeLimit(void) const { return (this->_bodySizeLimit); };
void	WebServer::setBodySizeLimit(size_t bodySizeLimit) { this->_bodySizeLimit = bodySizeLimit; };


WebServer::WebServer(std::string path) 
{
	
};
	
