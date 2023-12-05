/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:41:38 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/05 15:21:55 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _REQUEST_H__
# define _REQUEST_H__
# include <iostream>
# include <string>
# include <map>
# include <stdexcept>
# include <sstream>

# define RQ_SIZE_MAX 1024


using namespace std;

class Request 
{
	private	:
		size_t				_rId;
		short int			_mId;
		string				_method;
		string				_uri;
		string				_body;
		map<string, string>	_headers;

		static size_t		_num_request;

		Request(void);
		Request(const Request& src);
		Request&	operator=(const Request& src);

		bool	_is_method_known(string& test);
		
	public	:
		Request(string raw);
		~Request(void);

		int							getRid(void) const;
		const string&				getMethod(void) const;
		const string&				getUri(void) const;
		const string&				getBody(void) const;
		const map<string, string>&	getHeaders(void) const;

		class Error : public runtime_error
		{
			public:
				int	status;
				Error(string str, int status) : runtime_error(str), status(status) {};
		};
};

ostream& operator<<(ostream& os, const Request& req);

template <typename T, typename S>
void	print_map(map<T,S> dico)
{
	for (typename map<T,S>::iterator it = dico.begin(); it != dico.end(); ++it)
	{
		cout << it->first << it->second << endl;
	}
} 

#endif
