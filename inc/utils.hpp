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
# include <iomanip>
# include <stdexcept>

# include "VirtualHost.hpp"
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

#define LOGLEVEL 0

template <typename T>
e_logLevel	operator<<(e_logLevel lvl, T log)
{
	switch (lvl)
	{
		case logDEBUG:
		{
			if (logDEBUG >= LOGLEVEL)
				std::clog << FDEBUG << std::setw(12) << "[DEBUG] : " << log << END << std::endl;
			break;
		}
		case logINFO:
		{
			if (logINFO >= LOGLEVEL)
				std::clog << FINFO << std::setw(12) << " ---------- " << log << END << std::endl;
			break;
		}
		case logWARNING:
		{
			if (logWARNING >= LOGLEVEL)
				std::clog << FWARNING << std::setw(12) << "[WARNING] : " << log << END << std::endl;
			break;
		}
		case logERROR:
		{
			if (logERROR >= LOGLEVEL)
				std::clog << FERROR << std::setw(12) << "[ERROR] : " << log << END << std::endl;
			break;
		}
	}
	return (lvl);
}


void	sig_init(void (*handler) (int sig));
void	sig_handler(int sig);

std::string	getFile(std::string& path);
bool		check_access(std::string& path);

bool	 	isDirListReq(Request & req);
std::string	dirList(Request & req, vHostPtr & v_host);
std::string	findLocation(Request & req, vHostPtr & v_host);

int		wrap_tolower(int c);
int		wrap_iscntrl(int c);
void	wrap_close(int fd);

std::string&	ltrim(std::string& str, const char* set);
std::string&	rtrim(std::string& str, const char* set);
std::string&	trim(std::string& str, const char* set);

void	getDate( char * date );
int		lengthSize( int contentLength );

#endif
