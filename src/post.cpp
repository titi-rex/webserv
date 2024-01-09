/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 12:37:59 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/08 15:37:38 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <cstdlib>
#include <sstream>
#include <ctime>

// curl -X POST -d "nom=John&email=john@example.com&message=Bonjour" http://example.com/process_form

/*
curl POST http://127.0.0.1:8080/shutdown
curl -X POST -d "nom=John&email=john@example.com" http://127.0.0.1:8080
*/

void execute_cgi(const std::string& script_path, const std::string& post_data) {
    int pipe_fd[2];
    pipe(pipe_fd);

    pid_t pid = fork();

    if (pid == 0) 
    {

    } 
    else if (pid > 0) 
    {

        close(pipe_fd[0]);
    } 
    else
        perror("Error : fork");
}
