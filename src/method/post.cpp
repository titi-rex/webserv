/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 12:37:59 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/16 20:27:06 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "WebServer.hpp"
#include "VirtualHost.hpp"


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
