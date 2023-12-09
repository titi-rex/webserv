/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_conf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 21:13:46 by louisa            #+#    #+#             */
/*   Updated: 2023/12/09 21:14:55 by tlegrand         ###   ########.fr       */
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
		else if (sLine[0] == "listen"){
			pos = sLine[1].find(':');
			newServ.hosts.push_back(sLine[1].substr(0, pos));
			sPort = sLine[1].substr(pos + 1, sLine[1].size() - pos - 2);

			std::istringstream pStream(sPort);
			while (pStream >> port){
				newServ.ports.push_back(port);
				pStream.ignore(1, ',');
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

void	WebServer::debugServ()
{
	std::cout << "*------------- SERV --------------*" << std::endl;
	for (size_t i = 0; i < _virtualHost.size(); ++i) {
		for (size_t j = 0; j < _virtualHost[i].ports.size(); ++j) 
			std::cout << "server host = " << _virtualHost[i].hosts[j] << std::endl;
		for (size_t k = 0; k < _virtualHost[i].ports.size(); ++k)
			std::cout << "server port = " << _virtualHost[i].ports[k] << std::endl;
		for (size_t l = 0; l < _virtualHost[i].serverNames.size(); ++l)
			std::cout << "server names = " << _virtualHost[i].serverNames[l] << std::endl;
		std::cout << "server root = " << _virtualHost[i].root << std::endl;
		std::cout << "server index = " << _virtualHost[i].index << std::endl;
		std::cout << "server bodySize = " << _virtualHost[i].bodySize << std::endl;
	}
}

/*
int main()
{
	WebServer					serv("exemple.conf");
	std::vector<std::string> 	fileVec;
	uintptr_t					i = 0;
	
	std::ifstream file("exemple.conf");
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
*/

