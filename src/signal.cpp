/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 18:59:01 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/13 12:33:56 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include "WebServer.hpp"

extern sig_atomic_t	g_status;

void	sig_handler(int sig)
{
	(void)sig;
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
