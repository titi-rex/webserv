/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 11:25:02 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/15 13:53:14 by lboudjem         ###   ########.fr       */
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

void WebServer::fillElement(std::string key, std::string val) {
    std::map<std::string, std::string>::iterator it = _envCGI.find(key);

    if (it != _envCGI.end())
        it->second = val;
}


void    WebServer::fillEnvCGI( std::string port, std::string root) {
	// fillEnvCGI("USER", getenv("USER"));
	// fillEnvCGI("HOME", getenv("HOME"));
	// fillEnvCGI("HTTP_CACHE_CONTROL", HTTP_CACHE_CONTROL);
	// fillEnvCGI("HTTP_UPGRADE_INSECURE_REQUESTS", HTTP_UPGRADE_INSECURE_REQUESTS);
	// fillEnvCGI("HTTP_CONNECTION", HTTP_CONNECTION);
	fillEnvCGI("HTTP_ORIGIN", "http://" + getVirtualHost()[0].getHostPort().first); // host
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