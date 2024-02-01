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
# include <sstream>
# include <limits>
# include <unistd.h>
# include "Location.hpp"
# include "container.hpp"


class	VirtualHost
{
	private:
		const std::string		_dirPrefix;
		std::string				root;			//repertoire par defaut ou chercher les cibles des request
		std::string				index;			//fichier par default a chercher si la requete est un repertoire
		std::string				dirCgi;			//repertoire des cgi (default/data/cgi-bin)
		PairStrUint16_t			host_port; 		// host:port ou le virtualhost doit ecouter, par default 0.0.0.0:80
		VecStr_t				serverNames;	//list des nom auquels le virtual host doit repondre
		MapStrStr_t				cgi;			// s1 nom exension, s2 path exacte de l'executable(doit etre depuis dirCgi mais pas encore fait)
		MapStrLoc_t				locations;		//list des locations enregistrer dans le virtual host
	
	public:
		VirtualHost(void);
		VirtualHost(const VirtualHost& src);
		VirtualHost& operator=(const VirtualHost& src);
		~VirtualHost(void);

		VirtualHost(const std::string& dirPrefix);

		const std::string&		getRoot() const;
		// const std::string&		getDirPrefix() const;
		const std::string&		getIndex() const;
		const std::string&		getDirCgi() const;
		const PairStrUint16_t&	getHostPort() const;
		const VecStr_t&			getServerNames() const;
		const MapStrStr_t&		getCgi() const;
		const MapStrLoc_t&		getLocations() const;

		void					setRoot(VecStr_t& sLine);
		void					setIndex(VecStr_t& sLine);
		void					setDirCgi(VecStr_t& sLine);
		void					setHostPort(VecStr_t& sLine);
		void					setServerNames(VecStr_t& sLine);
		void					setCgi(VecStr_t& sLine, bool oneCgi);
		void					setLocations(Location& newLoc);

		void					secureUpload(void);
};

typedef VirtualHost* 				vHostPtr;
typedef std::vector<VirtualHost>	VecVHost_t;
typedef std::deque<vHostPtr>		DeqVHostPtr_t;

std::ostream&	operator<<(std::ostream &os, const vHostPtr v_host);
std::ostream&	operator<<(std::ostream &os, const VirtualHost& v_host);

#endif
