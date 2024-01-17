/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 16:32:00 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/05 18:21:25 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SocketServer_BASE_H__
# define _SocketServer_BASE_H__
# include <cstring>
# include <iostream>
# include <sstream>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>

class Socket 
{
	protected:
		int					_fd;
		struct sockaddr_in	_sin;
		int					_flags;
		std::string			_name;

	public	:

		Socket(void);
		Socket(const Socket& src);
		Socket&	operator=(const Socket& src);
		virtual ~Socket(void);

		Socket(int family, uint32_t haddr, uint16_t hport, int flags = SOCK_STREAM, int protocol = 0);

		void	setFd(int fd);
		void	setSin(int family, uint32_t addr, uint16_t port);
		virtual void	setName(void);
		
		int							getFd(void) const;
		const std::string&			getName(void) const;
		const struct sockaddr_in&	getSin(void) const;

		bool	operator==(Socket& ref);


		void	bind(void);
		void	connect(void);
		void	listen(int backlog);
		void	accept(int fd);

		static uint32_t 	hstrtoint(std::string host);
		static std::string	hintostr(uint32_t raw);
		static std::string	str_sock_family(const struct sockaddr_in& sock);

};

std::ostream&	operator<<(std::ostream& os, const Socket& Socket);
std::ostream&	operator<<(std::ostream &os, const struct sockaddr_in& sock);

#endif
