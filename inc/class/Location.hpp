/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 20:27:47 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/16 20:31:59 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _LOCATION_H__
# define _LOCATION_H__
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


class	Location
{
	private :
	
	public:
		Location(const Location& src);
		Location& operator=(const Location& src);
		Location();
		~Location();

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

#endif
