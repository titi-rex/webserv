/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 16:15:46 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/15 14:53:20 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_H__
# define _CLIENT_H__
# include "Socket.hpp"
# include "Request.hpp"
# include "virtual_host.hpp"

# define BUFFER_SIZE 2048
typedef enum 
{
	CREATED,
	ACCEPTED,
	READING,
	GATHERED,
	PROCEEDED,
	CGIWAIT,
	CGIOK,
	ERROR,
	FATAL,
	SENT,
} e_client_status;

class Client : public Socket
{
	private	:
		int		_serverEndPoint;
		int		_fd_cgi;
		size_t	_sizeLimit;

	void	_checkRequestSize(Request& rq);


	public	:
		v_host_ptr		host;
		Request			request;
		e_client_status	cstatus;
		bool			keepConnection;

		Client(void);
		Client(const Client& src);
		Client&	operator=(const Client& src);
		~Client(void);

		Client(size_t bodyLimit);

		void	accept(int sock_fd);
		bool	readRequest(void);
		bool	readCgi(void);
		void	proceedRequest(void);
		void	sendRequest(void);
		void	reset(void);
		
		int		getServerEndPoint(void) const;
		int		getFd_cgi(void) const;

		void	setFd_cgi(int fd_cgi);
};

std::ostream&	operator<<(std::ostream& os, const Client& Client);

#endif
