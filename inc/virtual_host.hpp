/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   virtual_host.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 12:53:28 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/13 16:27:08 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _V_HOST__
# define _V_HOST__
# include <vector>
# include <map>
# include <string>
# include <arpa/inet.h> 
# include <iostream> 
# include <map>
# include <stdexcept>
# include <algorithm>
# include <string>
# include <sstream>

class	t_location
{
	private :
	
	public:
		t_location(const t_location& src);
		t_location& operator=(const t_location& src);
		t_location();
		~t_location();

		bool								isPath;	//if false its an extension ! vrai par default
		bool								autoIndex;  // directory listing true par default
		int									lId;
		std::string							uri_or_ext;
		std::string							root;
		std::string							index;
		std::vector<std::string>			allowMethod;	// GET par default si empty
		std::string							redirection;
		
		bool								getIsPath() const;	
		bool								getAutoIndex() const;
		int									getLId() const;
		std::string							getUriOrExt() const;
		std::string							getRoot() const;
		std::string							getIndex() const;
		std::vector<std::string>			getAllowMethod() const;
		std::string							getRedirection() const;
		
		void								setIsPath(bool path);	
		void								setAutoIndex(std::string autoIndex);
		void								setLId(int lId);
		void								setUriOrExt(std::string uri_or_ext);
		void								setRoot(std::string root);
		void								setIndex(std::string index);
		void								setAllowMethod(std::vector<std::string>	allowMethod);
		void								setRedirection(std::string redirection);
};

class	t_virtual_host 
{
	private:
	
	public:
		t_virtual_host(const t_virtual_host& src);
		t_virtual_host& operator=(const t_virtual_host& src);
		t_virtual_host();
		~t_virtual_host();
	
		bool								defaultServer;	// si plusieur server avec meme host/ports le premier est celui par default;
		int									sId;
		size_t								bodySize;
		std::string							root;
		std::string							index;
		std::pair<std::string, uint16_t>	host_port; // host par default localhost
		std::vector<std::string>			serverNames;
		std::map<std::string, std::string>	cgi;	// s1 nom executable, s2 path
		std::map<std::string, t_location>	locations;

		bool								getDefaultServer() const;
		int									getSId() const;
		size_t								getBodySize() const;
		std::string							getRoot() const;
		std::string							getIndex() const;
		std::pair<std::string, uint16_t>	getHostPort() const;
		std::vector<std::string>			getServerNames() const;
		std::map<std::string, std::string>	getCgi() const;
		std::map<std::string, t_location>	getLocations() const;

		void								setDefaultServer(bool defaultServer);
		void								setSId(int sId);
		void								setBodySize(size_t bodySize);
		void								setRoot(std::string root);
		void								setIndex(std::string index);
		void								setHostPort(std::pair<std::string, uint16_t> host_port);
		void								setServerNames(std::vector<std::string> serverNames);
		void								setCgi(std::map<std::string, std::string> cgi);
		void								setLocations(std::map<std::string, t_location> locations);
};

typedef t_virtual_host* v_host_ptr;

#endif
