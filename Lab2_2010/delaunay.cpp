#include "delaunay.h"
#include <math.h>
#include <float.h> // FLT_MAX
#include <assert.h>
#include <GL/glut.h>

#define max(a,b) (((a)>(b))?(a):(b)) 
#define min(a,b) (((a)<(b))?(a):(b)) 

Point::Point()
{
}

Point::Point(float X, float Y):
x(X),y(Y)
{
}

Point operator+(const Point& p0, const Point& p1)
{
	return Point(p0.x+p1.x,p0.y+p1.y);
}

Point operator-(const Point& p0, const Point& p1)
{
	return Point(p0.x-p1.x,p0.y-p1.y);
}

Point operator*(const Point& p0,float f)
{
	return Point(p0.x*f,p0.y*f);
}


Point& Point::operator=(const Point& p)
{
	x = p.x;
	y = p.y;
	return (*this);
}

float DotProduct(const Point& p0, const Point& p1)
{
	return p0.x*p1.x+p0.y*p1.y;
}

bool Circumcircle(const Point& p0,const Point& p1,const Point& p2,Point& center, float& radius)
{
	return false;
}


bool PointInCircle(const Point& p, const Point& center, float r)
{
	return false;
}

/*******************************************************************************/

Delaunay::Delaunay()
{
}

void Delaunay::init(const std::vector<Point>& ipoints)
{
	assert(ipoints.size() >= 3 && "Cannot triangulate less than 3 points");
	this->ipoints = ipoints;
	points.clear();
	count=0;
}


Delaunay::triangle* Delaunay::addTriangle(unsigned int i, unsigned int j, unsigned int k)
{
	triangle* t = new triangle();
	t->i = i;
	t->j = j;
	t->k = k;

	triangles[t] = t;
	return t;
}

void Delaunay::removeTriangle(triangle* t)
{
	triangles.erase(t);
	delete t;
}


void Delaunay::computeSupertriangle(const std::vector<Point>& ipoints)
{
	/*
	auto min_max_X = minmax_element(ipoints.begin(), ipoints.end(),
		[](Point left, Point right) {return left.x < right.x; });
	auto min_max_Y = minmax_element(ipoints.begin(), ipoints.end(),
		[](Point left, Point right) {return left.y < right.y; });

	float minX = min_max_X.first->x;
	float minY = min_max_Y.first->y;

	float maxX = min_max_X.second->x;
	float maxY = min_max_Y.second->y;

	cout << "X: min " << minX << " // max " << maxX << endl;
	cout << "Y: min " << minY << " // max " << maxY << endl;

	*/
	points.clear();
	points.reserve(ipoints.size() + 3);

	float M = -FLT_MAX;
	unsigned int i;
	for (i = 0; i < ipoints.size(); i++) {
		M = max(fabs(ipoints[i].x), M);
		M = max(fabs(ipoints[i].y), M);
	}

	assert(M != FLT_MAX);

	Point p;
	//p_ -1
	p.x = 3 * M; p.y = 0;
	points.push_back(p);

	//p_ -2
	p.x = 0; p.y = 3 * M;
	points.push_back(p);

	//p_ -3
	p.x = -3 * M; p.y = -3 * M;
	points.push_back(p);

	//add the remainder points
	for (i = 0; i < ipoints.size(); i++) {
		points.push_back(ipoints[i]);
	}
		
	addTriangle(0,1,2);
	
	count = 3;
}

bool Delaunay::ready() const
{
	return (ipoints.size() >= 3);
}

void Delaunay::step()
{
	if (count == 0) computeSupertriangle(ipoints);
}


void Delaunay::findTrianglesWhoseCircumcircleContainsPoint(const Point& r, std::vector<triangle*>& containers) const
{	
}


void Delaunay::computeBoundary(const std::vector<triangle*>& region, std::vector<unsigned int>& boundary) const
{
}

void Delaunay::deleteInvalidTriangles(std::vector<triangle*>& triangles)
{
}

void Delaunay::triangulateCavity(const unsigned int r, const std::vector<unsigned int>& boundary)
{
}

void Delaunay::deleteTrianglesWithSupertriangleVertexs()
{
}


// rendering functions (for debugging purposes)

void renderCircle(const Point& center, const float radius)
{
	glBegin(GL_LINE_STRIP);
	float theta;
	float sinf,cosf;
#define SEGMENTS 48
	for (int i=0;i<SEGMENTS;i++)
	{
		theta = ((float)i)/(SEGMENTS-1);
		theta*=2*3.14159597;
		sinf = sin(theta);
		cosf= cos(theta);
		sinf*=radius;
		cosf*=radius;
		glVertex2f(center.x+sinf, center.y+cosf);

	}
	glEnd();
}

void Delaunay::renderCurrentVertexs()
{
	glPointSize(5);
	if (count==0)
	{
		glBegin(GL_POINTS);

		glColor3f(0,0,1);
		for (unsigned int i=0;i<ipoints.size();i++)
		{
			glVertex2f(ipoints[i].x,ipoints[i].y);
		}
		glEnd();
		return;
	}
	
	
	glBegin(GL_POINTS);

	unsigned int i;
	glColor3f(0,0.5,0.5);
	for (i=0;i<count;i++)
	{
		glVertex2f(points[i].x,points[i].y);
	}

	if (count < points.size() )
	{
		glColor3f(1,0,0);
		glVertex2f(points[count].x,points[count].y);

		glColor3f(0,0,1);
		for (i=count+1;i<points.size();i++)
		{
			glVertex2f(points[i].x,points[i].y);
		}
	}
	
	glEnd();

}

void Delaunay::renderCurrentBoundary()
{
}

void Delaunay::renderCurrentTriangles()
{
	glBegin(GL_TRIANGLES);

	Point p;
	std::map<triangle*, triangle*>::const_iterator it;

	float r, g, b;
	unsigned int i, j, k;
	unsigned int cnt = 0;

	for (it = triangles.begin(); it != triangles.end(); it++) {
		r = (float)((cnt >> 2) & 0x00000001);
		g = (float)((cnt >> 1) & 0x00000001);
		b = (float)(cnt & 0x00000001);
		glColor3f(r, g, b);
		cnt++;

		glColor3f(0, 0, 0);

		i = it->second->i;
		j = it->second->j;
		k = it->second->k;
		assert(i<ipoints.size());
		assert(j<ipoints.size());
		assert(k<ipoints.size());

		p.x = points[i].x;
		p.y = points[i].y;
		glVertex2f(p.x, p.y);
		p.x = points[j].x;
		p.y = points[j].y;
		glVertex2f(p.x, p.y);
		p.x = points[k].x;
		p.y = points[k].y;
		glVertex2f(p.x, p.y);
	}

	glEnd();
}

void Delaunay::renderCurrentCircles()
{
}
