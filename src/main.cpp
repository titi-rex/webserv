/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 20:34:14 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/10 14:06:22 by tlegrand         ###   ########.fr       */
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



int	main(int ac, char **av)
{
	// try
	// {
	// 	SocketServer s1(Socket::hstrtoint("0.0.0.0"), 8080);
	// 	SocketServer s2(Socket::hstrtoint("127.0.0.1"), 8080);
	// 	std::cout << s1 << std::endl;
	// 	std::cout << s2 << std::endl;
	// 	s1.bind();
	// 	std::cout << "s1 bind" << std::endl;
	// 	s1.listen(2);
	// 	std::cout << "s1 listen" << std::endl;
	// 	s2.bind();
	// 	std::cout << "s2 bind" << std::endl;
	// 	s2.listen(2);
	// 	std::cout << "s2 listen" << std::endl;
		
	// }
	// catch(const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// 	std::cerr << "errno value: " << errno << std::endl;
	// 	std::cerr << strerror(errno) << std::endl;
	// }
	
	// exit(0);
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

