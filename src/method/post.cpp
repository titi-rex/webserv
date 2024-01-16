/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 12:37:59 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/15 11:27:31 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "WebServer.hpp"
#include "virtual_host.hpp"


// curl -X POST -d "nom=John&email=john@example.com&message=Bonjour" http://example.com/process_form

/*
curl POST http://127.0.0.1:8080/shutdown
curl -X POST -d "nom=John&email=john@example.com" http://127.0.0.1:8080
*/


// int main(int argc, char** argv)
// {
//     execute_cgi("data/cgi/a.out", "ohoh");
//     cgi_handler();
    
//     return (0);
// }
