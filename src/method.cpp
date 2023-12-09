/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 22:58:30 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/09 23:00:34 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

std::string	WebServer::GET_error(int code)
{
	(void)code;
	return ("500 Internal Server Error\r\n\r\n");
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
