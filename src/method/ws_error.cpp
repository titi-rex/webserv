/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_error.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 14:37:11 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/28 19:22:09 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include "utils.hpp"

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
