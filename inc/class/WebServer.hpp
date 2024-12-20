/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 21:11:44 by tlegrand          #+#    #+#             */
/*   Updated: 2024/02/13 12:55:08 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _WEB_SERVER_H__
# define _WEB_SERVER_H__
# include <string>
# include <stdexcept>
# include <iostream>
# include <fstream>
# include <sstream>
# include <cstring>
# include <limits>
# include <string> 

# include <unistd.h>
# include <sys/epoll.h>
# include <errno.h>
# include <signal.h>
# include <sys/stat.h>
# include "Request.hpp"

typedef unsigned int long	uintptr_t;

# include "VirtualHost.hpp"
# include "SocketServer.hpp"
# include "Client.hpp"
# include "Request.hpp"

# include "utils.hpp"
# include "container.hpp"


# define MAX_EVENTS 50
# define TIMEOUT -1
# define BACKLOG 50
# define WBS_VER "WebServ - version 3.2"
# define CRLF "\r\n"

class Request;

class WebServer 
{
	private	:
		int					_efd;				// fd permettqnt d'acceder a l'instance epoll
		size_t				_bodySizeLimit;		// limite generale de la taille maximum du body des clients pour tout le server, active si le virtual host ne precise pas (si == size_t max => pas de limite )
		std::string			_dirPrefix;			//prefix pour tout les directory
		MapStrStr_t			_errorPage;			// indique ou chercher une page d'erreur specifique (est regarde en premier )
		VecVHost_t			_virtualHost;		// vector contenant tout les virtual hosts du server
		MapFdSockServ_t		_SocketServersList;	// map des SocketServers utilise par le server (key: fd, value: SocketServer)
		MapFdClient_t		_ClientList;		// map contenant les client du server
		MapFdClientPtr_t	_readyToProceedList;// list les client dont les request sont prete a etre proceder (fini de read)

		MapStrStr_t			_envCGI;		// variables d'environnement a envoyer aux CGI
		MapStrStr_t			_httpStatus;	//map http status <-> response line 
		MapStrStr_t			_contentType;
		MapFdClientPtr_t	_fdCgi;			//map liant fd d'une cgi a son client

		WebServer(void);
		WebServer(const WebServer& src);
		WebServer&	operator=(const WebServer& src);

		void		_SocketServerList_init(void);
		void		_epoll_init(void);

		bool		_is_server_named(vHostPtr v_host, const std::string& name);
		void		_selectServer(SocketServer& sk, Client& cl);
		void		_initContentTypeMap(void );
		void		_closeAllFd(bool log);
		void		_initHttpStatus(void);

	public	:
		WebServer(std::string path);
		~WebServer(void);

		void				setVirtualHost(const VecVHost_t& vHost);
		void				setErrorPage(const VecStr_t& sLine); 
		void				setBodySizeLimit(const VecStr_t& sLine);
		void				setDirPrefix(const VecStr_t& sLine);

		VecVHost_t			getVirtualHost(void) const;
		const std::string&	getDirPrefix(void) const;
		const MapStrStr_t&	getErrorPage(void) const;
		const MapStrStr_t&	getHttpStatus(void) const;
		const MapStrStr_t&	getContentType(void) const;
		size_t				getBodySizeLimit(void) const;

		// parsing
		int					parseConf(std::string &line);
		void 				findServ(VecStr_t fileVec, uintptr_t *i);
		void				parseServ(VecStr_t fileVec, uintptr_t start, uintptr_t end);
		Location			parseLocation(VecStr_t fileVec, VecStr_t sLine, uintptr_t *i);

		void				addVirtualHost(const VirtualHost& vHost);

		void				displayLocations(const VirtualHost& vHost);
		void				displayCGI(const VirtualHost& vHost);
		void				debugServ();

		// server
		void				initSocket(void);
		void				run(void);

		void				addClient(int socketServerFd);
		void				modEpollList(int fd, int op, uint32_t events);
		void				deleteClient(int client_fd);

		void				handle_epollerr(int event_id, uint32_t event_mask);
		void				handle_epollin(int event_id);
		void				handle_epollout(int event_id);
		void				error_epoll(std::string& status, int event_id);

		void				process_rq(Client &cl);
		void				process_rq_error(Client &cl);


		// CGI
		void				fillElement(std::string key , std::string val);
		void				fillValueFromHeader(MapStrStr_t header, std::string key);
		void				fillValueFromCGI(MapStrStr_t cgi, std::string key, std::string value); 
		void				fillEnvCGI(const Client& client);
		void				execute_cgi(const std::string& script_path,Client& client);

		// Method
		void				Method(Client& cl);	
		void				methodGet(Client& client, bool withBody);
		void				methodPost(Client& client);
		void				methodDelete(Client& client);
		void				getError(std::string status, Request& req);
		void				imageGet(Client & cl );

		bool 				createFile(const std::string& fileName, const std::string& content, const std::string uploadDir);
		bool				extractFileData(const std::string& part, std::string& filename, std::string& content);
		bool				processPostRequest(const std::string& requestBody, Client& client);
		VecStr_t			splitByBoundary(const std::string& requestBody, const std::string& boundary);
		std::string			extractBoundary(const std::string& requestBody);


};

void		formatLine(std::string &line);
VecStr_t	splitLine(const std::string& line);
void		setDir(std::string& target, const VecStr_t& sLine, const std::string& partName, const std::string& prefix);

#endif
