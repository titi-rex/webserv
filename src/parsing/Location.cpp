/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 13:41:14 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/24 15:15:50 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(void) : isPath(true), autoIndex(false), upload(false), index("index.html")
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
	if (_key.empty() || (root.empty() && redirection.first.empty()))
		throw std::runtime_error("Location: key / root or return are empty");
}


Location&	Location::operator=(const Location& src) 
{
	if (this == &src)
		return (*this);
	this->isPath = src.isPath;
	this->autoIndex = src.autoIndex;
	this->upload = src.upload;
	this->_key = src._key;
	this->root = src.root;
	this->index = src.index;
	this->uploadDir = src.uploadDir;
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

bool Location::getUpload() const{
	return(this->upload);
};

const std::string&	Location::getUriOrExt() const{
	return(this->_key);
};

const std::string&	Location::getRoot() const{
	return(this->root);
};

const std::string& Location::getIndex() const{
	return(this->index);
};

const std::string& Location::getUploadDir() const{
	return(this->uploadDir);
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

void	Location::setAutoIndex(const VecStr_t& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Location: autoindex supplied but value is missing");
	if (sLine[1] == "on")
		this->autoIndex = true;
	else if (sLine[1] == "off")
		this->autoIndex = false;
	else
		throw std::runtime_error("Location: Invalid Autoindex");
};

void	Location::setUriOrExt(const std::string& key)
{
	if (key.at(0) != '/' && key.at(0) != '.')
		std::runtime_error("Location: key is neither a path or a file extension");
	this->_key = key;
};

void	Location::setRoot(const VecStr_t& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Location: root supplied but value is missing");
	if (sLine[1].at(0) != '/' && sLine[1].at(0) != '.')
		std::runtime_error("Location: root is not a path");
	this->root = sLine[1];
};

void	Location::setIndex(const VecStr_t& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Location: index supplied but value is missing");
	this->index = sLine[1];
};

void	Location::setUploadDir(const VecStr_t& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Location: upload_dir supplied but value is missing");
	if (access(sLine[1].c_str(), F_OK | R_OK))
		throw std::runtime_error("Location: can't access upload_dir");
	this->uploadDir = sLine[1];
	upload = true;
};

void	Location::setAllowMethod(const VecStr_t& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Location: allow_method supplied but value is missing");
	for (size_t j = 1; j < sLine.size(); ++j)
	{
		if (std::find(this->allowMethod.begin(), this->allowMethod.end(), sLine[j]) == this->allowMethod.end())
			this->allowMethod.push_back(sLine[j]);
	}
};

void	Location::setRedirection(const VecStr_t& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Location: return supplied but value is missing");
	this->redirection.first = sLine.at(1);
	try 
	{
		this->redirection.second = sLine.at(2);	
	}
	catch (std::exception & e)
	{
		logWARNING << "Location: return with no new location"; 
	}
};
