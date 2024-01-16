/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 11:25:02 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/16 12:33:54 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <sys/types.h>
#include <sys/wait.h> 

void WebServer::initEnvCGI() 
{
    for (std::map<std::string, std::string>::iterator it = _envCGI.begin(); it != _envCGI.end(); ++it)
        it->second = "";
}

//doit creer la key si elle n'existe pas
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
    // SCRIPT_NAME

    fillValueFromHeader(client.request.getHeaders(), "content-type");
    fillValueFromHeader(client.request.getHeaders(), "content-length");

    std::cout << "*------- DEBUG CGI ENV -------*" << std::endl;
	typedef std::map<std::string, std::string>::const_iterator LocationIterator;
    for (LocationIterator it = _envCGI.begin(); it != _envCGI.end(); ++it) {
		std::cout << std::endl;
        std::cout << "CGI first = " << it->first << std::endl;
        std::cout << "CGI second = : " << it->second << std::endl;
    }

    // client
    // HTTP_ACCEPT
    // HTTP_ACCEPT_LANGUAGE
    // HTTP_USER_AGENT
    // HTTP_COOKIE
    // HTTP_REFERER
}

void WebServer::execute_cgi(const std::string& script_path) 
{
    int pipe_fd[2];
    pipe(pipe_fd);
    
    pid_t pid = fork();

    if (pid == 0) 
    {
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        char* const argc[] = {const_cast<char*>(script_path.c_str()), NULL};
        char* const argv[] = {NULL};
		char* const envp[] = {NULL};
        
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
}