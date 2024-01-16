/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_utils.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 20:11:34 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/16 21:54:29 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

bool	WebServer::_is_server_named(vHostPtr v_host, const std::string& name)
{
	for (VecStr_t::const_iterator it = v_host->getServerNames().begin(); it != v_host->getServerNames().end(); ++it)
	{
		if (*it == name)
			return (true);
	}
	return (false);
}

vHostPtr	WebServer::_selectServer(SocketServer& sk, Request& rq)
{
	if (rq.getHeaders().count("host:"))
	{		
		for (DeqVHostPtr_t::iterator it = sk.v_hosts.begin(); it != sk.v_hosts.end(); ++it)
		{
			if (_is_server_named(*it, rq.getHeaders().at("host:")))
				return (*it);
		}
	}
	return (sk.v_hosts[0]);
};
