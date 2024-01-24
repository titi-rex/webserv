/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:43:41 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/24 16:35:30 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "utils.hpp"

Request::Request(void) : _mId(UNKNOW), _needCgi(false), _parsingStatus(RL),  _bodySizeExpected(0), _size(0), _lenChunk(ULONG_MAX){};

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
	_needCgi = src._needCgi;
	_parsingStatus = src._parsingStatus;
	_raw = src._raw;
	_size = src._size;
	_lenChunk = src._lenChunk;
	_rline = src._rline;
	_rheaders = src._rheaders;
	_rbody = src._rbody;
	_pathTranslated = src._pathTranslated;
	_bodySizeExpected = src._bodySizeExpected;
	response = src.response;
	_rStrStatus = src._rStrStatus;
	return (*this);
};

Request::~Request(void) {};

e_method 			Request::getMid(void) const {return (this->_mId);};
const std::string& 	Request::getUri(void) const { return (this->_uri); };
const std::string&	Request::getQuery(void) const { return (this->_query); };
const std::string&	Request::getPathInfo(void) const { return (this->_pathInfo); };
const std::string&	Request::getExt(void) const { return (this->_ext); };
const std::string&	Request::getBody(void) const { return (this->_body); };
const MapStrStr_t&	Request::getHeaders(void) const { return (this->_headers); };
const std::string&	Request::getPathTranslated(void) const {return (this->_pathTranslated);};
bool				Request::getNeedCgi(void) const {return (this->_needCgi);};

e_parsingStatus		Request::getPstatus(void) const {return (this->_parsingStatus);};
size_t				Request::getBodySizeExpected(void) const {return (this->_bodySizeExpected);};

const std::string&	Request::getRline(void) const {return (this->_rline);};
const std::string&	Request::getRStrStatus(void) const {return (this->_rStrStatus);};
const std::string&	Request::getRbody(void) const { return (this->_rbody); };
const MapStrStr_t&	Request::getRheaders(void) const { return (this->_rheaders); };

const std::string&	Request::getResponse(void) const { return (this->response); };

const std::string	Request::getMethodName(void) const
{
	switch (this->_mId)
	{
		METHOD_ENUM(METHOD_ENUM_CASE)
		default:
			return ("UNKNOW");
	}	
}

std::string Request::getSpecifiedHeader(const std::string& key) const {
	const MapStrStr_t& headers = getHeaders();

	MapStrStr_t::const_iterator it = headers.find(key);

	if (it != headers.end())
		return it->second;
	else
		return "";
}

void	Request::setPathtranslated(const std::string& path ) { this->_pathTranslated = path; };
void	Request::setExt(const std::string& extension ) { this->_ext = extension; };
void	Request::setNeedCgi(const bool yes) { this->_needCgi = yes; };

void	Request::setPstatus(const e_parsingStatus newStatus) {_parsingStatus = newStatus;};

void	Request::setRline(const std::string& line ) { this->_rline = line; };
void	Request::setRbody(const std::string& body ) { this->_rbody = body; };
void	Request::setRheaders(const std::string& key, const std::string& value ) { this->_rheaders[key] = value; };

void	Request::setResponse(const std::string& response ) { this->response = response; };

void	Request::setRStrStatus(const std::string& status, const MapStrStr_t* statusList, const std::string& defaultStatus)
{
	if (statusList == NULL || statusList->count(status))
		this->_rStrStatus = status;
	else
		this->_rStrStatus = defaultStatus;
};

void	Request::findSetType(Request & req, std::string & path, MapStrStr_t	mapContentType)
{
	std::size_t	found = path.rfind('.');
	if (found != 0)
	{
		std::string	extension = path.substr(found + 1, path.length() - found);
		if (mapContentType.count(extension))
		{
			std::string	contentType = mapContentType[extension];
			req.setRheaders("Content-type", contentType);
		}
	}
}


void	Request::makeResponse (void)
{
	MapStrStr_t::iterator	iter;

	this->response = "HTTP/1.1 " + this->_rStrStatus + " " + this->_rline + "\n";

	if (_rheaders.count("transfer-encoding") == 0 && _rheaders.count("content-length") == 0)	
	{
		std::stringstream	sstr;
		sstr << _rbody.size();
		_rheaders["content-length"] = sstr.str();
	}

	char	date[30];
	getDate(date);
	_rheaders["Date"] = date;

	for (iter = this->_rheaders.begin(); iter != this->_rheaders.end(); ++iter)
		this->response += iter->first + ": " + iter->second + "\n";

	this->response += "\r\n";
	this->response += this->_rbody;
}

