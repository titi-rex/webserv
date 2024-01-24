/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_error.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 14:37:11 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/24 13:17:36 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include "utils.hpp"

// trouver rl depuis map //lire fichier ??
std::string	getRL(std::string status)
{
	std::string	rl = "HTTP/1.1 " + status + " OK\r\n";


	return (rl);
}

std::string	getPageByDir(std::string dir, std::string code)
{
	std::string	target = dir + code + ".html";

	if (target.compare(0, 1, ".") != 0)
		target = "." + target;

	if (access(target.c_str(),  F_OK | R_OK))
		throw std::runtime_error("error: access by dir");
	std::string body = getFile(target);

	return (body);
}


void	WebServer::getError(std::string status, Request& req)
{
	std::string	pageDir;

	req.setRline(_httpStatus[req.getRStrStatus()]);
	// Use for redirection
	if (status.at(0) == '3')
		req.makeResponse();
	else try 
	{
		if (_errorPage.count(status) && check_access(_errorPage[status]))
			pageDir = _errorPage[status];
		else
			pageDir = _dirErrorPage + status + ".html";	
		req.setRbody(getFile(pageDir));	//throw open file (fatal)
		req.makeResponse();
	}
	catch (std::exception & e)
	{
		std::clog << e.what() << ", for page: " << pageDir << std::endl;
		throw std::runtime_error("699: get_error: couldn't get default page");
	}
}
