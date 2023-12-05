/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:43:41 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/05 15:25:12 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

size_t	Request::_num_request = 0;

ostream& operator<<(ostream& os, const Request& req)
{
	os << "rid: " << req.getRid() << endl;
	os << "RL: " << req.getMethod() << " " << req.getUri() << " HTTP/1.1" << endl;
	os << "Headers :" << endl;
	print_map(req.getHeaders());
	os << "Body :" << endl;
	os << "{" << req.getBody() << "}" << endl;
	return (os);
};


Request::Request(void) : _rId(_num_request++), _mId(-1) {};
Request::Request(const Request& src) : _rId(_num_request++) {*this = src;};
Request&	Request::operator=(const Request& src) 
{
	if (this == &src)
		return (*this);
	_mId = src._mId;
	_method = src._method;
	_uri = src._uri;
	_body = src._body;
	_headers = src._headers;
	return (*this);
};
Request::~Request(void){};


int 			Request::getRid(void) const { return (this->_rId); };
const string&	Request::getMethod(void) const { return (this->_method); };
const string& 	Request::getUri(void) const { return (this->_uri); };
const string&	Request::getBody(void) const { return (this->_body); };
const map<string, string>&	Request::getHeaders(void) const { return (this->_headers); };


bool	Request::_is_method_known(string & test)
{
	string	ref[3] = {"GET", "HEAD", "POST"};
	
	for (size_t i = 0; i < 3; ++i)
	{
		if (ref[i].compare(test) == 0)
		{
			_mId = i;
			return (true);
		}
	}
	return (false);
}

Request::Request(string raw) : _rId(_num_request++), _mId(-1) 
{
	istringstream	iss_raw(raw);
	string			tmp;

	iss_raw >> _method;
	iss_raw >> _uri;
	iss_raw >> tmp;

	if (tmp.compare("HTTP/1.1") != 0)
		throw Error("Wrong HTTP version", 505);
	if (_is_method_known(_method) == false)
		throw Error("Method not implemented", 501);
	if (_uri[0] != '/')
		throw Error("Bad URI", 400);

	iss_raw >> tmp;
	while (getline(iss_raw, tmp, '\n') && tmp != "\r" && tmp != "")
	{	
		istringstream iss(tmp);
		string key;
		iss >> key;
		iss >> _headers[key];
	}
	getline(iss_raw, _body, '\0');
}
