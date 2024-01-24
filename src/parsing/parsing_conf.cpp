/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_conf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 21:13:46 by louisa            #+#    #+#             */
/*   Updated: 2024/01/24 16:10:45 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include "Request.hpp"

int WebServer::parseConf(std::string &line)
{
	VecStr_t	splitedLine;
	size_t		tmp;
	
	formatLine(line);
	splitedLine = splitLine(line);
	if (splitedLine.empty() || splitedLine[0] == "\n")
		return (0);
	else if (splitedLine[0] == "body_size_limit")
	{
		std::stringstream stream(splitedLine[1]);
		stream >> tmp;
		setBodySizeLimit(tmp);
	}
	else if (splitedLine[0] == "dir_error_page")
		setDirErrorPage(splitedLine[1]);
	else if (splitedLine[0] == "error_page")
		setErrorPage(splitedLine[1], splitedLine[2]);
	else if (line.find("server") != std::string::npos)
		return (1);
	else
		throw std::runtime_error("Unrecognised line in configuration file");
	return (0);
}

Location WebServer::parseLocation(VecStr_t fileVec, VecStr_t sLine, uintptr_t *i)
{
	Location	newLoc;
	
	if (sLine.size() < 2 || sLine[1].find_first_of("{}") != std::string::npos)
		throw std::runtime_error("Location: no key supplied");
	newLoc.setUriOrExt(sLine[1]);
	++(*i);
	while ((fileVec[*i].find("}") == std::string::npos))
	{
		formatLine(fileVec[*i]);
		sLine = splitLine(fileVec[*i]);
		if (sLine.empty() || sLine[0].empty() || sLine[0] == "{")
		{
			++(*i);
			continue;
		}
		else if (sLine[0] == "root")
			newLoc.setRoot(sLine);
		else if (sLine[0] == "return")
			newLoc.setRedirection(sLine);
		else if (sLine[0] == "index")
			newLoc.setIndex(sLine);
		else if (sLine[0] == "allow_methods")
			newLoc.setAllowMethod(sLine);
		else if (sLine[0] == "autoindex")
			newLoc.setAutoIndex(sLine);
		else if (sLine[0] == "upload_dir")
			newLoc.setUploadDir(sLine);
		else
			throw std::runtime_error("Location: Unrecognised line in configuration file : " + fileVec[*i]);
		++(*i);
	}
	newLoc.isLegit();
	return (newLoc);
}

void WebServer::parseServ(VecStr_t fileVec, uintptr_t start, uintptr_t end)
{
	VirtualHost	newServ;
	VecStr_t	sLine;

	for (uintptr_t i = start; i <= end; ++i) 
	{
		formatLine(fileVec[i]);
		sLine = splitLine(fileVec[i]);
		if (sLine.empty() || sLine[0] == "}" || sLine[0] == "{" || sLine[0] == "\n")
			continue ;
		else if (sLine[0] == "location")
		{
			Location newLoc;
			newLoc = parseLocation(fileVec, sLine, &i);
			newServ.setLocations(newLoc);
		}
		else if (sLine[0] == "listen")
			newServ.setHostPort(sLine);
		else if (sLine[0] == "server_name")
			newServ.setServerNames(sLine);
		else if (sLine[0] == "root")
			newServ.setRoot(sLine);
		else if (sLine[0] == "index")
			newServ.setIndex(sLine);
		else if (sLine[0] == "dir_cgi")
			newServ.setDirCgi(sLine);
		else if (sLine[0] == "body_size_limit")
			newServ.setBodySize(sLine);
		else if (sLine[0] == "path_cgi")
			newServ.setCgi(sLine, true);
		else if (sLine[0] == "cgi_available")
			newServ.setCgi(sLine, false);
		else
			throw std::runtime_error("Server: Unrecognised line in configuration file : " + fileVec[i]);
	}
	addVirtualHost(newServ);
}

void WebServer::findServ(VecStr_t fileVec, uintptr_t *i)
{
	uintptr_t	start = 0;
	uintptr_t	end = 0;
	int 		bracket = 0;
	char		c;

	for (uintptr_t j = *i; j < fileVec.size(); ++j)
	{
		const std::string &line = fileVec[j];
		for (size_t k = 0; k < line.size(); ++k)
		{
			c = line[k];
			if (c == '{')
			{
				if (start == 0)
					start = j + 1;
				++bracket;
			}
			else if (c == '}')
			{
				--bracket;
				if (bracket == 0)
				{
					*i = j + 1;
					end = j;
					parseServ(fileVec, start, end);
					return;
				}
			}
		}
	}
	throw std::runtime_error("Error : wrong synthax in configuration file");
}


void WebServer::displayLocations(const VirtualHost& vHost)
{
	typedef MapStrLoc_t::const_iterator	LocationIterator;

	for (LocationIterator it = vHost.getLocations().begin(); it != vHost.getLocations().end(); ++it) {
		const Location& location = it->second;
		std::clog << std::endl;
		std::clog << "Location ID: " << it->first << std::endl;
		std::clog << "Is path: " << (location.getIsPath() ? "true" : "false") << std::endl;
		std::clog << "Auto index: " << (location.getAutoIndex() ? "on" : "off") << std::endl;
		std::clog << "Location URI or Extension: " << location.getUriOrExt() << std::endl;
		std::clog << "Location root: " << (location.getRoot().empty() ? "empty" : location.getRoot()) << std::endl;
		std::clog << "Location return: " << (location.getRedirection().first.empty() ? "empty" : (location.getRedirection().first + " " + location.getRedirection().second)) << std::endl;
		std::clog << "Location index: " << location.getIndex() << std::endl;
		std::clog << "Location UploadDir: " << location.getUpload()  << std::endl;
		std::clog << "Location UploadDir: " << location.getUploadDir()<< std::endl;
		std::clog << "Location UploadDir: " << (location.getUpload() ? location.getUploadDir() : "non authorized") << std::endl;
		std::clog << "Location methods: ";
		for (size_t j = 0; j < location.getAllowMethod().size(); ++j)
			std::clog << location.getAllowMethod()[j] << " ";
		std::clog << std::endl;
	}
}

void WebServer::displayCGI(const VirtualHost& vHost)
{
	typedef MapStrStr_t::const_iterator	CgiIterator;

	std::clog << "CGI directory: " << (vHost.getDirCgi().empty() ? "empty" : vHost.getDirCgi()) << std::endl;
	for (CgiIterator it = vHost.getCgi().begin(); it != vHost.getCgi().end(); ++it) {
		std::clog << std::endl;
		std::clog << "CGI exec: " << it->first << std::endl;
		std::clog << "CGI path: " << it->second << std::endl;
	}
}

void	WebServer::debugServ()
{
	std::clog << "*------------- DEBUG --------------*" << std::endl;
	std::clog << "size body max = " << getBodySizeLimit() << std::endl;
	std::clog << "error page directory = " << getDirErrorPage() << std::endl;
	std::clog << "error page value = ";
	if (_errorPage.empty() == false)
		std::clog << getErrorPage();
	else
		std::clog << "none";
	std::clog << std::endl;
	std::clog << "*------------- SERV --------------*" << std::endl;
	for (size_t i = 0; i < _virtualHost.size(); ++i)
	{
		std::clog << "Server host = " << _virtualHost[i].getHostPort().first << std::endl;
		std::clog << "Server port = " << _virtualHost[i].getHostPort().second << std::endl;
		for (size_t l = 0; l < _virtualHost[i].getServerNames().size(); ++l)
			std::clog << "Server names = " << _virtualHost[i].getServerNames()[l] << std::endl;
		std::clog << "Server root = " << _virtualHost[i].getRoot() << std::endl;
		std::clog << "Server index = " << _virtualHost[i].getIndex() << std::endl;
		std::clog << "Server bodySize = " << _virtualHost[i].getBodySize() << std::endl;
		displayCGI(_virtualHost[i]);
		std::clog << std::endl;
		std::clog << "*------------- LOCATIONS --------------*" << std::endl;
		displayLocations(_virtualHost[i]);
	}
}
