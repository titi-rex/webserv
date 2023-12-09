/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 20:31:17 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/09 23:04:51 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


/*
*	Webserv, a c++98 web server by 42 student
*	Copyright (C) 2023  tititeam 
*	https://www.gnu.org/licenses/gpl-3.0.html
*/





#ifndef _WEBSERV_H__
# define _WEBSERV_H__
# include <iostream>
# include <fstream>
# include <vector>
# include <sstream>
# include <stdexcept>


# include <sys/time.h>
# include <sys/socket.h>
# include <sys/epoll.h>
# include <fcntl.h>
# include <netinet/in.h>
# include <cstring>
#include <netdb.h>
#include <signal.h>

# include <unistd.h>
# include <errno.h>


# include "Request.hpp"
# include "WebServer.hpp"



void	sig_init(void (*handler) (int sig));
void	sig_handler(int sig);


std::ostream&	operator<<(std::ostream &os, const struct sockaddr_in& sock);
std::string		str_sock_family(const struct sockaddr_in& sock);

std::string	hintostr(uint32_t raw);
uint32_t	hstrtoint(std::string host);


int			sock_init(std::string host, in_port_t port, int backlog);
std::string	recept_request(int sock_listen, int&client_fd);
void		send_response(int client_fd, std::string response);




std::ostream&	operator<<(std::ostream &os, const t_virtual_host& v_host);
std::ostream&	operator<<(std::ostream &os, const v_host_ptr v_host);
template <typename T> std::ostream&	operator<<(std::ostream &os, const std::vector<T>& vec);

# define MAXLINE 24
# define MAX_EVENTS 5
# define TIMEOUT 2000
# define FIRST_PORT 8080
# define HOST_ANY "0.0.0.0"
# define HOST_LB "127.0.0.1"
# define BACKLOG 5


#endif

//  string 127 0 0 1
// 1111111 00000000 00000000 00000001 binaire
// 2130706433 decimal
// hexa 
