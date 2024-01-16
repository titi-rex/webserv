/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 13:41:14 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/16 14:16:38 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "virtual_host.hpp"

t_location::t_location(void) : lId(0), isPath(true), autoIndex(true), index("index.html")
{
	allowMethod.push_back("GET");
}

t_location::~t_location(void) {}

t_location::t_location(const t_location& src) 
{
	*this = src;
};

void	t_location::isLegit() const
{
	if (uri_or_ext.empty() || (root.empty() && redirection.first.empty()))
		throw std::runtime_error("Location: key / root or return are empty");
}


t_location&	t_location::operator=(const t_location& src) 
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

bool t_location::getIsPath() const{
	return(this->isPath);
};	

bool t_location::getAutoIndex() const{
	return(this->autoIndex);
};

int	t_location::getLId() const{
	return(this->lId);
};

const std::string&	t_location::getUriOrExt() const{
	return(this->uri_or_ext);
};

const std::string&	t_location::getRoot() const{
	return(this->root);
};

const std::string& t_location::getIndex() const{
	return(this->index);
};

const std::vector<std::string>& t_location::getAllowMethod() const{
	return(this->allowMethod);
};

const std::pair<std::string, std::string>&	t_location::getRedirection() const{
	return(this->redirection);
};

void	t_location::setIsPath(bool path){
	this->isPath = path;
};	

void	t_location::setAutoIndex(std::string autoIndex){
	if (autoIndex == "on")
		this->autoIndex = true;
	else if (autoIndex == "off")
		this->autoIndex = false;
	else
		throw std::runtime_error("Location: Invalid Autoindex");
};

void	t_location::setLId(int lId){
	this->lId = lId;
};

void	t_location::setUriOrExt(std::string uri_or_ext)
{
	if (uri_or_ext.at(0) != '/' && uri_or_ext.at(0) != '.')
		std::runtime_error("Location: uri_or_ext is neither a path or a file extension");
	this->uri_or_ext = uri_or_ext;
};

void	t_location::setRoot(std::string root)
{
	if (root.at(0) != '/' && root.at(0) != '.')
		std::runtime_error("Location: root is not a path");
	this->root = root;
};

void	t_location::setIndex(std::string index){
	this->index = index;
};

void	t_location::setAllowMethod(std::vector<std::string>& allowMethod){
	for (size_t j = 1; j < allowMethod.size(); ++j)
		if (std::find(this->allowMethod.begin(), this->allowMethod.end(), allowMethod[j]) == this->allowMethod.end())
			this->allowMethod.push_back(allowMethod[j]);
};

void	t_location::setRedirection(std::vector<std::string>& sLine)
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
