/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 13:41:14 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/16 22:16:03 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(void) : isPath(true), autoIndex(true), index("index.html")
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

const std::string&	Location::getUriOrExt() const{
	return(this->uri_or_ext);
};

const std::string&	Location::getRoot() const{
	return(this->root);
};

const std::string& Location::getIndex() const{
	return(this->index);
};

const VecStr_t& Location::getAllowMethod() const{
	return(this->allowMethod);
};

const PairStrStr_t&	Location::getRedirection() const{
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

void	Location::setAllowMethod(VecStr_t& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Server: allow_method supplied but value is missing");

	for (size_t j = 1; j < sLine.size(); ++j)
	{
		if (std::find(this->allowMethod.begin(), this->allowMethod.end(), sLine[j]) == this->allowMethod.end())
			this->allowMethod.push_back(sLine[j]);
	}
};

void	Location::setRedirection(VecStr_t& sLine)
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
