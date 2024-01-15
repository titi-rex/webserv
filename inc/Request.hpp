/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:41:38 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/15 11:20:51 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _REQUEST_H__
# define _REQUEST_H__
# include <string>
# include <sstream>
# include <iostream>
# include <map>
# include <stdexcept>
# include <algorithm>
# include <cstring>
# include <cctype>
# include <climits>

# include "map_operator.hpp"


# define RL_MIN_LENGTH 14
# define RL_MAX_LENGTH 2500
# define HD_MAX_LENGTH 1000

# define METHOD_ENUM(TYPE) \
	TYPE(eGET, 0) \
	TYPE(ePOST, 1) \
	TYPE(eDELETE, 2) \
	TYPE(eHEAD, 3) \
	TYPE(eUNKNOW, 4) 

# define METHOD_ENUM_TYPE(NAME, VALUE) NAME = (VALUE),

typedef enum
{
	METHOD_ENUM(METHOD_ENUM_TYPE)
} e_method;

# define METHOD_ENUM_CASE(NAME, ...) case NAME: return #NAME;

#define PLUS_ONE(...) + 1
enum {	
	METHOD_COUNT = (0 METHOD_ENUM(PLUS_ONE) - 1)
};


int	wrap_tolower(int c);
int	wrap_iscntrl(int c);

std::string&	ltrim(std::string& str, const char* set);
std::string&	rtrim(std::string& str, const char* set);
std::string&	trim(std::string& str, const char* set);

class Request 
{
	private	:
		size_t					_rId;
		e_method 				_mId;
		std::string				_uri;
		std::string				_body;
		std::map<std::string, std::string>	_headers;

		static size_t		_num_request;


		typedef enum {
			RL,
			HEADERS,
			BODYCLENGTH,
			BODYCHUNK,
			DONE,
		}	parsing_status;
		
		parsing_status 	_pstatus;
		std::string		_raw;
		size_t			_size;


		bool	_findBodySize(void);
		std::string	_extractRange(size_t& start, size_t& end, const char *set);
		bool		_is_method_known(std::string& test);

		bool	_parseRequestLine(void);
		bool	_parseHeaders(void);
		bool	_parseBodyByLength(void);
		bool	_parseBodyByChunk(void);
		
		void		unchunk(std::istringstream& iss_raw);
		
	public	:
		// a remettre en privé un jour
		std::string							_rline;
		std::string							_rbody;
		std::map<std::string, std::string>	_rheaders;

		
		size_t			_bodyCount;
		size_t			_bodySizeExpected;
		std::string				response;
		short int				rstatus;


		Request(void);
		Request(const Request& src);
		Request&	operator=(const Request& src);
		~Request(void);

		
		bool	build(std::string raw = "");
		bool	addCgi(std::string	buff);
		void	clear(void);

		int								getRid(void) const;
		e_method 						getMid(void) const;
		const std::string				getMethodName(void) const;
		const std::string&				getUri(void) const;
		const std::string&				getBody(void) const;
		const std::map<std::string, std::string>&	getHeaders(void) const;

		void	setRline( std::string line );
		void	setRheaders( std::string key, std::string value );
		void	setRstatus( short int status );
		void	setRbody( std::string body );
		void	setResponse( std::string response );
};

std::ostream& operator<<(std::ostream& os, const Request& req);


#endif

