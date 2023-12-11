/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 22:58:30 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/11 14:58:20 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include "webserv.hpp" // je laisse ça comme ça?

std::string	WebServer::GET_error(int code)
{
	(void)code;
	return ("500 Internal Server Error\r\n\r\n");
}

// determine the requested methode
// std::string METHOD(Request& req, t_virtual_host* v_host);
// RL 200 ok
// heaers : dwdwd wdwD wLD 
// body kjfhdkjwsdfhikwsujd

std::string	WebServer::Method(Request & req, t_virtual_host* v_host)
{
	std::string	methods[4] = {"GET", "POST", "DELETE, ""HEAD"};
	std::string reqMethod = req.getMethod();
	int			i;

	for (i = 0; i < 5; ++i)
	{
		if (reqMethod == methods[i])
			break;
	}

	switch (i)
	{
		case 0:
			std::cout << "GET JUJU" << std::endl;
			return (GET("data/default_page/index.html"));
		case 1:
			std::cout << "POST" << std::endl;
			break;
		case 2:
			std::cout << "DELETE" << std::endl;
			break;
		case 3:
			std::cout << "HEAD" << std::endl;
			break;
		default:
			std::cout << "Not supported method!" << std::endl;
	};
}	


// std::string	get(Request rq, t_virtual_host v_host)
std::string	WebServer::GET(std::string path)
{
	std::string		response;
	std::ifstream	indexPage(path.c_str());

	if (indexPage.fail())
		throw std::runtime_error("error closing file");
	std::getline(indexPage, response, '\0');
	indexPage.close();
	response = "HTTP/1.0 200 OK\r\n\r\n" + response + "\r\n\r\n";
	return (response);
}
