#include "Circle.h"

Circle::Circle()
{

}

Circle::Circle(Point c, float r)
{
	this->c = c;
	this->r = r;
}

Circle &Circle::operator=(const Circle &circle)
{
	c = circle.c;
	r = circle.r;
	return (*this);
}