/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:41:38 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/04 17:45:47 by tlegrand         ###   ########.fr       */
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
		bool				_legit;
		short int			_mid;
		size_t				_rid;
		string				_method;
		string				_uri;
		string				_body;
		map<string, string>	_headers;

		static size_t		_num_request;

	public	:
		Request(void);
		Request(const Request& src);
		Request&	operator=(const Request& src);
		~Request(void);


		string	getUri(void) const;
		void	setUri(string uri);
		map<string, string>	getHeaders(void) const;
		void	setHeaders(map<string, string> headers);
		string	getBody(void) const;
		void	setBody(string body);
		bool	getLegit(void) const;
		void	setLegit(bool legit);

		Request(string raw);

		class Error : public runtime_error
		{
			public:
				int	status;
				Error(string str, int status) : runtime_error(str), status(status) {};
		};
};

template <typename T, typename S>
void	print_map(map<T,S> dico)
{
	for (typename map<T,S>::iterator it = dico.begin(); it != dico.end(); ++it)
	{
		cout << it->first << it->second << endl;
	}
	cout << endl;
} 

#endif
