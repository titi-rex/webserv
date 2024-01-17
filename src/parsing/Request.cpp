/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:43:41 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/17 13:43:09 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(void) : _mId(UNKNOW), _pstatus(RL), _size(0), _lenChunk(ULONG_MAX), _bodySizeExpected(0) {};

Request::Request(const Request& src) {*this = src;};

Request&	Request::operator=(const Request& src) 
{
	if (this == &src)
		return (*this);
	_mId = src._mId;
	_uri = src._uri;
	_query = src._query;
	_pathInfo = src._pathInfo;
	_ext = src._ext;
	_body = src._body;
	_headers = src._headers;
	_pstatus = src._pstatus;
	_rline = src._rline;
	_rheaders = src._rheaders;
	_rbody = src._rbody;
	_pathTranslated = src._pathTranslated;
	_bodySizeExpected = src._bodySizeExpected;
	response = src.response;
	rstatus = src.rstatus;
	_rStrStatus = src._rStrStatus;
	return (*this);
};

Request::~Request(void) {};


e_method 			Request::getMid(void) const {return (this->_mId);};
const std::string& 	Request::getUri(void) const { return (this->_uri); };
const std::string&	Request::getBody(void) const { return (this->_body); };
const std::string&	Request::getQuery(void) const { return (this->_query); };
const std::string&	Request::getPathInfo(void) const { return (this->_pathInfo); };
const std::string&	Request::getExt(void) const { return (this->_ext); };
e_parsingStatus		Request::getPstatus(void) const {return (this->_pstatus);};
const std::string&	Request::getRStrStatus(void) const {return (this->_rStrStatus);};
const MapStrStr_t&	Request::getHeaders(void) const { return (this->_headers); };

const std::string	Request::getMethodName(void) const
{
	switch (this->_mId)
	{
		METHOD_ENUM(METHOD_ENUM_CASE)
		default:
			return ("UNKNOW");
	}	
}

void	Request::setPathtranslated( std::string path ) { this->_pathTranslated = path; };
void	Request::setRline( std::string line ) { this->_rline = line; };
void	Request::setRheaders( std::string key, std::string value ) { this->_rheaders[key] = value; };
void	Request::setRstatus( short int status ) { this->rstatus = status; };
void	Request::setRStrStatus( std::string status ) { this->_rStrStatus = status; };
void	Request::setRbody( std::string body ) { this->_rbody = body; };
void	Request::setResponse( std::string response ) { this->response = response; };

void	Request::makeResponse (void)
{
	// std::clog << "\nmakeResponse()" << std::endl;

	MapStrStr_t::iterator	iter;

	this->response = "HTTP/1.1 " + this->_rStrStatus + " " + this->_rline + "\n"; // Place holder for the status description

	for (iter = this->_rheaders.begin(); iter != this->_rheaders.end(); ++iter)
		this->response += iter->first + ": " + iter->second + "\n";

	this->response += "\r\n\r\n";
	this->response += this->_rbody;
	this->response += "\r\n\r\n";

	// std::clog << this->_body << std::endl;
}

bool	Request::isChunked(void) const
{
	return (_pstatus == BODYCHUNK);
};

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

/**
 * @brief return true if we need more data to complete the RL, false else
 */
bool	Request::_parseRequestLine(void)
{
	std::string	tmp;
	size_t		start = 0;
	size_t		end = -1;
	size_t 		i;
	
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
	i = _uri.find_first_of('?');
	if (i != std::string::npos)
	{	
		_query = _uri.substr(i + 1, _uri.size());
		_uri.erase(i, _uri.size());
	}
	i = _uri.find_last_of('/');
	_pathInfo = _uri.substr(i, _uri.size());
	i = _uri.find_last_of('.');
	if (i != std::string::npos)
		_ext = _uri.substr(i + 1, _uri.size());
	
	tmp = _extractRange(start, end, " \n");
	if (tmp.compare("HTTP/1.1") == false)
		throw std::runtime_error("505 Wrong HTTP version");
	_raw.erase(0, end + 1);
	_pstatus = HEADERS;
	return (false);
}

/**
 * @brief find the body size, return true if a content-length has been supplied, false if data is chunked
 */
