#include "delaunay.h"
#include <math.h>
#include <float.h> // FLT_MAX
#include <assert.h>
#include <GL/glut.h>

#define max(a,b) (((a)>(b))?(a):(b)) 
#define min(a,b) (((a)<(b))?(a):(b)) 


float DotProduct(const Point& p0, const Point& p1)
{
	return p0.x*p1.x+p0.y*p1.y;
}

bool Circumcircle(const Point& p0,const Point& p1,const Point& p2,Point& center, float& radius)
{
	float dA, dB, dC, aux1, aux2, div;

	dA = p0.x * p0.x + p0.y * p0.y;
	dB = p1.x * p1.x + p1.y * p1.y;
	dC = p2.x * p2.x + p2.y * p2.y;

	aux1 = (dA*(p2.y - p1.y) + dB*(p0.y - p2.y) + dC*(p1.y - p0.y));
	aux2 = -(dA*(p2.x - p1.x) + dB*(p0.x - p2.x) + dC*(p1.x - p0.x));
	div = (2 * (p0.x*(p2.y - p1.y) + p1.x*(p0.y - p2.y) + p2.x*(p1.y - p0.y)));

	if (div == 0) {
		return false;
	}

	center.x = aux1 / div;
	center.y = aux2 / div;

	radius = sqrt((center.x - p0.x)*(center.x - p0.x) + (center.y - p0.y)*(center.y - p0.y));

	return true;
}