bool	Request::_is_method_known(std::string& test)
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

std::string	Request::_extractRange(size_t& start, size_t& end, const char* set)
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
		throw std::runtime_error("400: Bad URI");
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
		throw std::runtime_error("505: Wrong HTTP version");
	_raw.erase(0, end + 1);
	_parsingStatus = HEADERS;
	return (false);
}

/**
 * @brief find the body size, return true if a content-length has been supplied, false if data is chunked
 */
bool	Request::_findBodySize(void)
{
	if (_mId == HEAD || _mId == GET || _mId == DELETE)
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
			_findBodySize() ? _parsingStatus = BODYCLENGTH : _parsingStatus = BODYCHUNK;
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
bool	Request::_parseBodyByLength(std::string& body)
{
	body += _raw;
	_raw.clear();

	if (body.size()  > _bodySizeExpected)
		throw std::runtime_error("400: More data than expected");
	if (body.size() == _bodySizeExpected)
	{
		_parsingStatus = DONE;
		return (true);
	}
	return (false);
}

/**
 * @brief return true if all body data has benn gathered (0 lengh chunk found), ese false
 */
bool	Request::_parseBodyByChunk(std::string& body)
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
				_parsingStatus = DONE;
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
bool	Request::build(const std::string& raw)
{
	_raw += raw;
	switch (_parsingStatus)
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
			if (_parsingStatus == BODYCLENGTH)
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

	while (_raw.find("\r\n") != std::string::npos || _raw.find("\n") != std::string::npos)
	{
		if (std::strncmp(_raw.c_str(), "\r\n", 2) == 0 || _raw.find(':') == std::string::npos)
		{
			_raw.erase(0, 2);
			if (_rheaders.find("content-length") == _rheaders.end())
				_parsingStatus = CGIEOF;
			else
			{
				_parsingStatus = CGICL;
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
 */
bool	Request::addCgi(const std::string& buff)
{
	_raw += buff;
	switch (_parsingStatus)
	{
		case CGIHD:
		{
			if (_parseCgiHeaders())
				return (false);
			__attribute__((fallthrough));
		}
		case CGICL:
		{
			if (_parsingStatus == CGICL)
			{
				if (_parseBodyByLength(_rbody))
					return (true);
				break ;
			}
			__attribute__((fallthrough));
		}
		case CGIEOF:
		{
			_rbody += buff;
			break;
		}
		case RL:
		case HEADERS:
		case BODYCLENGTH:
		case BODYCHUNK:
		case DONE:
			throw std::runtime_error("698: request parsing loop (cgi)");
	}
	return (false);
}

void	Request::clear(void)
{
	_mId = UNKNOW;
	_uri.clear();
	_query.clear();
	_pathInfo.clear();
	_ext.clear();
	_body.clear();
	_headers.clear();
	_pathTranslated.clear();
	_parsingStatus = RL;
	_bodySizeExpected = 0;
	_raw.clear();
	_size = 0;
	_lenChunk = 0;
	_rline.clear();
	_rStrStatus.clear();
	_rbody.clear();
	_rheaders.clear();
	response.clear();
}

std::ostream& operator<<(std::ostream& os, const Request& req)
{
	os << "===REQUEST SIDE===" << std::endl;
	os << "RL: " << req.getMethodName() << " " << req.getUri() << " HTTP/1.1" << std::endl;
	os << "query:" << req.getQuery() << std::endl;
	os << "pathInfo:" << req.getPathInfo() << std::endl;
	os << "extentsion:" << req.getExt() << std::endl;
	os << "Headers: " ;
	os << req.getHeaders();
	os << "Body: " ;
	os << "{" << req.getBody() << "}" << std::endl;
	os << "===RESPONSE SIDE===" << std::endl;
	os << "_rStrStatus: " << req.getRStrStatus() << std::endl;
	os << "_rline: " << req.getRline() << std::endl;
	os << "_rheaders: " << req.getRheaders() << std::endl;
	os << "_rbody: " << "{" << req.getRbody() << "}" << std::endl;
	// os << "response: " << req.response << std::endl;
	
	return (os);
};