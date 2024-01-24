/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 20:27:47 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/24 15:12:41 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _LOCATION_H__
# define _LOCATION_H__
# include <string>
# include <iostream> 
# include <stdexcept>
# include <algorithm>
# include <unistd.h>
# include "container.hpp"
# include "log.hpp"


class	Location
{
	private :
		bool				isPath;			//if false its an extension ! vrai par default
		bool				autoIndex;		//directory listing, indique si le vhost doit repondre avec un index si la requet est un repertoire, si non renvoyer une erreur true par default
		bool				upload;			//indiaue si la location autoriseles upload
		std::string			_key;		//valeur depuis laquelle la location est utilise (key)
		std::string			root;			//repertoire ou chercher la cible de la requete, si empty utiliser celle du virtual host
		std::string			index;			//fichier par default a chercher si la requet est un repertoire, si empty utiliser celle du virtual host
		std::string			uploadDir;		//repertoire ou les upload doivent etre fait
		VecStr_t			allowMethod;	//METHOD autorise pour cette location, GET par default si empty
		PairStrStr_t		redirection;	//substitue  la cible de la requete  ( ou la valeur de la location seulement, je crois pas mais plus de recherche sont necessaire), la method doit a nouveau etre utilise avec cette nouvelle cible

	public:
		Location(const Location& src);
		Location& operator=(const Location& src);
		Location();
		~Location();

		bool				getIsPath() const;	
		bool				getAutoIndex() const;
		bool				getUpload() const;
		const std::string&	getUriOrExt() const;
		const std::string&	getRoot() const;
		const std::string&	getIndex() const;
		const std::string&	getUploadDir() const;
		const VecStr_t&		getAllowMethod() const;
		const PairStrStr_t&	getRedirection() const;
		
		void				setIsPath(bool path);	
		void				setAutoIndex(const VecStr_t& sLine);
		void				setUriOrExt(const std::string& key);
		void				setRoot(const VecStr_t& sLine);
		void				setIndex(const VecStr_t& sLine);
		void				setUploadDir(const VecStr_t& sLine);
		void				setAllowMethod(const VecStr_t& sLine);
		void				setRedirection(const VecStr_t& sLine);

		void				isLegit() const;
};

typedef std::map<std::string, Location>	MapStrLoc_t;

#endif
