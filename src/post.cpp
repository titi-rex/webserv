/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 12:37:59 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/10 12:13:53 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "WebServer.hpp"
#include "virtual_host.hpp"
#include <cstdlib>
#include <sstream>
#include <ctime>  
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <stdio.h> 
#include <ctype.h>
#include <unistd.h> 

// curl -X POST -d "nom=John&email=john@example.com&message=Bonjour" http://example.com/process_form

/*
curl POST http://127.0.0.1:8080/shutdown
curl -X POST -d "nom=John&email=john@example.com" http://127.0.0.1:8080
*/

void execute_cgi(const std::string& script_path, const std::string& post_data) 
{
    int pipe_fd[2];
    pipe(pipe_fd);
    
    pid_t pid = fork();

    if (pid == 0) 
    {
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        char* const argv[] = {const_cast<char*>(script_path.c_str()), NULL};
        char* const envp[] = {nullptr};
        
        execve(argv[0], argv, NULL);

        perror("error : execve");
        exit(EXIT_FAILURE);
    } 
    else if (pid > 0) 
    {
        close(pipe_fd[1]);
        
        waitpid(pid, NULL, 0);
            
        char buffer[500];
        ssize_t read_bytes;

        while ((read_bytes = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
            std::cout << "buffer = " << buffer << std::endl;

        close(pipe_fd[0]);
    }
    else
        perror("Error : fork");
}

void    t_virtual_host::cgi_handler(void)
{
    
}

// int main(int argc, char** argv)
// {
//     execute_cgi("data/cgi/a.out", "ohoh");
//     cgi_handler();
    
//     return (0);
// }
