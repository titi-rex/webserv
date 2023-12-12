/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:41:38 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/12 15:06:47 by tlegrand         ###   ########.fr       */
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
# define N_METHOD 4


class Request 
{
	private	:
		size_t					_rId;
		short int				_mId;
		std::string				_method;
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
		const std::string&				getMethod(void) const;
		const std::string&				getUri(void) const;
		const std::string&				getBody(void) const;
		const std::map<std::string, std::string>&	getHeaders(void) const;
};

std::ostream& operator<<(std::ostream& os, const Request& req);



#endif
