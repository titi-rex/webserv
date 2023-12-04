/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:43:41 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/04 17:49:09 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

size_t	Request::_num_request = 0;

Request::Request(void) :  _legit(true), _mid(-1), _rid(_num_request++) {};

Request::Request(const Request& src) 
{
	
	this->setUri(src.getUri());
	this->setHeaders(src.getHeaders());
	this->setBody(src.getBody());
	this->setLegit(src.getLegit());
	
};

Request&	Request::operator=(const Request& src) 
{
	if (this == &src)
		return (*this);
	
	this->setUri(src.getUri());
	this->setHeaders(src.getHeaders());
	this->setBody(src.getBody());
	this->setLegit(src.getLegit());
	return (*this);
};

Request::~Request(void){};



string	Request::getUri(void) const { return (this->_uri); };
void	Request::setUri(string uri) { this->_uri = uri; };

map<string, string>	Request::getHeaders(void) const { return (this->_headers); };
void	Request::setHeaders(map<string, string> headers) { this->_headers = headers; };

string	Request::getBody(void) const { return (this->_body); };
void	Request::setBody(string body) { this->_body = body; };

bool	Request::getLegit(void) const { return (this->_legit); };
void	Request::setLegit(bool legit) { this->_legit = legit; };


bool	is_method_known(string & test, short int & mid)
{
	string	ref[3] = {"GET", "HEAD", "POST"};
	
	for (size_t i = 0; i < 3; ++i)
	{
		if (ref[i].compare(test))
		{
			mid = i;
			return (true);
		}
	}
	return (false);
}

Request::Request(string raw) :  _legit(true), _mid(-1), _rid(_num_request++)
{
	string	tmp;
	string	tmp2;
	istringstream	is(raw);
	int len = raw.find("\r\n", 0, 2);
	
	if (len < 14)
		throw Error("RQ too short", 400);
	cout << "RL lenght " << len << endl;
	is >> _method;
	is >> _uri;
	is >> tmp;

	cout << "Request line is :" << _method << " " << _uri << " " << tmp << endl;
	if (tmp.compare("HTTP/1.1") != 0)
		throw Error("Wrong HTTP version", 505);
	if (is_method_known(_method, _mid) == false)
		throw Error("Method not implemented", 501);
	if (_uri[0] != '/')
		throw Error("Bad URI", 400);
	int	count = len;
	len = raw.find("\r\n\r\n", 0, 4);
	cout << "HD lenght " << len - count << endl;
	cout << "RQ + HD lenght " << len << endl;
	is >> tmp;
	while (getline(is, tmp, '\n'))
	{	
		if (tmp == "\r")
			break ;
		istringstream iss(tmp);
		string key;
		string value;
		iss >> key;
		iss >> value;
		_headers[key] = value;

	}
	getline(is, tmp, '\0');
	_body = tmp;
	print_map(_headers);
	cout << _body << endl;

};
	
// request line \r\n
// header\r\n
// header\r\n
// \r\n
// body\r\n

// ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=