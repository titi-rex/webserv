/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 11:25:02 by lboudjem          #+#    #+#             */
/*   Updated: 2024/02/01 14:31:29 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <sys/types.h>
#include <sys/wait.h> 

void WebServer::fillElement(std::string key, std::string val) 
{
    MapStrStr_t::iterator it = _envCGI.find(key);

    if (it != _envCGI.end())
        it->second = val;
    else
        _envCGI[key] = val;
}

void WebServer::fillValueFromHeader(MapStrStr_t header, std::string key) 
{
    MapStrStr_t::iterator it = header.find(key);

    if (it != header.end())
    {
        std::string upperKey = key;
        std::transform(upperKey.begin(), upperKey.end(), upperKey.begin(), ::toupper);

        _envCGI[upperKey] = it->second;
    }
}

void WebServer::fillValueFromCGI(MapStrStr_t cgi, std::string key, std::string value) 
{
    MapStrStr_t::iterator it = cgi.find(value);

    if (it != cgi.end())
	{
        _envCGI[key] = it->second;
		// std::cout << "SCRIPT_NAME = " << it->second << std::endl;
	}
}

std::string uint16tostr(uint16_t value) {
    std::ostringstream oss;
    oss << value;
    std::string strValue = oss.str();

    return strValue;
}

std::string uint32tostr(uint32_t value) {
    std::ostringstream oss;
    oss << value;
    std::string strValue = oss.str();

    return strValue;
}

void    WebServer::fillEnvCGI(const Client& client) 
{
    // serveur
    fillElement("SERVER_SOFTWARE", "Webserver/1.01");
    fillElement("SERVER_NAME", client.host->getServerNames()[0]);
    fillElement("GATEWAY_INTERFACE", "CGI/1.1");

    // requete
    fillElement("SERVER_PROTOCOL", "HTTP/1.1");
    fillElement("SERVER_PORT", uint16tostr(client.host->getHostPort().second));
    if (client.getMid() == 0)
        fillElement("REQUEST_METHOD", "GET");
    else
        fillElement("REQUEST_METHOD", "POST");
    fillElement("PATH_INFO", client.getPathTranslated());
    fillElement("SCRIPT_FILENAME", client.getPathTranslated());
    fillElement("PATH_TRANSLATED", client.getPathTranslated());
    fillElement("QUERY_STRING", client.getQuery());
    fillElement("REMOTE_HOST", "");
    fillElement("REDIRECT_STATUS", "200");
    fillElement("REMOTE_ADDR", uint32tostr(client.getSin().sin_addr.s_addr));
    fillElement("AUTH_TYPE", "null"); 
    fillValueFromCGI(client.host->getCgi(), "SCRIPT_NAME", client.host->getCgi().at(client.getExt()));
    fillValueFromHeader(client.getHeaders(), "content-type");
    fillValueFromHeader(client.getHeaders(), "content-length");

    // client
    // HTTP_ACCEPT
    // HTTP_ACCEPT_LANGUAGE
    // HTTP_USER_AGENT
    // HTTP_COOKIE
    // HTTP_REFERER
}

void convertToEnvp(const MapStrStr_t& map, char**& envp)
{
    int i = 0;
    int envpSize = map.size() + 1; 
    
    envp = new char*[envpSize];
    // std::cout << std::endl;
    // std::cout << "*------- CGI ENVIROMMENT -------*" << std::endl;
    // std::cout << std::endl;
    for (MapStrStr_t::const_iterator it = map.begin(); it != map.end(); ++it)
    {
        std::string keyValue = it->first + "=" + it->second;
        const char* keyValueChar = keyValue.c_str();
        
        size_t len = 0;
        while (keyValueChar[len] != '\0')
            ++len;
        envp[i] = new char[len + 1];
        // std::cout << "env[i] = " << envp[i] << std::endl;
        for (size_t j = 0; j <= len; ++j)
            envp[i][j] = keyValueChar[j];
        ++i;
    }

    envp[i] = NULL;
}

void WebServer::execute_cgi(const std::string& script_path, Client& client) 
{
	char**	envp;

	std::clog <<  "EXEC CGI with path :"<< script_path << ":" << std::endl;

	convertToEnvp(_envCGI, envp);
	pipe(client.getFd_cgi());
	client.setPstatus(CGIHD);
	modEpollList(client.getFd_cgi()[0], EPOLL_CTL_ADD, EPOLLIN);
	_fdCgi[client.getFd_cgi()[0]] = &client;	//add fd cgi a un map reliant le fd au client 

	pid_t pid = fork();
	if (pid == 0) 
	{
		if (chdir(client.host->getDirCgi().c_str()) == 0)
		{
			dup2(client.getFd_cgi()[1], STDOUT_FILENO);
			_closeAllFd(false);

			// on doit executer la cgi avec la target en parametre askip?? (cf sujet)
			char* const argc[] = {const_cast<char*>(script_path.c_str()), const_cast<char*>(client.getPathTranslated().c_str()), NULL};
			std::cerr << "pathcript:" << argc[0] << std::endl;
			std::cerr << "arg1:" << argc[1] << std::endl;
			execve(argc[0], argc, envp);
		}
		perror("error : execve");
		exit(EXIT_FAILURE);
	} 
	else if (pid < 0)
		perror("Error : fork");

	close(client.getFd_cgi()[1]);
	for (int i = 0; envp[i] != NULL; ++i)
		delete[] envp[i];
	delete[] envp;
}
