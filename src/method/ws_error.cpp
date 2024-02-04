/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_error.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 14:37:11 by tlegrand          #+#    #+#             */
/*   Updated: 2024/02/04 13:07:40 by tlegrand         ###   ########.fr       */
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
	else
	{
		if (_errorPage.count(status) && access(_errorPage[status].c_str(), F_OK | R_OK) == 0)
			pageDir = _errorPage[status];
		else
			pageDir = _dirErrorPage + status + ".html";	
		req.setRbody(getFile(pageDir));	//throw open file (fatal)
		req.makeResponse();
	}
}
