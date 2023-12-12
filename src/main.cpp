/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 20:34:14 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/12 15:22:04 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
*	Webserv, a c++98 web server by 42 student
*	Copyright (C) 2023  tititeam 
*	https://www.gnu.org/licenses/gpl-3.0.html
*/


#include "webserv.hpp"
#include "WebServer.hpp"
#include "define.hpp"

sig_atomic_t	g_status = 1;

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Error: configuration file required\nUsage: ./webserv [configuration file]" << std::endl;
		return (1);
	}
	sig_init(sig_handler);
	try 
	{
		WebServer	server(av[1]);
		server.run();
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << "errno value: " << errno << std::endl;
		std::cerr << strerror(errno) << std::endl;
		
	}
	return (0);
}


// request line \r\n
// header\r\n
// header\r\n
// \r\n
// body\r\n

// authorized (recommended) in uri
// ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=

/*	chunked body encoding example

7\r\n
Mozilla\r\n
11\r\n
Developer Network\r\n
0\r\n
\r\n

*/

