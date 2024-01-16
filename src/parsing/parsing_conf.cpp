/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_conf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 21:13:46 by louisa            #+#    #+#             */
/*   Updated: 2024/01/16 20:31:59 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include "Request.hpp"

int WebServer::parseConf(std::string &line)
{
	std::vector<std::string>	splitedLine;
	size_t						tmp;
	
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

Location WebServer::parseLocation(std::vector<std::string> fileVec, std::vector<std::string> sLine, uintptr_t *i)
{
	Location newLoc;
	std::string first = sLine[1];
	
	++(*i);
	while ((fileVec[*i].find("}") == std::string::npos))
	{
		formatLine(fileVec[*i]);
		sLine = splitLine(fileVec[*i]);
		if (sLine[0].empty())
		{
			++(*i);
			continue;
		}
		else if (sLine[0] == "root")
			newLoc.setRoot(sLine[1]);
		else if (sLine[0] ==  "return")
			newLoc.setRedirection(sLine);
		else if (sLine[0] ==  "index")
			newLoc.setIndex(sLine[1]);
		else if (sLine[0] ==  "allow_methods")
			newLoc.setAllowMethod(sLine);
		else if (sLine[0] ==  "autoindex")
			newLoc.setAutoIndex(sLine[1]);
		else
			throw std::runtime_error("Location: Unrecognised line in configuration file : " + fileVec[*i]);
		++(*i);
	}
	newLoc.setUriOrExt(first);
	newLoc.isLegit();
	return (newLoc);
}

void WebServer::parseServ(std::vector<std::string> fileVec, uintptr_t start, uintptr_t end)
{
	VirtualHost 				newServ;
	std::vector<std::string>	sLine;

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

void WebServer::findServ(std::vector<std::string> fileVec, uintptr_t *i)
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
            if (c == '{'){
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


void WebServer::displayLocations(const VirtualHost& vHost) {
    typedef std::map<std::string, Location>::const_iterator LocationIterator;

    for (LocationIterator it = vHost.locations.begin(); it != vHost.locations.end(); ++it) {
        const Location& location = it->second;
		std::cout << std::endl;
        std::cout << "Location ID: " << it->first << std::endl;
        std::cout << "Is path: " << (location.isPath ? "true" : "false") << std::endl;
        std::cout << "Auto index: " << (location.autoIndex ? "on" : "off") << std::endl;
        std::cout << "Location URI or Extension: " << location.uri_or_ext << std::endl;
        std::cout << "Location root: " << (location.root.empty() ? "empty" : location.root) << std::endl;
        std::cout << "Location return: " << (location.redirection.first.empty() ? "empty" : (location.redirection.first + " " + location.redirection.second)) << std::endl;
		std::cout << "Location index: " << location.index << std::endl;
		std::cout << "Location methods: ";
		for (size_t j = 0; j < location.allowMethod.size(); ++j)
			std::cout << location.allowMethod[j] << " ";
		std::cout << std::endl;
    }
}

void WebServer::displayCGI(const VirtualHost& vHost) {
    typedef std::map<std::string, std::string>::const_iterator LocationIterator;

	std::cout << "CGI directory: " << vHost.getDirCgi() << std::endl;
    for (LocationIterator it = vHost.cgi.begin(); it != vHost.cgi.end(); ++it) {
		std::cout << std::endl;
        std::cout << "CGI exec: " << it->first << std::endl;
        std::cout << "CGI path: " << it->second << std::endl;
    }
}

void	WebServer::debugServ()
{
	std::cout << "*------------- DEBUG --------------*" << std::endl;
	std::cout << "size body max = " << getBodySizeLimit() << std::endl;
	std::cout << "error page directory = " << getDirErrorPage() << std::endl;
	std::cout << "error page value = ";
	if (_errorPage.empty() == false)
	{
		std::cout << getErrorPage();
	}
	else
		std::cout << "none";
	std::cout << std::endl;

	std::cout << "*------------- SERV --------------*" << std::endl;
	for (size_t i = 0; i < _virtualHost.size(); ++i) {
		std::cout << "Server host = " << _virtualHost[i].host_port.first << std::endl;
		std::cout << "Server port = " << _virtualHost[i].host_port.second << std::endl;
		for (size_t l = 0; l < _virtualHost[i].serverNames.size(); ++l)
			std::cout << "Server names = " << _virtualHost[i].serverNames[l] << std::endl;
		std::cout << "Server root = " << _virtualHost[i].root << std::endl;
		std::cout << "Server index = " << _virtualHost[i].index << std::endl;
		std::cout << "Server bodySize = " << _virtualHost[i].bodySize << std::endl;
		displayCGI(_virtualHost[i]);
		std::cout << std::endl;
		
		std::cout << "*------------- LOCATIONS --------------*" << std::endl;
		displayLocations(_virtualHost[i]);
	}
}
