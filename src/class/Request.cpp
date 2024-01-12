/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:43:41 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/12 20:03:16 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

size_t	Request::_num_request = 0;


Request::Request(void) : _rId(_num_request++), _mId(eUNKNOW), _pstatus(RLWAIT), _bodyCount(0), _bodySizeExpected(0) {};

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


std::string	Request::_extractRange(size_t& start, size_t& end, const char *set)
{
	start = end + 1;
	end = _raw.find_first_of(set, start);	
	return (_raw.substr(start, end - start));
}


size_t	Request::_findBodySize(void)
{
	std::map<std::string, std::string>::iterator itCl = _headers.find("content-lenght");
	std::map<std::string, std::string>::iterator itTe = _headers.find("transfert-encoding");
	if (itCl == _headers.end() && itTe == _headers.end())
		throw std::runtime_error("411: Length Required");
	else if (itCl != _headers.end() && itTe != _headers.end())
		throw std::runtime_error("400: Confusing lenght indication");
	
	if (itCl != _headers.end())
	{
		size_t	size = std::strtoul(itCl->second.c_str(), NULL, 10);
		if (size == ULONG_MAX)
			throw std::runtime_error("413: Request Entity Too Large");
		return (size);
	}
	if (itTe->second != "chunked")
		throw std::runtime_error("500: Unknow encoding");
	return (ULONG_MAX);
}


/**
 * @brief 
 * take raw input with minimal size (BUFFER_SIZE)
 * etape: 	1 RL
 * 			2 headers
 * 			3 body
 * RL minimal size : "GET / HTTP/1.1" = 14 char
 *   field-line   = field-name ":" OWS field-value OWS
 * Field values containing CR, LF, or NUL characters are invalid and dangerous
 */
bool	Request::build2(std::string	raw)
{
	std::cout << "_raw :" << _raw << ":" << std::endl;
	
	_raw += raw;
	size_t	start = 0;
	size_t	end = -1;
	std::string	tmp;
	
	switch (_pstatus)
	{
		case RLWAIT:
		{
			std::cout << "RLWAIT" << std::endl;

			if (_raw.find("\r\n") == std::string::npos)
			{
				std::clog << "waiting for rl end" << std::endl;
				if (_raw.size() > RL_MAX_LENGHT)
					throw std::runtime_error("414: URI too long");
				return (false);
			}
			if (_raw.size() < RL_MIN_LENGHT)
				throw std::runtime_error("400: RL too short");
			_pstatus = RL;
		}
		case RL:
		{
			std::cout << "RL" << std::endl;
			
			// check method
			tmp = _extractRange(start, end, " ");
			if (_is_method_known(tmp) == false)
				throw std::runtime_error("501: Method not implemented");

			//get URI
			_uri = _extractRange(start, end, " ");
			if (_uri.at(0) != '/')
				throw std::runtime_error("400 Bad URI");

			//get http version
			tmp = _extractRange(start, end, " \n");
			if (tmp.compare("HTTP/1.1") == false)
				throw std::runtime_error("505 Wrong HTTP version");

			_raw.erase(0, end + 1);
			_pstatus = HEADERS;
			break;
		}
		case HEADERS:
		{
			std::cout << "HEADERS" << std::endl;


			while (_raw.find("\r\n") != std::string::npos)
			{	
				if (_raw[0] == '\r' && _raw[1] == '\n') // use strncmp 
				{
					// end of header function : search for content-lenght or encrypt: chunked to read body or not
					_raw.erase(0, 2);
					if (_headers.count("host") == 0)
						throw std::runtime_error("400: No Host Header");
					_bodySizeExpected = _findBodySize();
					std::cout << "expect " << _bodySizeExpected << " char" << std::endl;
					if (_bodySizeExpected != ULONG_MAX)
						_pstatus = BODY_CLENGHT;
					else
						_pstatus = BODY_CHUNK;
					return (false);
				}
				
				end = -1;
				std::string	key = _extractRange(start, end, ":");
				std::string	value = _extractRange(start, end, "\n");

				// check for space in key (forbidden)
				if (key.find_first_of(" ") != std::string::npos)
					throw std::runtime_error("400 Bad URI");

				// uncase headers and trim value
				std::transform(key.begin(), key.end(), key.begin(), wrap_tolower);
				std::transform(value.begin(), value.end(), value.begin(), wrap_tolower);
				value = trim(value, " \r");

				//look for ctl char (forbidden)
				std::transform(key.begin(), key.end(), key.begin(), wrap_iscntrl);
				std::transform(value.begin(), value.end(), value.begin(), wrap_iscntrl);

				// add to header map
				_headers[key] = value;
				std::cout << "hget:" << key << "|" << _headers[key] << std::endl;
				
				_raw.erase(0, end + 1);
			}

			std::clog << "waiting for field-line end" << std::endl;
			if (_raw.size() > HD_MAX_LENGHT)
				throw std::runtime_error("414: URI too long");
			return (false);
			
			break;
		}
		case BODY_CLENGHT:
		{
			std::cout << "BODY CLENGHT" << std::endl;

			_body += _raw;
			_bodyCount = _body.size();
			_raw.clear();
		
			// check if malicious user send more data than announced
			if (_body.size()  > _bodySizeExpected)
			{
				std::cout << "size:" << _body.size() << std::endl;
				throw std::runtime_error("400: More data than expected");
			}
			// check if all data has been receive
			if (_body.size() == _bodySizeExpected)
				return (true);

			break;
		}
		case BODY_CHUNK:
		{
			std::cout << "BODY CHUNK" << std::endl;
			
			while (_raw.find("\r\n") != std::string::npos)
			{
				end = -1;
				tmp = _extractRange(start, end, "\n");
				size_t	len_chunk = std::strtoul(tmp.c_str(), NULL, 10);
				if (len_chunk == 0) // 0 size chunk mean end of data (or malicious message) 
					return (true) ;
				
				
				
					
				std::cout << "chunk lenght:" << len_chunk << std::endl;
				std::cout << "start:" << start << ",end:" << end << std::endl;
				
				_raw.erase(start, end + 1);
				std::cout << "raw is :" << _raw << ":" << std::endl;
				
				_body += _raw.substr(start, len_chunk);
				
				std::cout << "body is :" << _body << ":" << std::endl;
				_raw.erase(start, len_chunk + 2);
				std::cout << "raw is :" << _raw << ":" << std::endl;

			}

			
			break;
		}
		default:
		{
			std::cout << "default" << std::endl;
			break;
		}
	} 
	return (true);
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
	// if (buff.find("GET") == std::string::npos)
	// 	throw std::runtime_error("400: only accept get");
	
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

