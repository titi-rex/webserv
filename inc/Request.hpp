/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:41:38 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/12 12:49:48 by jmoutous         ###   ########lyon.fr   */
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

# include "map_operator.hpp"

# define RQ_SIZE_MAX 1024
# define N_METHOD 5

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
			BODY,
		}	parsing_status;
		
		parsing_status 	_pstatus;
		std::string		_raw;

		std::string							_rline;
		std::string							_rbody;
		std::map<std::string, std::string>	_rheaders;



		bool	_is_method_known(std::string& test);
		void	unchunk(std::istringstream& iss_raw);
		
	public	:
		std::string				response;
		short int				rstatus;


		Request(void);
		Request(const Request& src);
		Request&	operator=(const Request& src);
		~Request(void);
		
		bool	build2(std::string raw);
		bool	build(std::string raw);
		bool	addCgi(std::string	buff);
		void	clear(void);

		int								getRid(void) const;
		e_method 						getMid(void) const;
		const std::string				getMethodName(void) const;
		const std::string&				getUri(void) const;
		const std::string&				getBody(void) const;
		const std::map<std::string, std::string>&	getHeaders(void) const;

		void	setRheaders( std::string key, std::string value );
};

std::ostream& operator<<(std::ostream& os, const Request& req);


#endif

