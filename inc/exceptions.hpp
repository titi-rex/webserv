/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 12:49:26 by jmoutous          #+#    #+#             */
/*   Updated: 2024/01/10 12:54:40 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef _EXCEPTIONS__
# define _EXCEPTIONS__

# include <stdexcept>
# include "WebServer.hpp"

class locationRedirection : public std::exception {
	private:
		std::string	_redirection;
		locationRedirection();

	public:
		locationRedirection(const std::string & redir);
		const char*	what( void ) const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW;
		~locationRedirection() _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW;
};

class faviconDetected : public std::exception {
	public:
		faviconDetected();
		const char*	what( void ) const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW;
		~faviconDetected() _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW;
};

#endif
