/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualHost.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 14:03:40 by lboudjem          #+#    #+#             */
/*   Updated: 2023/12/13 16:19:32 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "virtual_host.hpp"

t_virtual_host::t_virtual_host(){}
t_virtual_host::~t_virtual_host(){}

t_virtual_host::t_virtual_host(const t_virtual_host& src) 
{
	this->defaultServer = src.defaultServer;
	this->sId = src.sId;
	this->bodySize = src.bodySize;
	this->root = src.root;
	this->index = src.index;
	this->host_port = src.host_port;
	this->serverNames = src.serverNames;
	this->cgi = src.cgi;
	this->locations = src.locations;
};

t_virtual_host&	t_virtual_host::operator=(const t_virtual_host& src) 
{
	if (this == &src)
		return (*this);
	this->defaultServer = src.defaultServer;
	this->sId = src.sId;
	this->bodySize = src.bodySize;
	this->root = src.root;
	this->index = src.index;
	this->host_port = src.host_port;
	this->serverNames = src.serverNames;
	this->cgi = src.cgi;
	this->locations = src.locations;
	return (*this);
};

bool t_virtual_host::getDefaultServer() const{
	return(this->defaultServer);
};

int	t_virtual_host::getSId() const{
	return(this->sId);
};

size_t t_virtual_host::getBodySize() const{
	return(this->bodySize);
};

std::string t_virtual_host::getRoot() const{
	return(this->root);
};

std::string t_virtual_host::getIndex() const{
	return(this->index);
};

std::pair<std::string, uint16_t> t_virtual_host::getHostPort() const{
	return(this->host_port);
};

std::vector<std::string> t_virtual_host::getServerNames() const{
	return(this->serverNames);
};

std::map<std::string, std::string> t_virtual_host::getCgi() const{
	return(this->cgi);
};

std::map<std::string, t_location> t_virtual_host::getLocations() const{
	return(this->locations);
};

void	t_virtual_host::setDefaultServer(bool defaultServer){
	this->defaultServer = defaultServer;
};

void	t_virtual_host::setSId(int sId){
	this->sId = sId;
};

void	t_virtual_host::setBodySize(size_t bodySize){
	this->bodySize = bodySize;
};

void	t_virtual_host::setRoot(std::string root){
	this->root = root;
};

void	t_virtual_host::setIndex(std::string index){
	this->index = index;
};

void	t_virtual_host::setHostPort(std::pair<std::string, uint16_t> host_port){
	this->host_port = host_port;
};

void	t_virtual_host::setServerNames(std::vector<std::string> serverNames){
	this->serverNames = serverNames;
};

void	t_virtual_host::setCgi(std::map<std::string, std::string> cgi){
	this->cgi = cgi;
};

void	t_virtual_host::setLocations(std::map<std::string, t_location> locations){
	this->locations = locations;
};
