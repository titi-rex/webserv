/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 20:34:14 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/17 20:02:29 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
*	Webserv, a c++98 web server by 42 student
*	Copyright (C) 2023  tititeam 
*	https://www.gnu.org/licenses/gpl-3.0.html
*/


#include "WebServer.hpp"
#include "utils.hpp"

sig_atomic_t	g_status = 1;


// void	_epoll_error_handler(void)
// {
// 	if (errno == EBADF || errno == EINVAL)
// 		throw std::runtime_error("fatal: wrong epoll fd");
// 	if (errno == EFAULT)
// 		throw std::runtime_error("fatal: epoll cant write events");
// 	throw std::runtime_error("fatal: unknow");
// };


		// std::cout << "start:" << start << ",end:" << end << std::endl;
		// 	std::cout << "method is :" << tmp << ":" << std::endl;

#define RQL "POST / HTTP/1.1\r\nHost: mImi\r\nconTent-Length: 35\r\n\r\nthis is a body\nabody bodant!\n"
#define RQC "POST / HTTP/1.1\r\nHost: mImi\r\ntransfer-EncodinG: chunked\r\n\r\nf\r\nthis is a body\n\r\n7\r\n hello,\r\n7\r\n world!\r\n0\r\n"
#define RQG "GET /test/loulou.mimi?why=mimi HTTP/1.1\r\nHost: mImi\r\n\r\n"


int	main(int ac, char **av)
{
	if (ac != 2)
	{
		logERROR << "Error: configuration file required\nUsage: ./webserv [configuration file]";
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
		logERROR << e.what();
		logERROR << "errno value: " << errno;
		logERROR << strerror(errno);
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

