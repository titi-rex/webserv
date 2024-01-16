/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualHost.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 12:53:28 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/16 20:24:47 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _VIRTUAL_HOST_H__
# define _VIRTUAL_HOST_H__
# include <vector>
# include <map>
# include <string>
# include <iostream> 
# include <map>
# include <stdexcept>
# include <algorithm>
# include <string>
# include <sstream>
# include <limits>
# include <arpa/inet.h>
# include <unistd.h>

# include "Location.hpp"


class	VirtualHost
{
	private:
	
	public:
		VirtualHost(const VirtualHost& src);
		VirtualHost& operator=(const VirtualHost& src);
		VirtualHost();
		~VirtualHost();
	
		int									sId;			//id du virtual host
		size_t								bodySize;		//limite de taille pour les client body
		std::string							root;			//repertoire par defaut ou chercher les cibles des request
		std::string							index;			//fichier par default a chercher si la requete est un repertoire
		std::string							dirCgi;			//repertoire des cgi (default/data/cgi-bin)
		std::pair<std::string, uint16_t>	host_port; 		// host:port ou le virtualhost doit ecouter, par default 0.0.0.0:80
		std::vector<std::string>			serverNames;	//list des nom auquels le virtual host doit repondre
		std::map<std::string, std::string>	cgi;			// s1 nom executable, s2 path exacte de l'executable
		std::map<std::string, Location>	locations;		//list des locations enregistrer dans le virtual host

		int										getSId() const;
		size_t									getBodySize() const;
		const std::string&							getRoot() const;
		const std::string&							getIndex() const;
		const std::string&							getDirCgi() const;
		const std::pair<std::string, uint16_t>&		getHostPort() const;
		const std::vector<std::string>&				getServerNames() const;
		const std::map<std::string, std::string>&	getCgi() const;
		const std::map<std::string, Location>&	getLocations() const;

		void								setBodySize(std::vector<std::string>& sLine);
		void								setRoot(std::vector<std::string>& sLine);
		void								setIndex(std::vector<std::string>& sLine);
		void								setDirCgi(std::vector<std::string>& sLine);
		void								setHostPort(std::vector<std::string>& sLine);
		void								setServerNames(std::vector<std::string>& sLine);
		void								setCgi(std::vector<std::string>& sLine, bool oneCgi);
		void								setLocations(Location& newLoc);
};

typedef VirtualHost* v_host_ptr;	//typedef pour un pointer vers un virtual host

#endif
