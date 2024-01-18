/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 18:59:01 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/17 20:03:59 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <iostream>

extern sig_atomic_t	g_status;

void	sig_handler(int sig)
{
	(void)sig;
	std::clog << "\001\033[2K\002Signal caugth!" << std::endl;
	g_status = 0;
}

void	sig_init(void (*handler) (int sig))
{
	struct sigaction	act;

	act.sa_flags = SA_RESTART;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGINT);
	act.sa_handler = handler;
	sigaction(SIGINT, &act, NULL);
}

// SIGABRT