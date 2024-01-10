/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   virtual_host.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 12:53:28 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/10 12:10:26 by lboudjem         ###   ########.fr       */
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

		bool								isPath;			//if false its an extension ! vrai par default
		bool								autoIndex;		//directory listing, indique si le vhost doit repondre avec un index si la requet est un repertoire, si non renvoyer une erreur true par default
		int									lId;			//id de l'instance
		std::string							uri_or_ext;		//valeur depuis laquelle la location est utilise
		std::string							root;			//repertoire ou chercher la cible de la requete, si empty utiliser celle du virtual host
		std::string							index;			//fichier par default a chercher si la requet est un repertoire, si empty utiliser celle du virtual host
		std::vector<std::string>			allowMethod;	//METHOD autorise pour cette location, GET par default si empty
		std::string							redirection;	//substitue  la cible de la requete  ( ou la valeur de la location seulement, je crois pas mais plus de recherche sont necessaire), la method doit a nouveau etre utilise avec cette nouvelle cible
		
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
	
		bool								defaultServer;	// si plusieur server avec meme host/ports (obsolete) le premier est celui par default;
		int									sId;			//id du virtual host
		size_t								bodySize;		//limite de taille pour les client body
		std::string							root;			//repertoire par defaut ou chercher les cibles des request
		std::string							index;			//fichier par default a chercher si la requete est un repertoire
		std::pair<std::string, uint16_t>	host_port; 		// host:port ou le virtualhost doit ecouter, par default 0.0.0.0:80
		std::vector<std::string>			serverNames;	//list des nom auquels le virtual host doit repondre
		std::map<std::string, std::string>	cgi;			// s1 nom executable, s2 path exacte de l'executable
		std::map<std::string, t_location>	locations;		//list des locations enregistrer dans le virtual host

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

		void								cgi_handler(void);
};

typedef t_virtual_host* v_host_ptr;	//typedef pour un pointer vers un virtual host

class locationRedirection : public std::exception {
	private:
		std::string	_redirection;
		locationRedirection();

	public:
		locationRedirection(const std::string & redir);
		const char*	what( void ) const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW;
		~locationRedirection() _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW;
};


#endif