bool	Request::_findBodySize(void)
{
	if (_mId == HEAD || _mId == GET)
		return (true);

	MapStrStr_t::iterator itCl = _headers.find("content-length");
	MapStrStr_t::iterator itTe = _headers.find("transfer-encoding");

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
 * @brief return true if more data is needed to parse header, else false
 */
bool	Request::_parseHeaders(void)
{
	size_t		start = 0;
	size_t		end = -1;

	while (_raw.find("\r\n") != std::string::npos)
	{	
		if (std::strncmp(_raw.c_str(), "\r\n", 2) == 0)
		{
			_raw.erase(0, 2);
			if (_headers.count("host") == 0)
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
		_headers[key] = value;
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
	body += _raw;
	_raw.clear();

	if (body.size()  > _bodySizeExpected)
		throw std::runtime_error("400: More data than expected");
	if (body.size() == _bodySizeExpected)
	{
		_pstatus = DONE;
		return (true);
	}
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
			if (_lenChunk == ULONG_MAX)
				throw std::runtime_error("400: Chunk length overflow");
			if (_lenChunk == 0)
			{
				_pstatus = DONE;
				return (true) ;
			}
			_raw.erase(start, end + 1);
		}
		check = _raw.find("\r\n", 0);
		if (check == std::string::npos)
			return (false);
		if (_raw.at(check - 1) == '\r')
			--check;
		std::cout << "check " << check << std::endl;
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
 */
bool	Request::build(std::string	raw)
{
	_raw += raw;
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
			if (_parseHeaders())
				return (false);
			__attribute__((fallthrough));
		}
		case BODYCLENGTH:
		{
			if (_pstatus == BODYCLENGTH)
			{	
				if (_parseBodyByLength(_body))
					return (true);
				break ;
			}
			__attribute__((fallthrough));
		}
		case BODYCHUNK:
		{
			if (_parseBodyByChunk(_body))
				return (true);
			break;
		}
		case CGIHD:
		case CGICL:
		case CGIEOF:
		case DONE:
			throw std::runtime_error("698: request parsing loop");
	} 
	return (false);
}


bool	Request::_parseCgiHeaders(void)
{
	size_t		start = 0;
	size_t		end = -1;

	while (_raw.find("\r\n") != std::string::npos)
	{	
		if (std::strncmp(_raw.c_str(), "\r\n", 2) == 0)
		{
			_raw.erase(0, 2);
			if (_rheaders.find("content-length") == _rheaders.end())
				_pstatus = CGIEOF;
			else
			{
				_pstatus = CGICL;
				_bodySizeExpected = std::strtoul(_rheaders["content-length"].c_str(), NULL, 10);
				if (_bodySizeExpected == ULONG_MAX)
					throw std::runtime_error("413: Request Entity Too Large");
			}
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
		_rheaders[key] = value;
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
 * @brief parse CGI response, work like Request::build 
 * 
 */
bool	Request::addCgi(std::string	buff)
{
	_raw += buff;
	switch (_pstatus)
	{
		case CGIHD:
		{
			if (_parseCgiHeaders())
				return (false);
			__attribute__((fallthrough));
		}
		case CGICL:
		{
			if (_pstatus == CGICL)
			{	
				if (_parseBodyByLength(_body))
					return (true);
				break ;
			}
			__attribute__((fallthrough));
		}
		case CGIEOF:
		{
			_body += buff;
			break;
		}
		case RL:
		case HEADERS:
		case BODYCLENGTH:
		case BODYCHUNK:
		case DONE:
			throw std::runtime_error("698: request parsing loop (cgi)");
	}
	return (true);
}

void	Request::clear(void)
{
	_mId = UNKNOW;
	_uri.clear();
	_body.clear();
	_headers.clear();
	_rbody.clear();
	_rheaders.clear();
	rstatus = 0;
}

std::ostream& operator<<(std::ostream& os, const Request& req)
{
	os << "RL: " << req.getMethodName() << " " << req.getUri() << " HTTP/1.1" << std::endl;
	os << "query:" << req.getQuery() << std::endl;
	os << "pathInfo:" << req.getPathInfo() << std::endl;
	os << "extentsion:" << req.getExt() << std::endl;
	os << "Headers :" << std::endl;
	os << req.getHeaders();
	os << "Body :" << std::endl;
	os << "{" << req.getBody() << "}" << std::endl;
	return (os);
};
