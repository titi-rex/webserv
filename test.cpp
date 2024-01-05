/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 17:54:34 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/05 17:59:59 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

class foo  
{
	public:
	
		std::string name_;
		unsigned int age_;
		foo(const std::string& name = "", const unsigned int age = 04) : name_(name), age_(age) {};
	};

// Or this?
class foo2 
{
	
	public:
		std::string name_;
		unsigned int age_;
		foo2() : name_(""), age_(0)	{};
		foo2(const std::string& name, const unsigned int age) : name_(name), age_(age) {};
};

void	blabla(int one = 1, char c = 'b')
{
	std::cout << "one is " << one << ", c is " << c << std::endl;
}


int main(void)
{
	blabla(4);
	foo	h("hey");
	std::cout << h.name_ << " " << h.age_ << std::endl;
	return(0);
}