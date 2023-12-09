#ifndef _WEB_SERVER_H__
# define _WEB_SERVER_H__
# include <string>
# include <map>
# include <vector>
# include <stdexcept>
# include <iostream>
# include <fstream>
# include <sstream>
# include <unistd.h>

typedef unsigned int long	uintptr_t;

typedef struct	s_location
{
	bool						isPath;	//if false its an extension !
	bool						autoIndex;  // directory listing
	int							lId;
	std::string					uri_or_ext;
	std::string					root;
	std::vector<std::string>	index;
	std::vector<std::string>	allowMethod;	// GET par default si empty
	std::string					redirection;
}	t_location;

typedef struct	s_virtual_host 
{
	bool								defaultServer;	// si plusieur server avec meme host/ports le premier est celui par default;
	int									sId;
	size_t								bodySize;
	std::string							host;
	std::string							root;
	std::string							index;
	std::vector<int>					ports;
	std::vector<std::string>			serverNames;
	std::map<std::string, std::string>	cgi;	// s1 nom binaire, s2 path
	std::map<std::string, t_location>	locations;
}	t_virtual_host;

typedef t_virtual_host* v_host_ptr;


class WebServer 
{
	private	:
		size_t								_bodySizeLimit;
		std::string							_dirErrorPage;
		std::map<std::string, std::string>	_errorPage;
		std::vector<t_virtual_host>			_virtualHost;


		int	_efd;

		WebServer(void);
		WebServer(const WebServer& src);
		WebServer&	operator=(const WebServer& src);

		void	_socketList_init(void);
		void	_epoll_init(void);


	public	:
		std::map<int, v_host_ptr>			_socketsList;
		WebServer(std::string path);
		~WebServer(void);

		std::vector<t_virtual_host>	getVirtualHost(void) const;
		void	setVirtualHost(std::vector<t_virtual_host> virtualHost);
		std::map<std::string,std::string>&	getErrorPage(void);
		void	setErrorPage(std::string key, std::string value); 
		std::string	getDirErrorPage(void) const;
		void	setDirErrorPage(std::string dirErrorPage);
		size_t	getBodySizeLimit(void) const;
		void	setBodySizeLimit(size_t bodySizeLimit);

		int		parseConf(std::string &line);
		void	parseServ(std::vector<std::string> fileVec, uintptr_t start, uintptr_t end);
		void 	findServ(std::vector<std::string> fileVec, uintptr_t *i);
		void	addVirtualHost(const t_virtual_host& virtualHost);

		void	run(void);

		void	send_response(int client_fd);	//	notre futur method GET
};

#endif
