/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 20:31:17 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/12 20:02:42 by tlegrand         ###   ########.fr       */
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
# include <stdexcept>

# include "map_operator.hpp"
# include "deque_operator.hpp"
# include "vector_operator.hpp"

# include "virtual_host.hpp"
# include "Request.hpp"

typedef enum {
	logDEBUG,
	logINFO,
	logWARNING,
	logERROR,
}	e_logLevel;


#define FDEBUG "\001\033[32m\002"
#define FINFO  "\001\033[0;37m\002"
#define FWARNING "\001\033[0;33m\002"
#define FERROR "\001\033[0;31m\002"
#define END "\001\033[0m\002"

void	sig_init(void (*handler) (int sig));
void	sig_handler(int sig);

std::string	getFile(std::string& path);
bool		check_access(std::string& path);

bool	 	isDirListReq(Request & req);
std::string	dirList(Request & req, v_host_ptr & v_host);
std::string	findLocation(Request & req, v_host_ptr & v_host);

int	wrap_tolower(int c);
int	wrap_iscntrl(int c);

std::string&	ltrim(std::string& str, const char* set);
std::string&	rtrim(std::string& str, const char* set);
std::string&	trim(std::string& str, const char* set);


#endif
