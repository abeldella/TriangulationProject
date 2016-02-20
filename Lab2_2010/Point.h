#pragma once

class Point
{
public:
	Point();
	Point(float x, float y);

	union
	{
		struct { float x, y; };
		struct { float v[2]; };
	};

	Point operator+(const Point& p);
	Point operator-(const Point& p);
	Point operator*(float f);
	Point& operator=(const Point& p);
	
};