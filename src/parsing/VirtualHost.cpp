/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualHost.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 14:03:40 by lboudjem          #+#    #+#             */
/*   Updated: 2024/02/13 13:48:49 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VirtualHost.hpp"

VirtualHost::VirtualHost(void) : root(_dirPrefix + "/data"), index("index.html"), dirCgi(_dirPrefix + "/data/cgi-bin/"), host_port("0.0.0.0", 80) 
{
	serverNames.push_back("default");
}

VirtualHost::VirtualHost(const VirtualHost& src) : _dirPrefix(src._dirPrefix)
{
	*this = src;
};

VirtualHost&	VirtualHost::operator=(const VirtualHost& src) 
{
	if (this == &src)
		return (*this);
	this->root = src.root;
	this->index = src.index;
	this->dirCgi = src.dirCgi;
	this->host_port = src.host_port;
	this->serverNames = src.serverNames;
	this->cgi = src.cgi;
	this->locations = src.locations;
	return (*this);
};

VirtualHost::~VirtualHost(void) {}

VirtualHost::VirtualHost(const std::string& dirPrefix) : _dirPrefix(dirPrefix), root(_dirPrefix + "/data"), index("index.html"), dirCgi(_dirPrefix + "/data/cgi-bin/"), host_port("0.0.0.0", 80) {
	serverNames.push_back("default");
}

const std::string&	VirtualHost::getRoot() const
{
	return(this->root);
};

const std::string&	VirtualHost::getIndex() const
{
	return(this->index);
};

const std::string&	VirtualHost::getDirCgi() const
{
	return(this->dirCgi);
};

const PairStrUint16_t&	VirtualHost::getHostPort() const
{
	return(this->host_port);
};

const VecStr_t&	VirtualHost::getServerNames() const
{
	return(this->serverNames);
};

const MapStrStr_t&	VirtualHost::getCgi() const
{
	return(this->cgi);
};

const MapStrLoc_t&	VirtualHost::getLocations() const
{
	return(this->locations);
};

void	VirtualHost::setRoot(VecStr_t& sLine)
{
	setDir(this->root, sLine, "VirtualHost", _dirPrefix);
};

void	VirtualHost::setIndex(VecStr_t& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("VirtualHost: index supplied but value is missing");
	this->index = sLine.at(1);
	if (index.at(index.size() - 1) == '/' or index.at(0) == '.' )
		throw std::runtime_error("VirtualHost: index can't be a directory: " + index);
};


void	VirtualHost::setDirCgi(VecStr_t& sLine)
{
	setDir(this->dirCgi, sLine, "VirtualHost", _dirPrefix);
};

bool verifieSyntaxe(const std::string& s) 
{
	std::istringstream	ss(s);
	int					num;

	for (int i = 0; i < 4; ++i)
	{
		if (!(ss >> num)) 
			return (false);
		if (i < 3 && ss.get() != '.')
			return (false);
		if (num < 0 || num > 255) 
			return (false);
	}
	return (ss.eof());
}

void	legitHost(const std::string& s)
{
	size_t	i = 0;
	size_t	count = 0;

	if (s.find_first_not_of(".0123456789") != std::string::npos)
		throw std::runtime_error("VirtualHost: invalid host: " + s);
	while (i < s.size())
	{
		size_t	j = 0;
		while ((i + j + 1) < s.size() and std::isdigit(s.at(i + j)) and j < 3)
			++j;
		i += j;
		if (count < 3 && s.at(i) == '.')
			++i;
		else if (s.at(i) == '\0')
			return ;
		else
			throw std::runtime_error("VirtualHost: invalid host: " + s);
		++count;
		++i;
	}
}

uint16_t isIntValid(const std::string& s) 
{
	if (s.find_first_not_of("0123456789") != std::string::npos)
			throw std::runtime_error("Error : invalid port: incorrect value: " + s);

	std::istringstream	ss(s);
	int					num;
	ss >> num;

	if (!ss.fail() && ss.eof() && num >= std::numeric_limits<uint16_t>::min() && num <= std::numeric_limits<uint16_t>::max())
		return (num);
	else
		throw std::runtime_error("Error : invalid port: out of limits: " + s);
}

void	VirtualHost::setHostPort(VecStr_t& sLine)
{
	size_t		tmp;
	std::string	sPort;

	if (sLine.size() < 2)
		throw std::runtime_error("VirtualHost: host supplied but value is missing");
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
	legitHost(this->host_port.first);
};

void	VirtualHost::setServerNames(VecStr_t& sLine)
{
	if (sLine.size() < 2)
		throw std::runtime_error("Server: sever_name supplied but value is missing");
	for (size_t j = 1; j < sLine.size(); ++j)
			this->serverNames.push_back(sLine[j]);
};

void	VirtualHost::setCgi(VecStr_t& sLine, bool oneCgi)
{
	if (oneCgi)
	{
		if (sLine.size() < 3)
			throw std::runtime_error("Server: path_cgi supplied but value is missing");
		if (sLine[2].at(0) == '/')
			sLine[2].erase(0,1);
		cgi[sLine[1]] = sLine[2];
		std::string	tmp = this->dirCgi + cgi[sLine[1]];
		if (access(tmp.c_str(), F_OK | X_OK))
			throw std::runtime_error("Server: cgi \'" + cgi[sLine[1]] + "\' not accessible");
	}
	else
	{
		if (sLine.size() < 2)
			throw std::runtime_error("Server: cgi_available supplied but value is missing");
		for (size_t j = 1; j < sLine.size(); ++j)
		{
			if (cgi.count(sLine[j]) == 0)
				cgi[sLine[j]] = sLine[j];
			else
				continue;
			std::string	tmp = this->dirCgi + cgi[sLine[j]];
			if (access(tmp.c_str(), F_OK | X_OK))
				throw std::runtime_error("Server: cgi \'" + cgi[sLine[j]] + "\' not accessible");
		}
	}
};

void	VirtualHost::setLocations(Location& newLoc)
{
	this->locations.insert(std::pair<std::string, Location>(newLoc.getUriOrExt(), newLoc));
};

void	VirtualHost::secureUpload(void)
{
	for (MapStrLoc_t::const_iterator it = locations.begin(); it != locations.end(); ++it)
	{
		if (it->second.getUploadDir() == this->dirCgi)
			throw std::runtime_error("Virtual Host: security breach, upload_dir same as dir-cgi for location: " + it->first);
	}
}

/**
 * @brief format id/names/host
 */
std::ostream&	operator<<(std::ostream &os, const vHostPtr v_host)
{
	
	os << v_host->getServerNames() << "/" << v_host->getHostPort().first << ":" << v_host->getHostPort().second;
	return (os);
}

/**
 * @brief format id/names/host
 */
std::ostream&	operator<<(std::ostream &os, const VirtualHost& v_host)
{
	os << v_host.getServerNames() << "/" << v_host.getHostPort().first << ":" << v_host.getHostPort().second;
	return (os);
}
