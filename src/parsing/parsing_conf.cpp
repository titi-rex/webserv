/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_conf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 21:13:46 by louisa            #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2023/12/13 16:22:26 by lboudjem         ###   ########.fr       */
=======
/*   Updated: 2023/12/13 13:25:04 by jmoutous         ###   ########lyon.fr   */
>>>>>>> 1d93f79ac1f7465341a82a894595211196205e6a
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
	else if (splitedLine[0] == "bodySizeLimit"){
		std::stringstream stream(splitedLine[1]);
		stream >> tmp;
		setBodySizeLimit(tmp);
	}
	else if (splitedLine[0] == "dirErrorPage")
		setDirErrorPage(splitedLine[1]);
	else if (splitedLine[0] == "error_page")
		setErrorPage(splitedLine[1], splitedLine[2]);
	else if (line.find("server") != std::string::npos)
		return (1);
	else
		throw std::runtime_error("Unrecognised line in configuration file");
	return (0);
}

t_location WebServer::parseLocation(std::vector<std::string> fileVec, std::vector<std::string> sLine, uintptr_t *i)
{
	t_location newLoc;
	std::string first = sLine[1];
	
	initLocation(&newLoc);
	++(*i);
	while ((fileVec[*i].find("}") == std::string::npos)){
		formatLine(fileVec[*i]);
		sLine = splitLine(fileVec[*i]);
		if (sLine[0].empty())
			continue;
		else if (sLine[0] == "root")
			newLoc.setRoot(sLine[1]);
		else if (sLine[0] ==  "return")
			newLoc.setRedirection(sLine[1]);
		else if (sLine[0] ==  "index")
			newLoc.setIndex(sLine[1]);
		else if (sLine[0] ==  "allow_methods")
			newLoc.setAllowMethod(sLine);
		else if (sLine[0] ==  "autoindex")
			newLoc.setAutoIndex(sLine[1]);
		else
			throw std::runtime_error("Unrecognised line in configuration file : Location");
		++(*i);
	}
	newLoc.setUriOrExt(first);
	return newLoc;
}

bool verifieSyntaxe(const std::string& s) 
{
    std::istringstream ss(s);
    int num;

    for (int i = 0; i < 4; ++i) {
        if (!(ss >> num)) 
            return false;
        if (i < 3 && ss.get() != '.')
            return false;
        if (num < 0 || num > 255) 
            return false;
    }

    return ss.eof();
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

void WebServer::parseServ(std::vector<std::string> fileVec, uintptr_t start, uintptr_t end)
{
	t_virtual_host 				newServ;
	std::vector<std::string>	sLine;
	std::string					sPort;
	size_t						tmp = 0;

	for (uintptr_t i = start; i <= end; ++i) {
		formatLine(fileVec[i]);
		sLine = splitLine(fileVec[i]);
		if (sLine.empty() || sLine[0] == "}" || sLine[0] == "{" || sLine[0] == "\n")
			continue ;
		else if (sLine[0] == "location"){
			t_location newLoc;
			std::string first = sLine[1];
			newLoc = parseLocation(fileVec, sLine, &i);
			newServ.locations.insert(std::pair<std::string, t_location>(first, newLoc));
		}
		else if (sLine[0] == "listen"){
			tmp = sLine[1].find(':');
			if (tmp == std::string::npos)
			{
				if (verifieSyntaxe(sLine[1]) == true)
				{
					newServ.host_port.first = sLine[1];
					newServ.host_port.second = 80;
				}
				else
				{
					newServ.host_port.first = "127.0.0.1";
					newServ.host_port.second = isIntValid(sLine[1]);
				}
			}
			else
			{
				newServ.host_port.first = sLine[1].substr(0, tmp);
				sPort = sLine[1].substr(tmp + 1, sLine[1].size() - tmp - 1);
				if (sPort == "*")
					newServ.host_port.second = 80;
				else {
					newServ.host_port.second  = isIntValid(sPort);
				}
			}
		}
		else if (sLine[0] == "server_name"){
			for (size_t j = 1; j < sLine.size(); ++j)
				newServ.serverNames.push_back(sLine[j]);
		}
		else if (sLine[0] == "root")
			newServ.root = sLine[1];
		else if (sLine[0] == "index")
			newServ.index = sLine[1];
		else if (sLine[0] == "bodySizeLimit"){
			tmp = 0;
			std::stringstream stream(sLine[1]);
			stream >> tmp;
			newServ.bodySize = tmp;
		}
		else if (sLine[0] == "path_cgi")
			newServ.cgi.insert(std::pair<std::string, std::string>(sLine[1], sLine[2]));
		else if (sLine[0] == "cgi_available"){
			for (size_t j = 1; j < sLine.size(); ++j){
				for (std::map<std::string, std::string>::iterator it = newServ.cgi.begin(); it != newServ.cgi.end(); ++it)
					if (sLine[j] != it->first)
						newServ.cgi.insert(std::pair<std::string, std::string>(sLine[j], "/data/cgi-bin/"));
			}
		}
		else
		{
			std::cout << "error line = " << fileVec[i]<< std::endl;
			throw std::runtime_error("Unrecognised line in configuration file : Server");
		}
	}

	addVirtualHost(newServ);
}

void WebServer::findServ(std::vector<std::string> fileVec, uintptr_t *i)
{
	uintptr_t	start = 0;
	uintptr_t	end = 0;
    int 		bracket = 0;
	char		c;

    for (uintptr_t j = *i; j < fileVec.size(); ++j){
        const std::string &line = fileVec[j];
        for (size_t k = 0; k < line.size(); ++k){
            c = line[k];
            if (c == '{'){
                if (start == 0)
                    start = j + 1;
                ++bracket;
            }
            else if (c == '}'){
                --bracket;
                if (bracket == 0){
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


void WebServer::displayLocations(const t_virtual_host& virtualHost) {
    typedef std::map<std::string, t_location>::const_iterator LocationIterator;

    for (LocationIterator it = virtualHost.locations.begin(); it != virtualHost.locations.end(); ++it) {
        const t_location& location = it->second;
		std::cout << std::endl;
        std::cout << "Location ID: " << it->first << std::endl;
        std::cout << "Is path: " << location.isPath << std::endl;
        std::cout << "Auto index: " << location.autoIndex << std::endl;
        std::cout << "Location URI or Extension: " << location.uri_or_ext << std::endl;
        std::cout << "Location root: " << location.root << std::endl;
        std::cout << "Location return: " << location.redirection << std::endl;
		std::cout << "Location index: " << location.index << std::endl;
		std::cout << "Location methods: ";
		for (size_t j = 0; j < location.allowMethod.size(); ++j)
			std::cout << location.allowMethod[j] << " ";
		std::cout << std::endl;
    }
}

void WebServer::displayCGI(const t_virtual_host& virtualHost) {
    typedef std::map<std::string, std::string>::const_iterator LocationIterator;

    for (LocationIterator it = virtualHost.cgi.begin(); it != virtualHost.cgi.end(); ++it) {
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
	std::cout << getErrorPage();
	std::cout <<std::endl;

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
