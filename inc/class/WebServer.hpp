/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 21:11:44 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/17 21:21:34 by tlegrand         ###   ########.fr       */
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
# include "Request.hpp"

typedef unsigned int long	uintptr_t;

# include "VirtualHost.hpp"
# include "SocketServer.hpp"
# include "Client.hpp"
# include "Request.hpp"

# include "utils.hpp"
# include "exceptions.hpp"
# include "container.hpp"


# define MAX_EVENTS 50
# define TIMEOUT -1
# define BACKLOG 50

# define ERROR_500_MSG "HTTPS/1.1 500 Server goes wrong\r\n\r\n<html>\n<head><title>500 Internal Server Error</title></head>\n<body>\n<center><h1>500 Internal Server Error</h1></center>\n<hr><center>nginx/1.25.3</center>\n</body>\n</html>\n"


class Request;

class WebServer 
{
	private	:
		int					_efd;				// fd permettqnt d'acceder a l'instance epoll
		size_t				_bodySizeLimit;		// limite generale de la taille maximum du body des clients pour tout le server, active si le virtual host ne precise pas (si == size_t max => pas de limite )
		std::string			_dirErrorPage;		// indique un repertoire specifique ou chercher les pqges d'erreur
		MapStrStr_t			_errorPage;			// indique ou chercher une page d'erreur specifique (est regarde en premier )
		VecVHost_t			_virtualHost;		// vector contenant tout les virtual hosts du server
		MapFdSockServ_t		_SocketServersList;	// map des SocketServers utilise par le server (key: fd, value: SocketServer)
		int					_highSocket;
		MapFdClient_t		_ClientList;		// map contenant les client du server
		MapFdClientPtr_t	_readyToProceedList;// list les client dont les request sont prete a etre proceder (fini de read)

		MapStrStr_t			_envCGI;		// variables d'environnement a envoyer aux CGI
		MapStrStr_t			_httpStatus;	//map http status <-> response line 

		WebServer(void);
		WebServer(const WebServer& src);
		WebServer&	operator=(const WebServer& src);

		void		_SocketServerList_init(void);
		void		_epoll_init(void);

		bool		_is_server_named(vHostPtr v_host, const std::string& name);
		vHostPtr	_selectServer(SocketServer& sk, Request& rq);




	public	:
		WebServer(std::string path);
		~WebServer(void);

		void				setVirtualHost(VecVHost_t vHost);
		void				setErrorPage(std::string key, std::string value); 
		void				setDirErrorPage(std::string dirErrorPage);
		void				setBodySizeLimit(size_t bodySizeLimit);
		
		VecVHost_t			getVirtualHost(void) const;
		MapStrStr_t&		getErrorPage(void);
		MapStrStr_t&		getHttpStatus(void);
		std::string			getDirErrorPage(void) const;
		size_t				getBodySizeLimit(void) const;

		Location			parseLocation(VecStr_t fileVec, VecStr_t sLine, uintptr_t *i);
		int					parseConf(std::string &line);
		void				parseServ(VecStr_t fileVec, uintptr_t start, uintptr_t end);
		void 				findServ(VecStr_t fileVec, uintptr_t *i);
		void				addVirtualHost(const VirtualHost& vHost);
		void				displayLocations(const VirtualHost& vHost);
		void				displayCGI(const VirtualHost& vHost);
		void				debugServ();

		void				run(void);
		void				addClient(int socketServerFd);

		void				modEpollList(int fd, int op, uint32_t events);
		void				deleteClient(int client_fd);

		void				handle_epoll_error(int event_id, uint32_t event_mask);
		void				handle_epollin(int event_id);
		void				handle_epollout(int event_id);


		void				process_rq(Client &cl);
		void				process_rq_error(Client &cl);

		// CGI
		void				fillElement(std::string key , std::string val);
		void				fillValueFromHeader(MapStrStr_t header, std::string key);
		void				fillValueFromCGI(MapStrStr_t cgi, std::string key, std::string value); 
		void				fillEnvCGI(const Client& client);
		void				execute_cgi(const std::string& script_path,Client& client);




		std::string			Method(Client &cl, Request & req, vHostPtr & v_host);	
		std::string			methodGet( Request & req, vHostPtr & v_host, std::string & path );
		std::string			GET_error(int code);	//obsolete
		void				getError(std::string status, Request& req);	// GET special pour error
		std::string			methodPost(Client &client);

};

void		formatLine(std::string &line);
VecStr_t	splitLine(const std::string& line);

#endif
