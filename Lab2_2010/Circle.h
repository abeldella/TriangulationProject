#pragma once
#include "Point.h"

class Circle
{
public:
	Circle();
	Circle(Point c, float r);

	Point c;
	float r;
	Circle &operator=(const Circle &c);
};