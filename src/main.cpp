/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 20:34:14 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/04 17:46:25 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
*	Webserv, a c++98 web server by 42 student
*	Copyright (C) 2023  tititeam 
*	https://www.gnu.org/licenses/gpl-3.0.html
*/


#include "webserv.hpp"

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Error: configuration file required\nUsage: ./webserv [configuration file]" << std::endl;
		return (1);
	}
	(void)av;
	try 
	{
		Request	rq("GET / HTTP/1.1\r\nUser-Agent: python-requests/2.22.0\r\nAccept-Encoding: gzip\r\nAccept: */*\r\nConnection: keep-alive\r\nhost: none\r\nAccept-Language: fr\r\n\r\nbodyyyy boddy");
		
	}
	catch (Request::Error & e)
	{
		std::cout << e.status << " " << e.what() << std::endl;
		
	}
	return (0);
}

