/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualHost.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 14:03:40 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/16 15:41:34 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "virtual_host.hpp"

t_virtual_host::t_virtual_host(void) 
: sId(0), bodySize(1024), root("/data"), index("index.html"), host_port("0.0.0.0", 80) {}

t_virtual_host::~t_virtual_host(void) {}

t_virtual_host::t_virtual_host(const t_virtual_host& src) 
{
	*this = src;
};

t_virtual_host&	t_virtual_host::operator=(const t_virtual_host& src) 
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

int	t_virtual_host::getSId() const{
	return(this->sId);
};

size_t t_virtual_host::getBodySize() const{
	return(this->bodySize);
};

const std::string&	t_virtual_host::getRoot() const{
	return(this->root);
};

const std::string&	t_virtual_host::getIndex() const{
	return(this->index);
};

const std::pair<std::string, uint16_t>&	t_virtual_host::getHostPort() const{
	return(this->host_port);
};

const std::vector<std::string>&	t_virtual_host::getServerNames() const{
	return(this->serverNames);
};

const std::map<std::string, std::string>&	t_virtual_host::getCgi() const{
	return(this->cgi);
};

const std::map<std::string, t_location>&	t_virtual_host::getLocations() const{
	return(this->locations);
};


void	t_virtual_host::setBodySize(std::vector<std::string>& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Server: body_size_limit supplied but value is missing");
	this->bodySize = std::strtoul(sLine.at(1).c_str(), NULL, 10);	
};

void	t_virtual_host::setRoot(std::vector<std::string>& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Server: root supplied but value is missing");
	
	if (sLine.at(1).at(0) != '/' && sLine.at(1).at(0) != '.')
		std::runtime_error("Server: root is not a valid path");
	this->root = sLine.at(1);
};

void	t_virtual_host::setIndex(std::vector<std::string>& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Server: index supplied but value is missing");
	this->index = sLine.at(1);
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


void	t_virtual_host::setHostPort(std::vector<std::string>& sLine)
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

void	t_virtual_host::setServerNames(std::vector<std::string>& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Server: sever_name supplied but value is missing");
	for (size_t j = 1; j < sLine.size(); ++j)
			this->serverNames.push_back(sLine[j]);
};

void	t_virtual_host::setCgi(std::vector<std::string>& sLine, bool oneCgi)
{
	if (oneCgi)
	{
		if (sLine.size() < 3)
			throw std::runtime_error("Server: path_cgi supplied but value is missing");
		cgi[sLine[1]] = sLine[2];
	}
	else
	{
		if (sLine.size() < 2)
			throw std::runtime_error("Server: cgi_available supplied but value is missing");
		for (size_t j = 1; j < sLine.size(); ++j)
		{
			if (cgi.count(sLine[j]) == 0)
				cgi[sLine[j]] = "/data/cgi-bin/" + sLine[j];
		}
	}
};

void	t_virtual_host::setLocations(t_location& newLoc)
{
	this->locations.insert(std::pair<std::string, t_location>(newLoc.uri_or_ext, newLoc));
};
