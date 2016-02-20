#ifndef _DELAUNAY_
#define _DELAUNAY_

#pragma warning (disable: 4786)

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <math.h>
#include "Point.h"
#include "Circle.h"
#include "Edge.h"
#include <list>

using namespace std;

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
	void drawCircle(Point c, float r);
	void renderIndex();
	std::vector<Circle> circles;


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

	bool done;

	// add triangle to the triangulation
	triangle* addTriangle(unsigned int i, unsigned int j, unsigned int k);
	
	// remove triangle from the triangulation
	void removeTriangle(triangle* t);

	// compute a triangle enclosing the set of points
	void computeSupertriangle(const std::vector<Point>& points);
	// compute the boundary of a triangle region
	void computeBoundary(const std::vector<triangle*>& region, std::vector<unsigned int>& boundary);
	// delete the set of triangles of the triangulation
	void deleteInvalidTriangles(std::vector<triangle*>& badTriangles);
	// generate triangles for filling the hole of triangles deleted
	void triangulateCavity(const unsigned int r, const std::vector<unsigned int>& boundary);
	// find triangles whose circumcircle containts input point
	void findTrianglesWhoseCircumcircleContainsPoint(const Point& r, std::vector<triangle*>& containers); 
	// delete triangles with supertriangle vertexs
	void deleteTrianglesWithSupertriangleVertexs();

	void drawString(int x, int y, const char* string);
	

	// index of current point
	unsigned int count;
	// the initial set of points
	std::vector<Point> ipoints;
	// the temporary set of points
	std::vector<Point> points;
	std::map<triangle*, triangle*> triangles;
};

#endif // _DELAUNAY_
