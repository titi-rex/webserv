/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   virtual_host.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 12:53:28 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/12 20:37:06 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _V_HOST__
# define _V_HOST__
# include <vector>
# include <map>
# include <string>

typedef struct	s_location
{
	bool						isPath;	//if false its an extension ! vrai par default
	bool						autoIndex;  // directory listing true par default
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
	std::string							root;
	std::string							index;
	std::pair<std::string, int>			host_port; // host par default localhost
	std::vector<std::string>			serverNames;
	std::map<std::string, std::string>	cgi;	// s1 nom executable, s2 path
	std::map<std::string, t_location>	locations;
}	t_virtual_host;

typedef t_virtual_host* v_host_ptr;

#endif
