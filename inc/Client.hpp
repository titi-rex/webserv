/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 16:15:46 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/05 18:36:55 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_H__
# define _CLIENT_H__
# include "SocketServer.hpp"
# include "Request.hpp"

class Client 
{
	private	:
		int					_sockfd;
		struct sockaddr_in	_sin;
		Request	_rq;
		int		_fd_server;
		int		_fd_cgi;
		int		_status;

	public	:

		Client(void);
		Client(const Client& src);
		Client&	operator=(const Client& src);
		~Client(void);

		SocketServer	get_SocketServer(void) const;
		void	set_SocketServer(SocketServer SocketServer);

		Request	get_rq(void) const;
		void	set_rq(Request rq);
		


		int		getFd_server(void) const;
		void	setFd_server(int fd_server);
		int		getFd_cgi(void) const;
		void	setFd_cgi(int fd_cgi);
		int		getStatus(void) const;
		void	setStatus(int status);

};

#endif
