#include "Point.h"

Point::Point()
{
}

Point::Point(float X, float Y) :
	x(X), y(Y)
{
}

Point Point::operator+(const Point& p)
{
	return Point(x + p.x, y + p.y);
}

Point Point::operator-(const Point& p)
{
	return Point(x - p.x, y - p.y);
}

Point Point::operator*(float f)
{
	return Point(x*f, y*f);
}


Point& Point::operator=(const Point& p)
{
	x = p.x;
	y = p.y;
	return (*this);
}
