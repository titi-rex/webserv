/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:43:41 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/12 15:28:35 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

size_t	Request::_num_request = 0;





Request::Request(void) : _rId(_num_request++), _mId(eUNKNOW) {};
Request::Request(const Request& src) : _rId(_num_request++) {*this = src;};
Request&	Request::operator=(const Request& src) 
{
	if (this == &src)
		return (*this);
	_mId = src._mId;
	_uri = src._uri;
	_body = src._body;
	_headers = src._headers;
	return (*this);
};

Request::~Request(void) {};


int 				Request::getRid(void) const { return (this->_rId); };
e_method 			Request::getMid(void) const {return (this->_mId);}
const std::string& 	Request::getUri(void) const { return (this->_uri); };
const std::string&	Request::getBody(void) const { return (this->_body); };
const std::map<std::string, std::string>&	Request::getHeaders(void) const { return (this->_headers); };

const std::string	Request::getMethodName(void) const
{
	switch (this->_mId)
	{
		METHOD_ENUM(METHOD_ENUM_CASE)
		default:
			return ("UNKNOW");
	}	
}

void	Request::setRline( std::string line ) { this->_rline = line; };
void	Request::setRheaders( std::string key, std::string value ) { this->_rheaders[key] = value; };
void	Request::setRstatus( short int status ) { this->rstatus = status; };
void	Request::setRbody( std::string body ) { this->_rbody = body; };
void	Request::setResponse( std::string response ) { this->response = response; };


bool	Request::_is_method_known(std::string & test)
{
	std::string	ref[METHOD_COUNT] = {"GET", "POST", "DELETE", "HEAD"};
	
	for (int i = 0; i < METHOD_COUNT; ++i)
	{
		if (ref[i].compare(test) == 0)
		{
			_mId = (e_method) i;
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
			throw std::runtime_error("400 Bad Chunk");
		_body += tmp + '\n';
	}
}




/**
 * @brief 
 * take raw input with minimal size (BUFFER_SIZE)
 * etape: 	1 RL
 * 			2 headers
 * 			3 body
 * RL minimal size : "GET / HTTP/1.1" = 14 char
 */
bool	Request::build2(std::string	raw)
{
	switch (_pstatus)
	{
		case RL:
		{
			std::cout << "RL" << std::endl;
			break;
		}
		case HEADERS:
		{
			std::cout << "HEADERS" << std::endl;
			break;
		}
		case BODY:
		{
			std::cout << "BODY" << std::endl;
			break;
		}
		default:
		{
			std::cout << "default" << std::endl;
			break;
		}
	} 
}

bool	Request::build(std::string raw)
{
	static std::string	buff;
	

	buff += raw;
	if (buff.find("\r\n\r\n") == std::string::npos)
	{
		std::cout << "end header not found" << std::endl;
		return (false);
	}
	if (buff.find("GET") == std::string::npos)
		throw std::runtime_error("400: only accept get");
	
	std::istringstream	iss_raw(buff);

	buff.clear();
	
	std::string			method;
	std::string			tmp;

	iss_raw >> method;
	iss_raw >> _uri;
	iss_raw >> tmp;

	if (tmp.compare("HTTP/1.1") != 0)
		throw std::runtime_error("505 Wrong HTTP version");
	if (_is_method_known(method) == false)
		throw std::runtime_error("501 Method not implemented");
	if (_uri[0] != '/')
		throw std::runtime_error("400 Bad URI");

	std::getline(iss_raw, tmp, '\n');
	while (std::getline(iss_raw, tmp, '\n') && tmp != "" && tmp != "\r")
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
		throw std::runtime_error("400 No Host Header");
	if (_headers.find("transfert-encoding:") == _headers.end())
	{
		std::getline(iss_raw, _body, '\0');
		return (true);
	}
	if (_headers["transfert-encoding:"] != "chunked")
		throw std::runtime_error("400 Unknow encoding");
	unchunk(iss_raw);
	return (true);
}

bool	Request::addCgi(std::string	buff)
{
	_body += buff;
	return (true);
}


void	Request::clear(void)
{
	_rId = _num_request++;
	_mId = eUNKNOW;
	_uri.clear();
	_body.clear();
	_headers.clear();
	rstatus = 0;
}

std::ostream& operator<<(std::ostream& os, const Request& req)
{
	os << "rid: " << req.getRid() << std::endl;
	os << "RL: " << req.getMethodName() << " " << req.getUri() << " HTTP/1.1" << std::endl;
	os << "Headers :" << std::endl;
	os << req.getHeaders();
	os << "Body :" << std::endl;
	os << "{" << req.getBody() << "}" << std::endl;
	return (os);
};

