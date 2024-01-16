/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualHost.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 14:03:40 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/16 20:31:59 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VirtualHost.hpp"

VirtualHost::VirtualHost(void) : sId(0), bodySize(1024), root("/data"), index("index.html"), dirCgi("/data/cgi-bin/"), host_port("0.0.0.0", 80) {}

VirtualHost::~VirtualHost(void) {}

VirtualHost::VirtualHost(const VirtualHost& src) 
{
	*this = src;
};

VirtualHost&	VirtualHost::operator=(const VirtualHost& src) 
{
	if (this == &src)
		return (*this);
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

int	VirtualHost::getSId() const{
	return(this->sId);
};

size_t VirtualHost::getBodySize() const{
	return(this->bodySize);
};

const std::string&	VirtualHost::getRoot() const{
	return(this->root);
};

const std::string&	VirtualHost::getIndex() const{
	return(this->index);
};

const std::string&	VirtualHost::getDirCgi() const{
	return(this->dirCgi);
};

const std::pair<std::string, uint16_t>&	VirtualHost::getHostPort() const{
	return(this->host_port);
};

const std::vector<std::string>&	VirtualHost::getServerNames() const{
	return(this->serverNames);
};

const std::map<std::string, std::string>&	VirtualHost::getCgi() const{
	return(this->cgi);
};

const std::map<std::string, Location>&	VirtualHost::getLocations() const{
	return(this->locations);
};


void	VirtualHost::setBodySize(std::vector<std::string>& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Server: body_size_limit supplied but value is missing");
	this->bodySize = std::strtoul(sLine.at(1).c_str(), NULL, 10);	
};

void	VirtualHost::setRoot(std::vector<std::string>& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Server: root supplied but value is missing");
	
	if (sLine.at(1).at(0) != '/' && sLine.at(1).at(0) != '.')
		std::runtime_error("Server: root is not a valid path");
	this->root = sLine.at(1);
};

void	VirtualHost::setIndex(std::vector<std::string>& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Server: index supplied but value is missing");
	this->index = sLine.at(1);
};


void	VirtualHost::setDirCgi(std::vector<std::string>& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Server: dir_cgi supplied but value is missing");
	this->dirCgi = sLine.at(1);
};


bool verifieSyntaxe(const std::string& s) 
{
    std::istringstream ss(s);
    int num;

    for (int i = 0; i < 4; ++i)
	{
        if (!(ss >> num)) 
            return false;
        if (i < 3 && ss.get() != '.')
            return false;
        if (num < 0 || num > 255) 
            return false;
    }
    return (ss.eof());
}

uint16_t isIntValid(const std::string& s) 
{
    for (size_t i = 0; i < s.size(); ++i)
        if (s[i] < '0' || s[i] > '9') 
            throw std::runtime_error("Error : invalid port value");

    std::istringstream ss(s);
    int num;
    ss >> num;

    if (!ss.fail() && ss.eof() && num >= std::numeric_limits<uint16_t>::min() && num <= std::numeric_limits<uint16_t>::max())
        return (num);
	else
        throw std::runtime_error("Error : invalid port");
}


void	VirtualHost::setHostPort(std::vector<std::string>& sLine)
{
	size_t	tmp;
	std::string 	sPort;

	if (sLine.size() < 2)
		throw std::runtime_error("Server: host supplied but value is missing");
	tmp = sLine[1].find(':');
	if (tmp == std::string::npos)
	{
		if (verifieSyntaxe(sLine[1]) == true)
			this->host_port.first = sLine[1];
		else
			this->host_port.second = isIntValid(sLine[1]);
	}
	else
	{
		this->host_port.first = sLine[1].substr(0, tmp);
		sPort = sLine[1].substr(tmp + 1, sLine[1].size() - tmp - 1);
		if (sPort != "*")
			this->host_port.second  = isIntValid(sPort);
	}	
};

void	VirtualHost::setServerNames(std::vector<std::string>& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Server: sever_name supplied but value is missing");
	for (size_t j = 1; j < sLine.size(); ++j)
			this->serverNames.push_back(sLine[j]);
};

void	VirtualHost::setCgi(std::vector<std::string>& sLine, bool oneCgi)
{
	if (oneCgi)
	{
		if (sLine.size() < 3)
			throw std::runtime_error("Server: path_cgi supplied but value is missing");
		if (sLine[2].at(0) == '/')
			sLine[2].erase(0,1);
		cgi[sLine[1]] = this->dirCgi + sLine[2];
		// if (access(cgi[sLine[1]].c_str(), F_OK))
		// 	throw std::runtime_error("Server: cgi " + sLine[1] + " doesn't exist");
			
	}
	else
	{
		if (sLine.size() < 2)
			throw std::runtime_error("Server: cgi_available supplied but value is missing");
		for (size_t j = 1; j < sLine.size(); ++j)
		{
			if (cgi.count(sLine[j]) == 0)
				cgi[sLine[j]] = this->dirCgi + sLine[j];
		}
	}
};

void	VirtualHost::setLocations(Location& newLoc)
{
	this->locations.insert(std::pair<std::string, Location>(newLoc.uri_or_ext, newLoc));
};
