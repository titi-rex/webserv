/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 20:34:14 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/06 14:47:13 by lboudjem         ###   ########.fr       */
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

using namespace std;

// int	main(int ac, char **av)
// {
// 	if (ac != 2)
// 	{
// 		std::cerr << "Error: configuration file required\nUsage: ./webserv [configuration file]" << std::endl;
// 		return (1);
// 	}
// 	(void)av;
// 	try 
// 	{
// 		// Request	rq(GET_EX_BASE);
// 		// cout << rq << endl;
// 		Request	rq2(GET_EX_CHUNK);
// 		cout << rq2;
		
// 	}
// 	catch (Request::Error & e)
// 	{
// 		std::cout << e.status << " " << e.what() << std::endl;
		
// 	}
// 	return (0);
// }




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




