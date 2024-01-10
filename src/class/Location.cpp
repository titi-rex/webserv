/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 13:41:14 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/10 12:51:24 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "virtual_host.hpp"

t_location::t_location(){}
t_location::~t_location(){}

t_location::t_location(const t_location& src) 
{
	this->isPath = src.isPath;
	this->autoIndex = src.autoIndex;
	this->lId = src.lId;
	this->uri_or_ext = src.uri_or_ext;
	this->root = src.root;
	this->index = src.index;
	this->allowMethod = src.allowMethod;
	this->redirection = src.redirection;
};

t_location&	t_location::operator=(const t_location& src) 
{
	if (this == &src)
		return (*this);
	this->isPath = src.isPath;
	this->autoIndex = src.autoIndex;
	this->lId = src.lId;
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

std::string	t_location::getUriOrExt() const{
	return(this->uri_or_ext);
};

std::string	t_location::getRoot() const{
	return(this->root);
};

std::string t_location::getIndex() const{
	return(this->index);
};

std::vector<std::string> t_location::getAllowMethod() const{
	return(this->allowMethod);
};

std::string	t_location::getRedirection() const{
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
		throw std::runtime_error("Invalid Autoindex");
};

void	t_location::setLId(int lId){
	this->lId = lId;
};

void	t_location::setUriOrExt(std::string uri_or_ext){
	this->uri_or_ext = uri_or_ext;
};

void	t_location::setRoot(std::string root){
	this->root = root;
};

void	t_location::setIndex(std::string index){
	this->index = index;
};

void	t_location::setAllowMethod(std::vector<std::string>	allowMethod){
	for (size_t j = 1; j < allowMethod.size(); ++j)
		if (std::find(this->allowMethod.begin(), this->allowMethod.end(), allowMethod[j]) == this->allowMethod.end())
			this->allowMethod.push_back(allowMethod[j]);
};

void	t_location::setRedirection(std::string redirection){
	this->redirection = redirection;
};
