/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:43:41 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/15 13:49:27 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

size_t	Request::_num_request = 0;


Request::Request(void) : _rId(_num_request++), _mId(eUNKNOW), _pstatus(RL), _size(0), _lenChunk(-1), _bodySizeExpected(0) {};

Request::Request(const Request& src) : _rId(_num_request++) {*this = src;};

Request&	Request::operator=(const Request& src) 
{
	if (this == &src)
		return (*this);
	_mId = src._mId;
	_uri = src._uri;
	_body = src._body;
	_headers = src._headers;
	_pstatus = src._pstatus;
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

std::string	Request::_extractRange(size_t& start, size_t& end, const char *set)
{
	start = end + 1;
	end = _raw.find_first_of(set, start);	
	return (_raw.substr(start, end - start));
}

bool	Request::_findBodySize(void)
{
	if (_mId == eHEAD || _mId == eGET)
		return (true);

	std::map<std::string, std::string>::iterator itCl = _headers.find("content-length");
	std::map<std::string, std::string>::iterator itTe = _headers.find("transfer-encoding");

	if (itCl == _headers.end() && itTe == _headers.end())
		throw std::runtime_error("411: Length Required");
	else if (itCl != _headers.end() && itTe != _headers.end())
		throw std::runtime_error("400: Confusing length indication");
	if (itCl != _headers.end())
	{
		_bodySizeExpected = std::strtoul(itCl->second.c_str(), NULL, 10);
		if (_bodySizeExpected == ULONG_MAX)
			throw std::runtime_error("413: Request Entity Too Large");
		return (true);
	}
	if (itTe->second != "chunked")
		throw std::runtime_error("500: Unknow encoding");
	_bodySizeExpected = 0;
	return (false);
}


/**
 * @brief return true if we need more data to complete the RL, false else
 */
bool	Request::_parseRequestLine(void)
{
	std::string	tmp;
	size_t		start = 0;
	size_t		end = -1;
	
	if (_raw.size() > RL_MAX_LENGTH)
		throw std::runtime_error("414: URI too long");
	if (_raw.find("\r\n") == std::string::npos)
		return (true);
	if (_raw.size() < RL_MIN_LENGTH)
		throw std::runtime_error("400: RL too short");
	tmp = _extractRange(start, end, " ");
	if (_is_method_known(tmp) == false)
		throw std::runtime_error("501: Method not implemented");
	_uri = _extractRange(start, end, " ");
	if (_uri.at(0) != '/')
		throw std::runtime_error("400 Bad URI");
	tmp = _extractRange(start, end, " \n");
	if (tmp.compare("HTTP/1.1") == false)
		throw std::runtime_error("505 Wrong HTTP version");
	_raw.erase(0, end + 1);
	_pstatus = HEADERS;
	return (false);
}


/**
 * @brief return true if more data is needed to parse header, else false
 */
bool	Request::_parseHeaders(std::map<std::string, std::string> &headers)
{
	size_t		start = 0;
	size_t		end = -1;

	while (_raw.find("\r\n") != std::string::npos)
	{	
		if (std::strncmp(_raw.c_str(), "\r\n", 2) == 0)
		{
			_raw.erase(0, 2);
			if (headers.count("host") == 0)
				throw std::runtime_error("400: No Host Header");
			_findBodySize() ? _pstatus = BODYCLENGTH : _pstatus = BODYCHUNK;
			return (false);
		}
		end = -1;
		std::string	key = _extractRange(start, end, ":");
		std::string	value = _extractRange(start, end, "\n");
		if (key.find_first_of(" ") != std::string::npos)
			throw std::runtime_error("400: Forbidden space in header");
		value = trim(value, " \r");
		std::transform(key.begin(), key.end(), key.begin(), wrap_tolower);
		std::transform(value.begin(), value.end(), value.begin(), wrap_tolower);
		std::transform(key.begin(), key.end(), key.begin(), wrap_iscntrl);
		std::transform(value.begin(), value.end(), value.begin(), wrap_iscntrl);
		headers[key] = value;
		_raw.erase(0, end + 1);
		_size += end + 1;
		if (_size > HD_MAX_LENGTH)
			throw std::runtime_error("431: Request Header Field Too Large");		
	}
	if (_raw.size() > HD_MAX_LENGTH)
		throw std::runtime_error("431: Request Header Field Too Large");
	return (true);
}

/**
 * @brief return true if all body data has been gathered, else flase
 */
bool	Request::_parseBodyByLength(std::string &body)
{
	// if (_bodySizeExpected == 0)
		
	
	body += _raw;
	_raw.clear();

	if (body.size()  > _bodySizeExpected)
		throw std::runtime_error("400: More data than expected");
	if (body.size() == _bodySizeExpected)
	{
		_pstatus = DONE;
		return (true);
	}
	std::cout << "CHONK" << std::endl;

	return (false);
}

/**
 * @brief return true if all body data has benn gathered (0 lengh chunk found), ese false
 */
bool	Request::_parseBodyByChunk(std::string &body)
{
	std::string	tmp;
	size_t		start = 0;
	size_t		end = -1;
	size_t		check = 0;

	while (_raw.find("\r\n") != std::string::npos)
	{
		end = -1;
		if (_lenChunk == ULONG_MAX)
		{
			tmp = _extractRange(start, end, "\n");
			_lenChunk = std::strtoul(tmp.c_str(), NULL, 16);
			if (_lenChunk == 0)
				return (true) ;
			_raw.erase(start, end + 1);
		}
		check = _raw.find("\r\n", 0);
		if (check == std::string::npos)
			return (false);
		if (_raw.at(check - 1) == '\r')
			--check;
		if (check != _lenChunk)
			throw std::runtime_error("400: Malicious chunked data");
		body += _raw.substr(0, _lenChunk);
		_raw.erase(0, _lenChunk + 2);
		_lenChunk = ULONG_MAX;
	}
	return (false);
}


/**
 * @brief return true if request is complete, else false
 * take raw input with minimal size (BUFFER_SIZE)
 * etape: 	1 RL
 * 			2 headers
 * 			3 body
 * RL minimal size : "GET / HTTP/1.1" = 14 char
 *   field-line   = field-name ":" OWS field-value OWS
 * Field values containing CR, LF, or NUL characters are invalid and dangerous
 */
bool	Request::build(std::string	raw)
{
	_raw += raw;
	std::cout << " build" << std::endl;
	switch (_pstatus)
	{
		case RL:
		{
			if (_parseRequestLine())
				return (false);
			__attribute__((fallthrough));
		}
		case HEADERS:
		{
			if (_parseHeaders(_headers))
				return (false);
			__attribute__((fallthrough));
		}
		case BODYCLENGTH:
		{
			if (_pstatus == BODYCLENGTH)
				if (_parseBodyByLength(_body))
					return (true);
			__attribute__((fallthrough));
		}
		case BODYCHUNK:
		{
			
			if (_parseBodyByChunk(_body))
				return (true);
			break;
		}
		case DONE:
		{
			std::cout << "DONE" << std::endl;
			break;
		}
	} 
	return (false);
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

