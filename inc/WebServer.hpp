#ifndef _WEB_SERVER_H__
# define _WEB_SERVER_H__
# include <string>
# include <map>
# include <vector>
# include <stdexcept>
# include <iostream>

typedef struct	s_location
{
	int							lId;
	bool						isPath;	//if false its an extension !
	std::string					uri_or_ext;
	std::string					root;
	std::vector<std::string>	index;
	bool						autoIndex;  // directory listing
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


class WebServer 
{
	private	:
		size_t								_bodySizeLimit;
		std::string							_dirErrorPage;
		std::map<std::string,std::string>	_errorPage;
		std::vector<t_virtual_host>			_virtualHost;

		WebServer(void);
		WebServer(const WebServer& src);
		WebServer&	operator=(const WebServer& src);

	public	:
		WebServer(std::string path);
		~WebServer(void);

		std::vector<t_virtual_host>	getVirtualHost(void) const;
		void	setVirtualHost(std::vector<t_virtual_host> virtualHost);
		std::map<std::string,std::string>	getErrorPage(void) const;
		void	setErrorPage(std::map<std::string,std::string> errorPage);
		std::string	getDirErrorPage(void) const;
		void	setDirErrorPage(std::string dirErrorPage);
		size_t	getBodySizeLimit(void) const;
		void	setBodySizeLimit(size_t bodySizeLimit);

};

#endif