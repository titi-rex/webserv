/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketBase.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 16:32:00 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/05 17:17:41 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SOCKET_BASE_H__
# define _SOCKET_BASE_H__
# include <sys/socket.h>
# include <arpa/inet.h>
# include <cstring>
# include <iostream>
# include <sstream>

class SocketBase 
{
	
	private	:
		int					_fd;
		struct sockaddr_in	_sin;
		int					_flags;
		std::string			_name;

	public	:

		SocketBase(void);
		SocketBase(const SocketBase& src);
		SocketBase&	operator=(const SocketBase& src);
		virtual ~SocketBase(void);

		SocketBase(int family, uint32_t addr, uint16_t port, int flags, int protocol);

		
		void	setFd(int fd);
		void	setName(void);
		void	setSin(int family, uint32_t addr, uint16_t port);
		
		int							getFd(void) const;
		const std::string&			getName(void) const;
		const struct sockaddr_in&	getSin(void) const;


		bool	operator==(SocketBase& ref);

		static uint32_t 	hstrtoint(std::string host);
		static std::string	hintostr(uint32_t raw);
		static std::string	str_sock_family(const struct sockaddr_in& sock);

};

#endif
