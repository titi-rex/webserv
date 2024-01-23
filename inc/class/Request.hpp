/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:41:38 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/23 20:12:32 by tlegrand         ###   ########.fr       */
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

# include "container.hpp"


# define RL_MIN_LENGTH 14
# define RL_MAX_LENGTH 2500
# define HD_MAX_LENGTH 1000

# define METHOD_ENUM(TYPE) \
	TYPE(GET, 0) \
	TYPE(POST, 1) \
	TYPE(DELETE, 2) \
	TYPE(HEAD, 3) \
	TYPE(UNKNOW, 4) 

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

typedef enum {
	RL,
	HEADERS,
	BODYCLENGTH,
	BODYCHUNK,
	CGIHD,
	CGICL,
	CGIEOF,
	DONE,
}	e_parsingStatus;

class Request 
{
	private	:
		e_method 			_mId;
		std::string			_uri;
		std::string			_query;
		std::string			_pathInfo;
		std::string			_ext;
		std::string			_body;
		MapStrStr_t			_headers;
		bool				_needCgi;

		//tmp variable for parsing
		e_parsingStatus		_parsingStatus;
		std::string			_raw;
		size_t				_size;
		size_t				_lenChunk;

		//parsing request/cgi
		bool				_findBodySize(void);
		std::string			_extractRange(size_t& start, size_t& end, const char *set);
		bool				_is_method_known(std::string& test);
		bool				_parseRequestLine(void);
		bool				_parseHeaders(void);
		bool				_parseCgiHeaders(void);
		bool				_parseBodyByLength(std::string &body);
		bool				_parseBodyByChunk(std::string &body);

	public	:
		// a remettre en privé un jour
		std::string			_rline;
		std::string			_rStrStatus;
		std::string			_pathTranslated;
		std::string			_rbody;
		MapStrStr_t			_rheaders;
		size_t				_bodySizeExpected;

		std::string			response;

		Request(void);
		Request(const Request& src);
		Request&	operator=(const Request& src);
		~Request(void);


		e_method			getMid(void) const;
		bool				getNeedCgi(void) const;
		e_parsingStatus		getPstatus(void) const;
		const std::string	getMethodName(void) const;
		const std::string&	getUri(void) const;
		const std::string&	getBody(void) const;
		const std::string&	getQuery(void) const;
		const std::string&	getPathInfo(void) const;
		const std::string&	getExt(void) const;
		const std::string&	getRStrStatus(void) const;
		const MapStrStr_t&	getHeaders(void) const;

		void				setPathtranslated( std::string path );
		void				setRline( std::string line );
		void				setRheaders( std::string key, std::string value );
		void				setPstatus(e_parsingStatus newStatus);
		void				setRbody( std::string body );
		void				setResponse( std::string response );
		void				setExt( std::string extension );
		void				setNeedCgi(bool yes);
		void				setRStrStatus(const std::string& status, const MapStrStr_t* statusList = NULL, const std::string& defaultStatus = "500");

		bool				build(std::string raw = "");
		bool				addCgi(std::string	buff);
		void				makeResponse(void);
		void				clear(void);
};

std::ostream& operator<<(std::ostream& os, const Request& req);

#endif
