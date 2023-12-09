/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:41:38 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/08 22:12:54 by tlegrand         ###   ########.fr       */
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

# define RQ_SIZE_MAX 1024

using std::endl;
using std::cout;
using std::string;

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

		class Error : public std::runtime_error
		{
			public:
				int	status;
				Error(std::string str, int status);
		};
};

std::ostream& operator<<(std::ostream& os, const Request& req);

template <typename T, typename S>
std::ostream& operator<<(std::ostream &os, const std::map<T, S> dico)
{
	for (typename std::map<T, S>::const_iterator it = dico.begin(); it != dico.end(); ++it)
	{
		os << it->first << it->second << std::endl;
	}
	return (os);
} 

#endif
