/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 14:54:09 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/04 15:48:06 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
using namespace std;

//  key and value of enum
#define FRUIT_ENUM(VARIANT) \
	VARIANT(Apple, 0) \
	VARIANT(Banana, 1) \
	VARIANT(Orange, 3)	\
	VARIANT(WaterMelon, 4)

// format key and value to match enum syntaxe
#define FRUIT_ENUM_VARIANT(NAME, VALUE) NAME = (VALUE),

// definition of enum
typedef enum {
  FRUIT_ENUM(FRUIT_ENUM_VARIANT)
} Fruit;

// format tp match switch syntaxe
#define FRUIT_ENUM_STRING(NAME, ...) case NAME: return #NAME;

// add ONE
#define PLUS_ONE(...) + 1
// COUNT NUMBER OF ELEMENT IN ENUM, can be really long if enum long + force to calcul at runtime
#define FRUIT_COUNT (0 FRUIT_ENUM(PLUS_ONE))
// so we store it u=inside another enum
enum {
	FRUIT_COUNT2 = (0 FRUIT_ENUM(PLUS_ONE))
};


#define NAME(a) #a


const char *fruitname(Fruit fr)
{
	switch (fr)
	{
		FRUIT_ENUM(FRUIT_ENUM_STRING)
	default:
		return ("UNKNOW");
	}
}

int main(void)
{
	Fruit	fr = Apple;
	cout << "hello " << endl;

	
	cout <<  fruitname(fr) << endl;
	cout << "enum size " << FRUIT_COUNT << endl;
	cout << "enum size " << FRUIT_COUNT2 << endl;
	cout << "bye " << endl;
	return (0);
}

// use c++ -E enum_test.cpp
