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

void	sig_init(void (*handler) (int sig));
void	sig_handler(int sig);

std::string	getFile(std::string& path);
bool		check_access_error(std::string& path);

bool	 	isDirListReq(Request & req);
std::string	dirList(Request & req, v_host_ptr & v_host);

#endif
