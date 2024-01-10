/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 16:15:46 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/10 10:44:00 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_H__
# define _CLIENT_H__
# include "Socket.hpp"
# include "Request.hpp"

# define BUFFER_SIZE 1024
typedef enum 
{
	CREATED,
	ACCEPTED,
	READING,
	GATHERED,
	PROCEEDED,
	CGIWAIT,
	ERROR,
	FATAL,
	SENT,
} e_client_status;

class Client : public Socket
{
	private	:
		int		_serverEndPoint;
		int		_fd_cgi;

	public	:
		Request			request;
		e_client_status	cstatus;
		bool		keepConnection;

		Client(void);
		Client(const Client& src);
		Client&	operator=(const Client& src);
		~Client(void);


		void	accept(int sock_fd);
		bool	readRequest(void);
		void	proceedRequest(void);
		void	sendRequest(void);
		void	reset(void);
		
		int		getServerEndPoint(void) const;
		int		getFd_cgi(void) const;

		void	setFd_cgi(int fd_cgi);
};

std::ostream&	operator<<(std::ostream& os, const Client& Client);

#endif