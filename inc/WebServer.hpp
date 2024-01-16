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

# include "virtual_host.hpp"
# include "SocketServer.hpp"
# include "Client.hpp"
# include "Request.hpp"
# include "utils.hpp"
# include "exceptions.hpp"

# include "map_operator.hpp"
# include "deque_operator.hpp"
# include "vector_operator.hpp"

# define MAX_EVENTS 50
# define TIMEOUT 30000
# define BACKLOG 50

# define ERROR_500_MSG "HTTPS/1.1 500 Server goes wrong\r\n\r\n<html>\n<head><title>500 Internal Server Error</title></head>\n<body>\n<center><h1>500 Internal Server Error</h1></center>\n<hr><center>nginx/1.25.3</center>\n</body>\n</html>\n"


class Request;

class WebServer 
{
	private	:
		int									_efd;				// fd permettqnt d'acceder a l'instance epoll
		size_t								_bodySizeLimit;		// limite generale de la taille maximum du body des clients pour tout le server, active si le virtual host ne precise pas (si == size_t max => pas de limite )
		std::string							_dirErrorPage;		// indique un repertoire specifique ou chercher les pqges d'erreur
		std::map<std::string, std::string>	_errorPage;			// indique ou chercher une page d'erreur specifique (est regarde en premier )
		std::vector<t_virtual_host>			_virtualHost;		// vector contenant tout les virtual hosts du server
		std::map<int, SocketServer>			_SocketServersList;	// map des SocketServers utilise par le server (key: fd, value: SocketServer)
		int		_highSocket;
		std::map<int, Client>				_ClientList;		// map contenant les client du server
		std::map<int, Client*>				_readyToProceedList;	// list les client dont les request sont prete a etre proceder (fini de read)

		std::map<std::string, std::string>	_envCGI;			// variables d'environnement a envoyer aux CGI

		WebServer(void);
		WebServer(const WebServer& src);
		WebServer&	operator=(const WebServer& src);

		void	_SocketServerList_init(void);
		void	_epoll_init(void);

		bool		_is_server_named(v_host_ptr v_host, const std::string& name);
		v_host_ptr	_selectServer(SocketServer& sk, Request& rq);


		void		_send_response(int client_fd, std::string response);


	public	:
		WebServer(std::string path);
		~WebServer(void);


		void								setVirtualHost(std::vector<t_virtual_host> virtualHost);
		void								setErrorPage(std::string key, std::string value); 
		void								setDirErrorPage(std::string dirErrorPage);
		void								setBodySizeLimit(size_t bodySizeLimit);
		
		std::vector<t_virtual_host>			getVirtualHost(void) const;
		std::map<std::string,std::string>&	getErrorPage(void);
		std::string							getDirErrorPage(void) const;
		size_t								getBodySizeLimit(void) const;

		t_location	parseLocation(std::vector<std::string> fileVec, std::vector<std::string> sLine, uintptr_t *i);
		int			parseConf(std::string &line);
		void		parseServ(std::vector<std::string> fileVec, uintptr_t start, uintptr_t end);
		void 		findServ(std::vector<std::string> fileVec, uintptr_t *i);
		void		addVirtualHost(const t_virtual_host& virtualHost);
		void		displayLocations(const t_virtual_host& virtualHost);
		void		displayCGI(const t_virtual_host& virtualHost);
		void		debugServ();

		void	run(void);
		void	addClient(int socketServerFd);

		void	modEpollList(int fd, int op, uint32_t events);
		void	deleteClient(int client_fd);

		void	handle_epollerr(int event_id);
		void	handle_epollhup(int event_id);
		void	handle_epollin(int event_id);
		void	handle_epollout(int event_id);


		void	process_rq(Client &cl);
		void	process_rq_error(Client &cl);

		// CGI
		void initEnvCGI();
		void fillElement(std::string key , std::string val);
		void fillEnvCGI(const Client& client);
		void execute_cgi(const std::string& script_path);




		std::string	Method(Request & req, v_host_ptr & v_host);	
		std::string	methodGet( Request & req, v_host_ptr & v_host, std::string & path );
		std::string	GET_error(int code);	//obsolete
		std::string	GET_error2(std::string status);	// GET special pour error

		std::string	methodPost(std::string path);

};

void						initLocation(t_location* loc);
void 						formatLine(std::string &line);
std::vector<std::string>	splitLine(const std::string& line);

std::ostream&	operator<<(std::ostream &os, const v_host_ptr v_host);
std::ostream&	operator<<(std::ostream &os, const t_virtual_host& v_host);

#endif
