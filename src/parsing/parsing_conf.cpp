/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_conf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 21:13:46 by louisa            #+#    #+#             */
/*   Updated: 2023/12/11 14:18:16 by lboudjem         ###   ########.fr       */
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
	if (splitedLine.empty())
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
		std::cout << "error" << std::endl;
		// ligne pas reconnu, throw exception
	return (0);
}

void WebServer::parseServ(std::vector<std::string> fileVec, uintptr_t start, uintptr_t end)
{
	t_virtual_host 				newServ;
	std::vector<std::string>	sLine;
	std::string					sPort;
	size_t						tmp;
	size_t						pos = 0;
	int							port = 0;

	for (uintptr_t i = start; i <= end; ++i) {
		formatLine(fileVec[i]);
		sLine = splitLine(fileVec[i]);
		std::cout << "Server line: " << fileVec[i] << std::endl;
		if (sLine.empty() || sLine[0] == "}" || sLine[0] == "{")
			continue ;
		else if (sLine[0] == "location"){
			t_location newLoc;
			std::string first = sLine[1];
			while ((fileVec[i].find("}") == std::string::npos)){
				formatLine(fileVec[i]);
				sLine = splitLine(fileVec[i]);
				if (sLine[0] == "root")
					newLoc.root = sLine[1];
				else if (sLine[0] ==  "return")
					newLoc.redirection = sLine[1];
				else if (sLine[0] ==  "index")
					for (size_t j = 1; j < sLine.size(); ++j)
						newLoc.index.push_back(sLine[j]);
				else if (sLine[0] ==  "allow_methods")
					for (size_t j = 1; j < sLine.size(); ++j)
						newLoc.allowMethod.push_back(sLine[j]);
				else if (sLine[0] ==  "autoindex"){
					if (sLine[1] == "on")
						newLoc.autoIndex = true;
					else if (sLine[1] == "off")
						newLoc.autoIndex = false;
					else
						std::cout << "???ERROR???" << std::endl;
						// trow exception
				}
				++i;
			}
			newLoc.uri_or_ext = first;
			newServ.locations.insert(std::pair<std::string, t_location>(first, newLoc));
		}
		else if (sLine[0] == "listen"){
			pos = sLine[1].find(':');
			newServ.host_port.first = sLine[1].substr(0, pos);
			sPort = sLine[1].substr(pos + 1, sLine[1].size() - pos - 2);
			if (sPort == "*")
				newServ.host_port.second = 80;
			else {
				std::istringstream pStream(sPort);
				while (pStream >> port){
					newServ.host_port.second = port;
					pStream.ignore(1, ',');
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
			std::stringstream stream(sLine[1]);
			stream >> tmp;
			newServ.bodySize = tmp;
		}
		else
			std::cout << "error" << std::endl;
			// ligne pas reconnu, throw exception
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
	// mauvaise synthax, throw exception !!
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
		for (size_t i = 0; i < location.index.size(); ++i)
			std::cout << "Location index: " << location.index[i] << std::endl;
		for (size_t j = 0; j < location.allowMethod.size(); ++j)
			std::cout << "Location methods: " << location.allowMethod[j] << std::endl;
    }
}

void	WebServer::debugServ()
{
	std::cout << "*------------- SERV --------------*" << std::endl;
	for (size_t i = 0; i < _virtualHost.size(); ++i) {
		std::cout << "Server host = " << _virtualHost[i].host_port.first << std::endl;
		std::cout << "Server port = " << _virtualHost[i].host_port.second << std::endl;
		for (size_t l = 0; l < _virtualHost[i].serverNames.size(); ++l)
			std::cout << "Server names = " << _virtualHost[i].serverNames[l] << std::endl;
		std::cout << "Server root = " << _virtualHost[i].root << std::endl;
		std::cout << "Server index = " << _virtualHost[i].index << std::endl;
		std::cout << "Server bodySize = " << _virtualHost[i].bodySize << std::endl;
		std::cout << std::endl;
		
		std::cout << "*------------- LOCATIONS --------------*" << std::endl;
		displayLocations(_virtualHost[i]);
		// std::cout << "location = " << _virtualHost[i].locations.first << std::endl;
	}
}



int main()
{
	WebServer					serv("z_notes/exemple.conf");
	std::vector<std::string> 	fileVec;
	uintptr_t					i = 0;
	
	std::ifstream file("z_notes/exemple.conf");
    if (!file.is_open()) {
        std::cerr << "Error could not open conf file :(" << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
		fileVec.push_back(line);
    }
	
	while (i < fileVec.size())
   	{
		if (serv.parseConf(fileVec[i]) == 1)
			serv.findServ(fileVec, &i);
		std::cout << "Formatted line: " << fileVec[i] << std::endl;
		std::cout << std::endl;
		++i;
   	}

	std::cout << "*------------- DEBUG --------------*" << std::endl;
	std::cout << "size body max = " << serv.getBodySizeLimit() << std::endl;
	std::cout << "error page dir = " << serv.getDirErrorPage() << std::endl;
	std::cout << "error page val = ";
	std::cout << serv.getErrorPage();
	std::cout <<std::endl;
	serv.debugServ();

    return 0;
}


