/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 12:50:56 by jmoutous          #+#    #+#             */
/*   Updated: 2024/01/10 12:51:34 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "exceptions.hpp"

locationRedirection::locationRedirection() {};
locationRedirection::locationRedirection(const std::string & redir) : _redirection(redir) {};
locationRedirection::~locationRedirection() _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {};

const char*	locationRedirection::what( void ) const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW
{
	return (this->_redirection.c_str());
};

faviconDetected::faviconDetected() {};
faviconDetected::~faviconDetected() _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {};
const char*	faviconDetected::what( void ) const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW
{
	return ("Favicon detected");
};
