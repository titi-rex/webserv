/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketServer.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 12:14:39 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/05 18:32:51 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SocketServer_H__
# define _SocketServer_H__
# include <iostream>
# include <sstream>

# include <sys/socket.h>
# include <arpa/inet.h> 
# include <cstring>

# include "VirtualHost.hpp"
# include "Socket.hpp"
# include "container.hpp"


class SocketServer : public Socket
{
	public	:
		DeqVHostPtr_t	v_hosts;

		SocketServer(void);
		SocketServer(const SocketServer& src);
		SocketServer&	operator=(const SocketServer& src);
		~SocketServer(void);

		SocketServer(uint32_t haddr, uint16_t hport);	
};

std::ostream&	operator<<(std::ostream& os, const SocketServer& SocketServer);

typedef std::map<int, SocketServer>	MapFdSockServ_t;

#endif
