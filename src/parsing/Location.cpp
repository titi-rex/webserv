/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 13:41:14 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/16 20:31:59 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VirtualHost.hpp"

Location::Location(void) : lId(0), isPath(true), autoIndex(true), index("index.html")
{
	allowMethod.push_back("GET");
}

Location::~Location(void) {}

Location::Location(const Location& src) 
{
	*this = src;
};

void	Location::isLegit() const
{
	if (uri_or_ext.empty() || (root.empty() && redirection.first.empty()))
		throw std::runtime_error("Location: key / root or return are empty");
}


Location&	Location::operator=(const Location& src) 
{
	if (this == &src)
		return (*this);
	this->lId = src.lId;
	this->isPath = src.isPath;
	this->autoIndex = src.autoIndex;
	this->uri_or_ext = src.uri_or_ext;
	this->root = src.root;
	this->index = src.index;
	this->allowMethod = src.allowMethod;
	this->redirection = src.redirection;
	return (*this);
};

bool Location::getIsPath() const{
	return(this->isPath);
};	

bool Location::getAutoIndex() const{
	return(this->autoIndex);
};

int	Location::getLId() const{
	return(this->lId);
};

const std::string&	Location::getUriOrExt() const{
	return(this->uri_or_ext);
};

const std::string&	Location::getRoot() const{
	return(this->root);
};

const std::string& Location::getIndex() const{
	return(this->index);
};

const std::vector<std::string>& Location::getAllowMethod() const{
	return(this->allowMethod);
};

const std::pair<std::string, std::string>&	Location::getRedirection() const{
	return(this->redirection);
};

void	Location::setIsPath(bool path){
	this->isPath = path;
};	

void	Location::setAutoIndex(std::string autoIndex){
	if (autoIndex == "on")
		this->autoIndex = true;
	else if (autoIndex == "off")
		this->autoIndex = false;
	else
		throw std::runtime_error("Location: Invalid Autoindex");
};

void	Location::setLId(int lId){
	this->lId = lId;
};

void	Location::setUriOrExt(std::string uri_or_ext)
{
	if (uri_or_ext.at(0) != '/' && uri_or_ext.at(0) != '.')
		std::runtime_error("Location: uri_or_ext is neither a path or a file extension");
	this->uri_or_ext = uri_or_ext;
};

void	Location::setRoot(std::string root)
{
	if (root.at(0) != '/' && root.at(0) != '.')
		std::runtime_error("Location: root is not a path");
	this->root = root;
};

void	Location::setIndex(std::string index){
	this->index = index;
};

void	Location::setAllowMethod(std::vector<std::string>& allowMethod){
	for (size_t j = 1; j < allowMethod.size(); ++j)
		if (std::find(this->allowMethod.begin(), this->allowMethod.end(), allowMethod[j]) == this->allowMethod.end())
			this->allowMethod.push_back(allowMethod[j]);
};

void	Location::setRedirection(std::vector<std::string>& sLine)
{
	this->redirection.first = sLine.at(1);
	try 
	{
		this->redirection.second = sLine.at(2);	
	}
	catch (std::exception & e)
	{
		std::clog << "Location: return with no new location" << std::endl; 
	}
};
