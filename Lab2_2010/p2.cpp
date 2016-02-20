#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include <GL/gl.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


/* Windows parameters */
#define TITLE  "Geometria computacional: Pràctica 2, Triangulació de Delaunay"

unsigned int width = 500;
unsigned int height = 500;
float extents = 50.0f;

#include "Delaunay.h"

// delaunay variables
std::vector<Point> points;
Delaunay delaunay;

// interface variables
bool renderAllCircles = false;
bool renderCurrentCircles = false;
bool renderCurrentBoundary = false;
int shape = -1;


void 
drawString (char *s)
{
  unsigned int i;
  for (i = 0; i < strlen (s); i++)
    glutBitmapCharacter (GLUT_BITMAP_HELVETICA_10, s[i]);
};

// initialize delaunay triangulator
void init()
{

	glPolygonMode(GL_FRONT,GL_LINE);
	glDisable(GL_CULL_FACE);
	int i,j,k;
// some predefined shapes

	if (shape != -1)
		points.clear();

	if (shape==0)
	{
		points.push_back(Point(-5,-5));
		points.push_back(Point(5,-5));
		points.push_back(Point(0,5));
	}

	if (shape==1)
	{
		points.push_back(Point(-5,-5));
		points.push_back(Point(5,-5));
		points.push_back(Point(0,5));
		points.push_back(Point(10,4));
		points.push_back(Point(-10,8));
		points.push_back(Point(8,8));
		points.push_back(Point(15,15));
	}

	if (shape==2)
	{

		for (i=0;i<5;i++)
			for (j=0;j<5;j++)
			{
				points.push_back(Point(-10+i*6,-10+j*6));
			}
	}

	if (shape == 3)
	{

		float sinf,cosf,theta;
		for (i=0;i<10;i++)
		{
			theta = i/((float)(10-1));
			theta*=2*3.14159597;
			sinf = sin(theta);
			cosf = cos(theta);

			points.push_back(Point(sinf*15,cosf*15));
		}
	}

	if (points.size() > 0)
	{
		srand(GetTickCount());
		for (i=0;i<50;i++)
		{
			j = rand()%points.size();
			k = rand()%points.size();
			std::swap(points[j],points[k]);
		}
	}

	if (points.size()>=3)
		delaunay.init(points);

}

void triangulate()
{
	if (delaunay.ready())
		delaunay.step();
	else if (points.size() >= 3)
	{
		init();
		delaunay.step();
	}
}

void 
idle (void)
{

//  glutPostRedisplay ();
}

void 
display(void)
{
	unsigned int i;

  /* Clean drawing board */
	glClearColor(1,1,1,1);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();

	glPointSize(5);
	glBegin(GL_POINTS);
	for (i=0;i<points.size();i++)
	{
		glVertex2fv((float*)&points[i]);
	}
	glEnd();
	
	delaunay.renderCurrentTriangles();
	delaunay.renderCurrentVertexs();
	delaunay.renderIndex();

	if (renderCurrentCircles)
		delaunay.renderCurrentCircles();

	if (renderCurrentBoundary)
		delaunay.renderCurrentBoundary();

  glutSwapBuffers ();
	
  glutPostRedisplay ();
};



/* Callback function for reshaping the main window */
void 
reshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-extents, extents, -extents, extents);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
};

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN )
			points.push_back(Point(-extents + x*(extents * 2 / width),
					extents + y*(-extents * 2 / height)));
	}
}

/* Now comes a function that processes keyboard events */
void 
keyboard (unsigned char key, int x, int y)
{
  switch (key)
   {
		case 'q':
    case 'Q':
			exit(0);
			break;

		case 'r':
    case 'R':
      init ();
      break;

		case 's':
    case 'S':
		{
      shape = (shape+1)%4;
			init();
      break;
		}

		case 'c':
			renderCurrentCircles = !renderCurrentCircles;
			break;

		case 'b':
		case 'B':
			renderCurrentBoundary = !renderCurrentBoundary;
			break;

		case 't':
		case 'T':
			triangulate();
			break;
    };
};


int 
main (int argc, char **argv)
{

  /* Glut initializations */
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowPosition (5, 5);
  glutInitWindowSize (width, height);

  /* Main window creation and setup */
  glutCreateWindow (TITLE);
  glutDisplayFunc (display);
  glutReshapeFunc (reshape);
  glutKeyboardFunc (keyboard);
  glutMouseFunc(mouse);
  glutIdleFunc (idle);


  glutMainLoop ();

  return 0;
};
