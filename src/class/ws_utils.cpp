/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_utils.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 20:11:34 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/12 20:19:39 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

bool	WebServer::_is_server_named(v_host_ptr v_host, const std::string& name)
{
	for (std::vector<std::string>::iterator it = v_host->serverNames.begin(); it != v_host->serverNames.end(); ++it)
	{
		if (*it == name)
			return (true);
	}
	return (false);
}

v_host_ptr	WebServer::_selectServer(Socket& sk, Request& rq)
{
	if (rq.getHeaders().count("host:"))
	{		
		for (std::deque<v_host_ptr>::iterator it = sk.v_hosts.begin(); it != sk.v_hosts.end(); ++it)
		{
			if (_is_server_named(*it, rq.getHeaders().at("host:")))
				return (*it);
		}
	}
	return (sk.v_hosts[0]);
};
