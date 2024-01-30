/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 20:31:17 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/30 20:28:18 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


/*
*	Webserv, a c++98 web server by 42 student
*	Copyright (C) 2023  tititeam 
*	https://www.gnu.org/licenses/gpl-3.0.html
*/


#ifndef _UTILS_H__
# define _UTILS_H__

# include <signal.h>
# include <iostream>
# include <iomanip>
# include <stdexcept>

# include "VirtualHost.hpp"
# include "Request.hpp"
# include "Client.hpp"
# include "log.hpp"



void			sig_init(void (*handler) (int sig));
void			sig_handler(int sig);

std::string		getFile(const std::string& path);
bool			check_access(std::string& path);

void			dirList(Client& cl, const std::string& root);
bool			translatePath(Client& cl);

int				wrap_tolower(int c);
int				wrap_iscntrl(int c);
void			wrap_close(int fd);

std::string&	ltrim(std::string& str, const char* set);
std::string&	rtrim(std::string& str, const char* set);
std::string&	trim(std::string& str, const char* set);

void			getDate( char * date );
int				lengthSize( int contentLength );

#endif
