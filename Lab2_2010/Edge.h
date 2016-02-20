#pragma once

class Edge
{
public:	
	Edge();
	Edge(const unsigned &a, const unsigned &b);
	bool operator==(const Edge& p);
	unsigned int a;
	unsigned int b;

};