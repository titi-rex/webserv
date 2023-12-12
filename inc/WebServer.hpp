#ifndef _WEB_SERVER_H__
# define _WEB_SERVER_H__
# include <string>

# include <stdexcept>
# include <iostream>
# include <fstream>	//open file
# include <sstream>	//stringstream

# include <unistd.h>
# include <sys/epoll.h>

typedef unsigned int long	uintptr_t;

# include "virtual_host.hpp"
# include "Socket.hpp"

# include "map_operator.hpp"
# include "deque_operator.hpp"
# include "vector_operator.hpp"

# define BACKLOG 5

class Request;

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

		std::map<int, Socket>				_socketsList;

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

		void		run(void);

	
		std::string	GET(std::string path);
		std::string	GET_error(int code);	// GET special pour error


		bool	is_server_named(v_host_ptr v_host, const std::string& name);
		v_host_ptr	selectServer(Socket& sk, Request& rq);

};

void						initLocation(t_location* loc);
void 						formatLine(std::string &line);
std::vector<std::string>	splitLine(const std::string& line);

std::ostream&	operator<<(std::ostream &os, const v_host_ptr v_host);
std::ostream&	operator<<(std::ostream &os, const t_virtual_host& v_host);

#endif
