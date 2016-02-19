#ifndef _DELAUNAY_
#define _DELAUNAY_

#pragma warning (disable: 4786)

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;


class Point
{
public:
	Point();
	Point(float x, float y);

	union
	{
		struct {float x, y;};
		struct{ float v[2]; };
	};

	friend Point operator+(const Point& p0, const Point& p1);
	friend Point operator-(const Point& p0, const Point& p1);
	friend Point operator*(const Point& p0, float f);
	Point& operator=(const Point& p);
};

class Circle
{
public:
	Circle();
	Circle(Point c, float r);

	Point c;
	float r;
	Circle &operator=(const Circle &c);
};

class Delaunay
{
public:
	Delaunay();

	bool ready() const;
	void init(const std::vector<Point>& ipoints);
	void step();

// rendering functions
	void renderCurrentTriangles();
	void renderCurrentCircles();
	void renderCurrentVertexs();
	void renderCurrentBoundary();

protected:

	class triangle
	{
		public:
		union
		{
			struct { unsigned int i,j,k; };
			struct { unsigned int  v[3]; };
		};

	};

	// add triangle to the triangulation
	triangle* addTriangle(unsigned int i, unsigned int j, unsigned int k);
	
	// remove triangle from the triangulation
	void removeTriangle(triangle* t);

	// compute a triangle enclosing the set of points
	void computeSupertriangle(const std::vector<Point>& points);
	// compute the boundary of a triangle region
	void computeBoundary(const std::vector<triangle*>& region, std::vector<unsigned int>& boundary) const;
	// delete the set of triangles of the triangulation
	void deleteInvalidTriangles(std::vector<triangle*>& triangles);
	// generate triangles for filling the hole of triangles deleted
	void triangulateCavity(const unsigned int r, const std::vector<unsigned int>& boundary);
	// find triangles whose circumcircle containts input point
	void findTrianglesWhoseCircumcircleContainsPoint(const Point& r, std::vector<triangle*>& containers) const;
	// delete triangles with supertriangle vertexs
	void deleteTrianglesWithSupertriangleVertexs();

	// index of current point
	unsigned int count;
	// the initial set of points
	std::vector<Point> ipoints;
	// the temporary set of points
	std::vector<Point> points;

	std::map<triangle*, triangle*> triangles;
};

#endif // _DELAUNAY_
