/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:43:41 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/08 15:31:06 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Request.hpp"

size_t	Request::_num_request = 0;

std::ostream& operator<<(std::ostream& os, const Request& req)
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


int 				Request::getRid(void) const { return (this->_rId); };
const std::string&	Request::getMethod(void) const { return (this->_method); };
const std::string& 	Request::getUri(void) const { return (this->_uri); };
const std::string&	Request::getBody(void) const { return (this->_body); };
const std::map<std::string, std::string>&	Request::getHeaders(void) const { return (this->_headers); };


bool	Request::_is_method_known(std::string & test)
{
	std::string	ref[3] = {"GET", "HEAD", "POST"};
	
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

int	wrap_tolower(int c)
{
	return (std::tolower(c));
}

void	Request::unchunk(std::istringstream& iss_raw)
{
	std::string	tmp;
	size_t		chunk_size = 1;
	
	while (std::getline(iss_raw, tmp, '\n'))
	{
		chunk_size = std::strtol(tmp.c_str(), NULL, 16);
		if (chunk_size == 0)
			return ;
		std::getline(iss_raw, tmp, '\n');
		if (tmp.size() != chunk_size)
			throw Error("Bad Chunk", 400);
		_body += tmp + '\n';
	}
}

Request::Request(std::string raw) : _rId(_num_request++), _mId(-1) 
{
	std::istringstream	iss_raw(raw);
	std::string			tmp;

	iss_raw >> _method;
	iss_raw >> _uri;
	iss_raw >> tmp;

	if (tmp.compare("HTTP/1.1") != 0)
		throw Error("Wrong HTTP version", 505);
	if (_is_method_known(_method) == false)
		throw Error("Method not implemented", 501);
	if (_uri[0] != '/')
		throw Error("Bad URI", 400);

	getline(iss_raw, tmp, '\n');
	while (getline(iss_raw, tmp, '\n') && tmp != "" && tmp != "\r")
	{	
		std::istringstream iss(tmp);
		std::string key;
		std::string value;
		iss >> key;
		iss >> value;
		std::transform(key.begin(), key.end(), key.begin(), wrap_tolower);
		std::transform(value.begin(), value.end(), value.begin(), wrap_tolower);
		_headers[key] = value;
	}

	if (_headers.find("host:") == _headers.end())
		throw Error("No Host Header", 400);
	if (_headers.find("transfert-encoding:") == _headers.end())
	{
		getline(iss_raw, _body, '\0');
		return ;
	}
	if (_headers["transfert-encoding:"] != "chunked")
		throw Error("Unknow encoding", 400);
	unchunk(iss_raw);
}

// attention si strtol overflow -> thow
// attention si getline throw

Request::Error::Error(std::string str, int status) : runtime_error(str), status(status) {};
