/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 16:15:46 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/17 20:30:28 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_H__
# define _CLIENT_H__
# include "Socket.hpp"
# include "Request.hpp"
# include "VirtualHost.hpp"

# define BUFFER_SIZE 2048

# define CLIENT_ENUM(TYPE) \
	TYPE(CREATED) \
	TYPE(ACCEPTED) \
	TYPE(READING) \
	TYPE(GATHERED) \
	TYPE(PROCEEDED) \
	TYPE(CGIWAIT) \
	TYPE(CGIOK) \
	TYPE(ERROR) \
	TYPE(FATAL) \
	TYPE(SENT) 

# define CLIENT_ENUM_TYPE(NAME) NAME,

typedef enum
{
	CLIENT_ENUM(CLIENT_ENUM_TYPE)
} e_client_status;

# define CLIENT_ENUM_CASE(NAME, ...) case NAME: return #NAME;

class Client : public Socket
{
	private	:
		int		_serverEndPoint;
		int		_fd_cgi[2];
		size_t	_sizeLimit;

	void	_checkRequestSize(Request& rq);


	public	:
		vHostPtr		host;
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
		
		const std::string	getStatusStr(void) const;
		int		getServerEndPoint(void) const;
		int*	getFd_cgi(void) const;

		void	setFd_cgi(int fd_cgi[2]);
};

std::ostream&	operator<<(std::ostream& os, const Client& Client);

typedef std::map<int, Client>		MapFdClient_t;
typedef std::map<int, Client*>		MapFdClientPtr_t;

#endif
