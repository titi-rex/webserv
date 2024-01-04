/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:41:38 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/04 15:19:15 by jmoutous         ###   ########lyon.fr   */
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

typedef enum {eGET = 0, ePOST = 1, eDELETE = 2, eHEAD = 3, eUNKNOW = 4} e_method;

class Request 
{
	private	:
		size_t					_rId;
		e_method 				_mId;
		std::string				_uri;
		std::string				_body;
		std::map<std::string, std::string>	_headers;

		static size_t		_num_request;

		Request(void);
		Request(const Request& src);
		Request&	operator=(const Request& src);

		bool	_is_method_known(std::string& test);
		void	unchunk(std::istringstream& iss_raw);
		
	public	:
		Request(std::string raw);
		~Request(void);

		int								getRid(void) const;
		e_method 						getMid(void) const;
		std::string						getMethodName(void) const;
		const std::string&				getUri(void) const;
		const std::string&				getBody(void) const;
		const std::map<std::string, std::string>&	getHeaders(void) const;
};

std::ostream& operator<<(std::ostream& os, const Request& req);


#endif
