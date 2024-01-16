/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 11:25:02 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/16 15:37:57 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <sys/types.h>
#include <sys/wait.h> 

void WebServer::fillElement(std::string key, std::string val) 
{
    std::map<std::string, std::string>::iterator it = _envCGI.find(key);

    if (it != _envCGI.end())
        it->second = val;
    else
        _envCGI[key] = val;
}

void WebServer::fillValueFromHeader(std::map<std::string, std::string> header, std::string key) 
{
    std::map<std::string, std::string>::iterator it = header.find(key);

    if (it != header.end())
    {
        std::string upperKey = key;
        std::transform(upperKey.begin(), upperKey.end(), upperKey.begin(), ::toupper);

        _envCGI[upperKey] = it->second;
    }
}

void WebServer::fillValueFromCGI(std::map<std::string, std::string> cgi, std::string key, std::string value) 
{
    std::map<std::string, std::string>::iterator it = cgi.find(value);

    if (it != cgi.end())
        _envCGI[key] = it->second;
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
    if (client.request.getMid() == 0)
        fillElement("REQUEST_METHOD", "GET");
    else
        fillElement("REQUEST_METHOD", "POST");
    fillElement("PATH_INFO", client.request.getPathInfo());
    fillElement("PATH_TRANSLATED", client.request._pathTranslated);
    fillElement("QUERY_STRING", client.request.getQuery());
    fillElement("REMOTE_HOST", "");
    fillElement("REMOTE_ADDR", uint32tostr(client.getSin().sin_addr.s_addr)); // dans _sin dans socket (client herite de socket)
    fillElement("AUTH_TYPE", "null");
    fillValueFromCGI(client.host->getCgi(), "SCRIPT_NAME", client.request.getExt());
    fillValueFromHeader(client.request.getHeaders(), "content-type");
    fillValueFromHeader(client.request.getHeaders(), "content-length");

    // client
    // HTTP_ACCEPT
    // HTTP_ACCEPT_LANGUAGE
    // HTTP_USER_AGENT
    // HTTP_COOKIE
    // HTTP_REFERER
}

void convertToEnvp(const std::map<std::string, std::string>& map, char**& envp)
{
    int i = 0;
    int envpSize = map.size() + 1; 
    
    envp = new char*[envpSize];

    std::cout << "*------- CGI ENVIROMMENT -------*" << std::endl;
    std::cout << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = map.begin(); it != map.end(); ++it)
    {
        std::string keyValue = it->first + "=" + it->second;
        envp[i] = strdup(keyValue.c_str());
        std::cout << "env[i] = " << envp[i] << std::endl;
        ++i;
    }

    envp[i] = NULL;
}

void WebServer::execute_cgi(const std::string& script_path) 
{
    int         pipe_fd[2];
	char**      envp;

    convertToEnvp(_envCGI, envp);
    pipe(pipe_fd);
    pid_t pid = fork();

    if (pid == 0) 
    {
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        char* const argc[] = {const_cast<char*>(script_path.c_str()), NULL};
        char* const argv[] = {NULL};
        
        execve(argc[0], argv, envp);

        perror("error : execve");
        exit(EXIT_FAILURE);
    } 
    else if (pid > 0) 
    {
        close(pipe_fd[1]);
        
        waitpid(pid, NULL, 0);
            
        char buffer[BUFFER_SIZE];
        ssize_t read_bytes;

        while ((read_bytes = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
            std::cout << "buffer = " << buffer << std::endl;

        close(pipe_fd[0]);
    }
    else
        perror("Error : fork");

    for (int i = 0; envp[i] != NULL; ++i)
        delete[] envp[i];

    delete[] envp;
}


