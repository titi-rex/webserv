/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 15:12:45 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/24 15:35:40 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _LOG_H__
# define _LOG_H__
# include <iostream>
# include <iomanip>

typedef enum {
	logDEBUG,
	logINFO,
	logWARNING,
	logERROR,
}	e_logLevel;

#define FDEBUG "\001\033[32m\002"
#define FINFO  "\001\033[0;37m\002"
#define FWARNING "\001\033[0;33m\002"
#define FERROR "\001\033[0;31m\002"
#define FERASE "\001\033[2K\002"
#define END "\001\033[0m\002"

#define LOGLEVEL 1

template <typename T>
e_logLevel	operator<<(e_logLevel lvl, T log)
{
	switch (lvl)
	{
		case logDEBUG:
		{
			if (logDEBUG >= LOGLEVEL)
				std::clog << FDEBUG << std::setw(12) << "[DEBUG] : " << log << END << std::endl;
			break;
		}
		case logINFO:
		{
			if (logINFO >= LOGLEVEL)
				std::clog << FINFO << std::setw(12) << " ---------- " << log << END << std::endl;
			break;
		}
		case logWARNING:
		{
			if (logWARNING >= LOGLEVEL)
				std::clog << FERASE << FWARNING << std::setw(12) << "[WARNING] : " << log << END << std::endl;
			break;
		}
		case logERROR:
		{
			if (logERROR >= LOGLEVEL)
				std::clog << FERROR << std::setw(12) << "[ERROR] : " << log << END << std::endl;
			break;
		}
	}
	return (lvl);
}

#endif
