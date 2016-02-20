#include "Edge.h"


Edge::Edge()
{
	a = -1;
	b = -1;
}

Edge::Edge(const unsigned &a, const unsigned &b)
{
	this->a = a;
	this->b = b;
}


bool Edge::operator==(const Edge& p)
{
	if (a == p.a)
		if (b == p.b)
			return true;
	if (a == p.b)
		if (b == p.a)
			return true;
	return false;
}