bool PointInCircle(const Point& p, const Point& center, float r)
{
	float distance;

	distance = sqrt((double)(center.x - p.x)*(center.x - p.x) + (center.y - p.y)*(center.y - p.y));

	if (distance <= r)
		return true;
	else
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
	triangles.clear();
	circles.clear();
	count=0;
	done = false;
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
	points.clear();
	points.reserve(ipoints.size() + 3);

	float M = -FLT_MAX;
	unsigned int i;
	for (i = 0; i < ipoints.size(); i++) 
	{
		M = max(fabs(ipoints[i].x), M);
		M = max(fabs(ipoints[i].y), M);
	}

	assert(M != FLT_MAX);

	Point p;
	float scale = 3.0;

	// Point 1
	p.x = scale * M; 
	p.y = 0;
	points.push_back(p);

	// Point 2
	p.x = 0; 
	p.y = scale * M;
	points.push_back(p);

	// Point 3
	p.x = -1 * scale * M; 
	p.y = -1 * scale * M;
	points.push_back(p);

	// Add the remainder points
	for (i = 0; i < ipoints.size(); i++) 
	{
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
	if (!done)
	{
		if (count == 0)
			computeSupertriangle(ipoints);
		else
		{
			if (count < ipoints.size() + 3)
			{
				// Pasos intermedios

				cout << "======================================================================" << endl << endl;
				std::vector<triangle *> containers;
				std::vector<unsigned int> boundary;
				findTrianglesWhoseCircumcircleContainsPoint(points[count], containers);
				computeBoundary(containers, boundary);
				deleteInvalidTriangles(containers);
				triangulateCavity(count, boundary);
				count++;

				containers.clear();
				boundary.clear();
			}
			else if (count == ipoints.size() + 3)
			{
				deleteTrianglesWithSupertriangleVertexs();
				done = true;
			}
		}
	}
}

void Delaunay::findTrianglesWhoseCircumcircleContainsPoint(const Point& r, std::vector<triangle*>& containers)
{
	std::map<triangle*, triangle*>::const_iterator it;

	for (it = triangles.begin(); it != triangles.end(); it++)
	{
		Point center;
		float radious;
		if (Circumcircle(points[it->second->i], points[it->second->j], points[it->second->k], center, radious))
		{
			Circle circle(center, radious);
			circles.push_back(circle);

			if (PointInCircle(r, circle.c, circle.r))
			{
				containers.push_back(it->second);
			}
		}
	}
}

void Delaunay::computeBoundary(const std::vector<triangle*>& region, std::vector<unsigned int>& boundary)
{
	bool isShared = false;
	vector<Edge> sharedEdges;
	cout << endl;
	cout << "REGIONS: " << endl;
	for (unsigned i = 0; i < region.size(); i++)
	{
		cout << region[i]->i << " , " << region[i]->j << " , " << region[i]->k << endl;
	}
	cout << endl;
	if (region.size() == 1)
	{
		for (int p = 0; p < 3; p++)
		{
			int next = p + 1;
			if (next == 3)
				next = 0;
			Point edge = Point(region[0]->v[p], region[0]->v[next]);
			boundary.push_back(edge.x);
			boundary.push_back(edge.y);
		}
	}
	else
	{
		for (unsigned i = 0; i < region.size() - 1; i++)
		{
			for (int p1 = 0; p1 < 3; p1++)
			{
				int next1 = p1 + 1;

				if (next1 == 3)
					next1 = 0;
				isShared = false;
				Edge edge1 = Edge(region[i]->v[p1], region[i]->v[next1]);
				cout << endl;
				cout << "COMPARING :" << endl << endl;;
				cout << "EDGE 1: " << edge1.a << " , " << edge1.b << endl << endl;;
				cout << "WITH: " << endl << endl;;
				for (unsigned j = i + 1; j < region.size(); j++)
				{
					for (int p2 = 0; p2 < 3; p2++)
					{
						int next2 = p2 + 1;
						if (next2 == 3)
							next2 = 0;
						Edge edge2 = Edge(region[j]->v[p2], region[j]->v[next2]);
						cout << "EDGE 2: " << edge2.a << " , " << edge2.b << endl << endl;;
						if (edge1 == edge2)
						{
							isShared = true;
							sharedEdges.push_back(edge1);
							cout << "SHARED WITH ";
							cout << "EDGE 2: " << edge2.a << " , " << edge2.b << endl << endl;;
						}
					}
				}
				if (!isShared)
				{
					bool inside = false;
					for (unsigned i = 0; i < sharedEdges.size(); i++)
					{
						if (sharedEdges[i] == edge1)
							inside = true;
					}
					if (!inside)
					{
						boundary.push_back(edge1.a);
						boundary.push_back(edge1.b);
					}
				}
			}
		}
		int lastTriangle = region.size() - 1;
		// To include edges of the las triangle
		for (int p = 0; p < 3; p++)
		{
			bool inside = false;
			int next = p + 1;
			if (next == 3)
				next = 0;
			Edge edge = Edge(region[lastTriangle]->v[p], region[lastTriangle]->v[next]);
			for (unsigned i = 0; i < sharedEdges.size(); i++)
			{
				if (sharedEdges[i] == edge)
					inside = true;
			}
			if (!inside)
			{
				boundary.push_back(edge.a);
				boundary.push_back(edge.b);
			}
		}
	}
	
}

void Delaunay::deleteInvalidTriangles(std::vector<triangle*>& badTriangles)
{
	std::map<triangle*, triangle*>::const_iterator it;

	cout << endl;
	cout << "TRIANGLES BEFORE: " << endl;
	for (it = triangles.begin(); it != triangles.end(); it++)
	{
		cout << it->second->i << " , " << it->second->j << " , " << it->second->k << endl;
	}

	for (unsigned i = 0; i < badTriangles.size(); i++)
	{
		triangles.erase(badTriangles[i]);
	}

	cout << endl;
	cout << "TRIANGLES AFTER: " << endl;
	for (it = triangles.begin(); it != triangles.end(); it++)
	{
		cout << it->second->i << " , " << it->second->j << " , " << it->second->k << endl;
	}
}

void Delaunay::triangulateCavity(const unsigned int r, const std::vector<unsigned int>& boundary)
{
	cout << endl << "WITH NEW VERTEX : " << r << endl << endl;
	cout << "TRIANGLES CREATED: " << endl << endl;
	for (unsigned i = 0; i < boundary.size(); i = i + 2)
	{
		cout << r << " , " << boundary[i] << " , " << boundary[i + 1] << endl;
		addTriangle(r, boundary[i], boundary[i + 1]);
	}
}

void Delaunay::deleteTrianglesWithSupertriangleVertexs()
{
	std::map<triangle*, triangle*>::const_iterator it;

	bool found = false;

	it = triangles.begin();
	while (it != triangles.end())
	{
		found = false;
		for (int i = 0; i < 3; i++)
		{
			if (it->second->v[i] == 0 || it->second->v[i] == 1 || it->second->v[i] == 2)
			{
				triangles.erase(it++);
				found = true;
				break;
			}
		}
		if (!found)
			it++;
	}
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
		assert(i<points.size());
		assert(j<points.size());
		assert(k<points.size());

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
	for (unsigned i = 0; i < circles.size(); i++)
		drawCircle(circles[i].c, circles[i].r);
}

void Delaunay::drawCircle(Point c, float r)
{
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 1000; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(1000);//get the current angle

		float x = r * cosf(theta);//calculate the x component
		float y = r * sinf(theta);//calculate the y component

		glVertex2f(x + c.x, y + c.y);//output vertex

	}
	glEnd();
}


void Delaunay::drawString(int x, int y, const char* string)
{
	int i, len;
	glRasterPos2f(x, y);
	glColor3f(1.0f, 0.0f, 0.0f);
	len = strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, string[i]);
	}
}

void Delaunay::renderIndex()
{
	char buffer[4];

	for (unsigned i = 0; i < points.size(); i++)
	{
		_itoa_s(i, buffer, 10);
		drawString(points[i].x + 1, points[i].y, buffer);
	}
}