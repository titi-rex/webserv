/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 12:14:39 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/12 14:53:55 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SOCKET_H__
# define _SOCKET_H__
# include <stdint.h>
# include <deque>
# include <map>
# include <iostream>
# include <sstream>

# include <sys/socket.h>
# include <arpa/inet.h> //# include <netinet/in.h>
// # include <unistd.h>
#include <strings.h>

# include "virtual_host.hpp"

# include <unistd.h>

class Socket 
{
	
	private	:
		int			_fd;
		uint32_t	_host;
		uint16_t	_port;
		std::string	_name;

	public	:
		std::deque<v_host_ptr>	v_hosts;

		Socket(void);
		Socket(const Socket& src);
		Socket&	operator=(const Socket& src);
		~Socket(void);

		Socket(uint32_t host, uint16_t port);

		int			getFd(void) const;
		void		setFd(int fd);
		uint32_t	getHost(void) const;
		uint16_t	getPort(void) const;
		std::string	getName(void) const;
		void		setName(void);

		bool	operator==(Socket& ref);

		bool	is_already_used(std::map<int, Socket>& tmp, v_host_ptr v_host);
		void	sockInit(int backlog);

		static uint32_t 	hstrtoint(std::string host);
		static std::string	hintostr(uint32_t raw);
		static std::string	str_sock_family(const struct sockaddr_in& sock);
		
};

std::ostream& operator<<(std::ostream& os, const Socket& socket);
std::ostream&	operator<<(std::ostream &os, const struct sockaddr_in& sock);



#endif
