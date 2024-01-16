/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   virtual_host.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 12:53:28 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/16 16:07:44 by tlegrand         ###   ########.fr       */
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
# include <limits>


class	t_location
{
	private :
	
	public:
		t_location(const t_location& src);
		t_location& operator=(const t_location& src);
		t_location();
		~t_location();

		int									lId;			//id de l'instance
		bool								isPath;			//if false its an extension ! vrai par default
		bool								autoIndex;		//directory listing, indique si le vhost doit repondre avec un index si la requet est un repertoire, si non renvoyer une erreur true par default
		std::string							uri_or_ext;		//valeur depuis laquelle la location est utilise (key)
		std::string							root;			//repertoire ou chercher la cible de la requete, si empty utiliser celle du virtual host
		std::string							index;			//fichier par default a chercher si la requet est un repertoire, si empty utiliser celle du virtual host
		std::vector<std::string>			allowMethod;	//METHOD autorise pour cette location, GET par default si empty
		std::pair<std::string, std::string>	redirection;	//substitue  la cible de la requete  ( ou la valeur de la location seulement, je crois pas mais plus de recherche sont necessaire), la method doit a nouveau etre utilise avec cette nouvelle cible
		
		bool								getIsPath() const;	
		bool								getAutoIndex() const;
		int									getLId() const;
		const std::string&						getUriOrExt() const;
		const std::string&						getRoot() const;
		const std::string&						getIndex() const;
		const std::vector<std::string>&			getAllowMethod() const;
		const std::pair<std::string, std::string>&	getRedirection() const;
		
		void								setIsPath(bool path);	
		void								setAutoIndex(std::string autoIndex);
		void								setLId(int lId);
		void								setUriOrExt(std::string uri_or_ext);
		void								setRoot(std::string root);
		void								setIndex(std::string index);
		void								setAllowMethod(std::vector<std::string>& allowMethod);
		void								setRedirection(std::vector<std::string>& sLine);

		void								isLegit() const;
};

class	t_virtual_host
{
	private:
	
	public:
		t_virtual_host(const t_virtual_host& src);
		t_virtual_host& operator=(const t_virtual_host& src);
		t_virtual_host();
		~t_virtual_host();
	
		int									sId;			//id du virtual host
		size_t								bodySize;		//limite de taille pour les client body
		std::string							root;			//repertoire par defaut ou chercher les cibles des request
		std::string							index;			//fichier par default a chercher si la requete est un repertoire
		std::string							dirCgi;			//repertoire des cgi (default/data/cgi-bin)
		std::pair<std::string, uint16_t>	host_port; 		// host:port ou le virtualhost doit ecouter, par default 0.0.0.0:80
		std::vector<std::string>			serverNames;	//list des nom auquels le virtual host doit repondre
		std::map<std::string, std::string>	cgi;			// s1 nom executable, s2 path exacte de l'executable
		std::map<std::string, t_location>	locations;		//list des locations enregistrer dans le virtual host

		int										getSId() const;
		size_t									getBodySize() const;
		const std::string&							getRoot() const;
		const std::string&							getIndex() const;
		const std::string&							getDirCgi() const;
		const std::pair<std::string, uint16_t>&		getHostPort() const;
		const std::vector<std::string>&				getServerNames() const;
		const std::map<std::string, std::string>&	getCgi() const;
		const std::map<std::string, t_location>&	getLocations() const;

		void								setBodySize(std::vector<std::string>& sLine);
		void								setRoot(std::vector<std::string>& sLine);
		void								setIndex(std::vector<std::string>& sLine);
		void								setDirCgi(std::vector<std::string>& sLine);
		void								setHostPort(std::vector<std::string>& sLine);
		void								setServerNames(std::vector<std::string>& sLine);
		void								setCgi(std::vector<std::string>& sLine, bool oneCgi);
		void								setLocations(t_location& newLoc);
};

typedef t_virtual_host* v_host_ptr;	//typedef pour un pointer vers un virtual host

#endif
