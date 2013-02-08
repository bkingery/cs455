
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <GL/glut.h>   		// The GL Utility Toolkit (Glut) Header
#include <cml/cml.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include <assert.h>
using namespace std;

// Globals
#define HEIGHT 480
#define WIDTH 640

// typedefs
typedef cml::vector4f Color;
typedef cml::vector2i Point;
typedef vector<Point> Line;

bool drawMine;
int drawMode;
int lineWidth;
GLenum pointMode;
Point firstPoint;

// Data Structures
float raster[WIDTH*HEIGHT*3];
Color curColor;
Color clearColor;
std::vector<Point> savedPoints;

bool pointCompareY(Point p1, Point p2) {return (p1[1] < p2[1]);}

void init ()
{
  glShadeModel(GL_SMOOTH);						// Enable Smooth Shading
  glClearDepth(1.0f);							// Depth Buffer Setup
  glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
  glDepthFunc(GL_LEQUAL);						// The Type Of Depth Testing To Do
  glEnable ( GL_COLOR_MATERIAL );
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
  drawMine = false;
  drawMode = 0;
  pointMode = -1;
  firstPoint = Point(-1,-1);
  
  curColor = Color(1,1,1,1);
  clearColor = Color(1,1,1,1);
}

/**
 * Set pixle color r,g,b in raster for x,y
 */
void setPixel(int x, int y, float r, float g, float b)
{
  raster[((y*WIDTH) + x)*3 + 0] = r;
  raster[((y*WIDTH) + x)*3 + 1] = g;
  raster[((y*WIDTH) + x)*3 + 2] = b;
}

Color getPixelColor(int x, int y)
{
  Color c = Color(0,0,0,1);
  c[0] = raster[((y*WIDTH) + x)*3 + 0];
  c[1] = raster[((y*WIDTH) + x)*3 + 1];
  c[2] = raster[((y*WIDTH) + x)*3 + 2];
  
  return c;
}

/**
 * Sets the clear color.
 */
void bk_glClearColor(float r, float g, float b, float a)
{
  glClearColor(r,g,b,a);  // Forward to OpenGL
  clearColor.set(r,g,b,a);
}

/**
 * Clears the entire screen to the clear color.
 */
void bk_glClear(GLint bit)
{
  glClear(bit);
  
  //TODO what is this bit?!?!?!
  for (int x=0; x<WIDTH; x++)
	for (int y=0; y<HEIGHT; y++)
	  setPixel(x,y, clearColor[0], clearColor[1], clearColor[2]);
	  //setPixel(x,y, 1, 1, 1);
  return;
}

/**
 * Saves n points for drawing 
 * return true when n or more points are saved.
 */
bool savePoints(int x, int y, int n)
{
  savedPoints.push_back(Point(x,y));
  if (savedPoints.size() >= n)
	return true;
  else
	return false;
}

/**
 * Clears the saved points
 */
void clearSavedPoints()
{
  savedPoints.clear();
  assert (savedPoints.size() == 0);
}

/**
 * This tells you how to interpret points.
 * with parameters GL_POINTS, GL_LINES, and GL_TRIANGLES
 */
void bk_glBegin(GLenum mode)
{
  glBegin(mode);
  pointMode = mode;
  firstPoint.set(-1, -1);
  clearSavedPoints();
}

/**
 *	Returns the distance between point1 and point2
 */
float pointDistance(int x1, int y1, int x2, int y2)
{
  return sqrt(pow((x2-x1),2) + pow((y2-y1), 2));
}

/**
 * Returns the color interpolation based on 2 colors and a fraction
 */
Color colorInterpolation(Color COLOR1, Color COLOR2, float fraction)
{
  float INT_TO_FLOAT_CONST = 1.0f / 255.0f;
  fraction = min(fraction, 1.0f);
  fraction = max(fraction, 0.0f);
  
  float RED1 = COLOR1[0] * INT_TO_FLOAT_CONST;
  float GREEN1 = COLOR1[1] * INT_TO_FLOAT_CONST;
  float BLUE1 = COLOR1[2] * INT_TO_FLOAT_CONST;
  float ALPHA1 = COLOR1[3] * INT_TO_FLOAT_CONST;

  float RED2 = COLOR2[0] * INT_TO_FLOAT_CONST;
  float GREEN2 = COLOR2[1] * INT_TO_FLOAT_CONST;
  float BLUE2 = COLOR2[2] * INT_TO_FLOAT_CONST;
  float ALPHA2 = COLOR2[3] * INT_TO_FLOAT_CONST;

  float DELTA_RED = RED2 - RED1;
  float DELTA_GREEN = GREEN2 - GREEN1;
  float DELTA_BLUE = BLUE2 - BLUE1;
  float DELTA_ALPHA = ALPHA2 - ALPHA1;

  float red = RED1 + (DELTA_RED * fraction);
  float green = GREEN1 + (DELTA_GREEN * fraction);
  float blue = BLUE1 + (DELTA_BLUE * fraction);
  float alpha = ALPHA1 + (DELTA_ALPHA * fraction);

  red = min(red, 1.0f);
  red = max(red, 0.0f);
  green = min(green, 1.0f);
  green = max(green, 0.0f);
  blue = min(blue, 1.0f);
  blue = max(blue, 0.0f);
  alpha = min(alpha, 1.0f);
  alpha = max(alpha, 0.0f);

  return Color(red*255.0f, green*255.0f, blue*255.0f, alpha*255.0f);
}

bool isClearColor(Color c)
{
  if (c[0] == clearColor[0] && c[1] == clearColor[1] && c[2] == clearColor[2])
	return true;
  else
	return false;
}

/**
 * Draws a line with interpolated colors
 */
Line drawLine(int x1, int y1, int x2, int y2)
{
  Line line;
  
  float dy = y2 - y1;
  float dx = x2 - x1;
  float m = dy / dx;			// slope
  float b = y1 - m*(float)x1;	// intercept
  
  Color cStart = getPixelColor(x1, y1);
  Color cEnd = getPixelColor(x2, y2);
  assert(!isClearColor(cStart));
  assert(!isClearColor(cEnd));
  
  int xStart = x1;
  int yStart = y1;
  int xEnd = x2;
  int yEnd = y2;
  
  float d1, d2;	// distances used for color interpolation

  if (fabs(dx) > fabs(dy))					// slope < 1
  {
	float ys;
	dx = (dx > 0) ? 1 : -1;							// determine direction
	while (x1 != x2)
	{
	  x1 += dx;										// increment x
	  ys = (m*(float)x1+b)+0.5;						// +0.5 for rounding
	  d1 = pointDistance(xStart, yStart, x1, ys);
	  d2 = pointDistance(xStart, yStart, x2, y2);
	  Color c = colorInterpolation(cStart, cEnd, d1/d2);
	  assert(!isClearColor(c));
	  if (y1 == y2)
	  {
		for (int i=0; i<=lineWidth/2; i++)
		{
		  setPixel(x1, ys+i, c[0], c[1], c[2]);
		  setPixel(x1, ys-i, c[0], c[1], c[2]);
		}
	  }
	  else
	  {
		for (int i=0; i<=lineWidth/2; i++)
		{
		  setPixel(x1+i, ys, c[0], c[1], c[2]);
		  setPixel(x1-i, ys, c[0], c[1], c[2]);
		}
	  }
	  line.push_back(Point(x1,ys));
	}
  }
  else 												// slope >=1
  {
	dy = (dy > 0) ? 1 : -1;							// determine direction
	while (y1 != y2)
	{
	  y1 += dy;										// increment y
	  x1 = (dx !=0) ? (((float)y1-b)/m)+0.5 : x1;	// make sure dx != 0
	  d1 = pointDistance(xStart, yStart, x1, y1);
	  d2 = pointDistance(xStart, yStart, x2, y2);
	  Color c = colorInterpolation(cStart, cEnd, d1/d2);
	  assert(!isClearColor(c));
	  for (int i=0; i<=lineWidth/2; i++)
	  {
		setPixel(x1+i, y1, c[0], c[1], c[2]);
		setPixel(x1-i, y1, c[0], c[1], c[2]);
	  }
	  line.push_back(Point(x1,y1));
	}
  }
  
  return line;
}

/**
 * Fills a polygon
 * @precondition : l is a list of sorted points (on y) defining the outline
 */
void fillPolygon(Line l)
{
  int curY = l[0][1];
  int curXmin = l[0][0];
  int curXmax = l[0][0];
  for (Line::iterator it = l.begin(); it != l.end(); ++it)
  {
	Point p = *it;
	if (p[1] == curY)
	{
	  curXmin = (p[0] < curXmin) ? p[0] : curXmin;
	  curXmax = (p[0] > curXmax) ? p[0] : curXmax;
	}
	else
	{
	  drawLine(curXmin, curY, curXmax, curY);
	  //cout << curXmin << " , " << curXmax << " , " << curY << endl;
	  curY = p[1];
	  curXmin = p[0];
	  curXmax = p[0];
	}
  }
  drawLine(curXmin, curY, curXmax, curY);	// Just in case no change in y
  //cout << "\n************************\n" << endl;
}

/**
 * Draws a smooth shaded triangle
 */
void drawTriangle(Point p1, Point p2, Point p3)
{
  Line l1 = drawLine(p1[0], p1[1], p2[0], p2[1]);
  Line l2 = drawLine(p2[0], p2[1], p3[0], p3[1]);
  Line l3 = drawLine(p3[0], p3[1], p1[0], p1[1]);
  
  l1.insert(l1.end(), l2.begin(), l2.end());
  l1.insert(l1.end(), l3.begin(), l3.end());
  
  sort(l1.begin(), l1.end(), pointCompareY);
  
  fillPolygon(l1);
}

void drawQuad(Point p1, Point p2, Point p3, Point p4)
{
  Line l1 = drawLine(p1[0], p1[1], p2[0], p2[1]);
  Line l2 = drawLine(p2[0], p2[1], p3[0], p3[1]);
  Line l3 = drawLine(p3[0], p3[1], p4[0], p4[1]);
  Line l4 = drawLine(p4[0], p4[1], p1[0], p1[1]);
  
  l1.insert(l1.begin(), l2.begin(), l2.end());
  l1.insert(l1.begin(), l3.begin(), l3.end());
  l1.insert(l1.begin(), l4.begin(), l4.end());
  
  sort(l1.begin(), l1.end(), pointCompareY);
  
  fillPolygon(l1);
}

/**
 * Draws a line strip and saves the last point
 */
void drawStrip(int x, int y)
{
  if (firstPoint[0] == -1)
	firstPoint.set(x,y);
	
  if (savePoints(x,y,2))
  {
	Point p1 = savedPoints[0];
	Point p2 = savedPoints[1];
	drawLine(p1[0], p1[1], p2[0], p2[1]);
	clearSavedPoints();
	savePoints(x,y,2);
  }
}

/**
 * glVertex specifies a point for drawing, though how it is drawn depends
 * on the mode specified by glBegin.
 * glVertex2i(x,y) specifies the 4-vector point (x,y,0,1).
 *
 * plus possibly other glVertex calls
 */
void bk_glVertex2i(int x, int y)
{
  glVertex2i(x,y);
  
  setPixel(x,y, curColor[0], curColor[1], curColor[2]);
  
  switch (pointMode)
  {
	case GL_POINTS:
	  //setPixel(x,y, curColor[0], curColor[1], curColor[2]);
	  break;
	case GL_LINES:
	  if (savePoints(x,y,2))
	  {
		Point p1 = savedPoints[0];
		Point p2 = savedPoints[1];
		drawLine(p1[0], p1[1], p2[0], p2[1]);
		clearSavedPoints();
	  }
	  break;
	case GL_TRIANGLES:
	  if (savePoints(x,y,3))
	  {
		Point p1 = savedPoints[0];
		Point p2 = savedPoints[1];
		Point p3 = savedPoints[2];
		drawTriangle(p1, p2, p3);
		clearSavedPoints();
	  }
	  break;
	case GL_LINE_STRIP:
	  drawStrip(x,y);
	  break;
	case GL_LINE_LOOP:
	  drawStrip(x,y);
	  break;
	case GL_TRIANGLE_STRIP:
	  if (savePoints(x,y,3))
	  {
		Point p1 = savedPoints[0];
		Point p2 = savedPoints[1];
		Point p3 = savedPoints[2];
		drawTriangle(p1, p2, p3);
		clearSavedPoints();
		savePoints(p2[0],p2[1],3);
		savePoints(p3[0],p3[1],3);
	  }
	  break;
	case GL_POLYGON:
	case GL_TRIANGLE_FAN:
	  if (savePoints(x,y,3))
	  {
		Point p1 = savedPoints[0];
		Point p2 = savedPoints[1];
		Point p3 = savedPoints[2];
		drawTriangle(p1, p2, p3);
		clearSavedPoints();
		savePoints(p1[0],p1[1],3);
		savePoints(p3[0],p3[1],3);
	  }
	  break;
	case GL_QUADS:
	  if (savePoints(x,y,4))
	  {
		Point p1 = savedPoints[0];
		Point p2 = savedPoints[1];
		Point p3 = savedPoints[2];
		Point p4 = savedPoints[3];
		drawQuad(p1, p2, p3, p4);
		clearSavedPoints();
	  }
	  break;
	case GL_QUAD_STRIP:
	  if (savePoints(x,y,4))
	  {
		Point p1 = savedPoints[0];
		Point p2 = savedPoints[1];
		Point p3 = savedPoints[2];
		Point p4 = savedPoints[3];
		drawQuad(p1, p2, p4, p3);
		clearSavedPoints();
		savePoints(p3[0],p3[1],3);
		savePoints(p4[0],p4[1],3);
	  }
	  break;
	default:
	  //TODO begin must be called
	  break;
  }
  return;
}

/**
 * There must be one glEnd for every glBegin.
 * glVertex only works between Begin & End pairs.
 */
void bk_glEnd()
{
  glEnd();
  switch (pointMode)
  {
	case GL_LINE_LOOP:
	  drawStrip(firstPoint[0], firstPoint[1]);
	  break;
	default:
	  break;
  }
  pointMode = -1;
  clearSavedPoints();
}

/**
 * There is one current color (a 4-vector) at all times in OpenGL.
 * Initially, it is (1,1,1,1) (white). Calling glColor3f(r,g,b) sets it to (r,g,b,1).
 */
void bk_glColor3f(float r, float g, float b)
{
  glColor3f(r,g,b);
  curColor.set(r,g,b,1);
}

void bk_glLineWidth(int w)
{
  glLineWidth(w);
  lineWidth = w;
}

/**
 * Draws based on the drawMode
 */
void draw()
{
  bk_glClearColor(0,0,0,1);  // Set the clear color
  bk_glClear(GL_COLOR_BUFFER_BIT);
  
  switch (drawMode)
  {
	case 0:
	  bk_glBegin(GL_POINTS);
	  for(float theta=0, radius=60.0; radius>1.0; theta+=0.1, radius-=0.3)
	  {
		bk_glColor3f(radius/60.0, 0.3, 1-(radius/60.0));
		bk_glVertex2i(200+radius*cos(theta), 200+radius*sin(theta));
	  }
	  bk_glEnd();
	  break;
	case 1:
	  bk_glBegin(GL_LINES);
	  for(int i=0; i<=8; i++)
	  {
		bk_glColor3f(1,0,0);
		bk_glVertex2i(200,200);
		bk_glVertex2i(200 + 10*i, 280);
		bk_glColor3f(0,1,0);
		bk_glVertex2i(200,200);
		bk_glColor3f(0,1,1);
		bk_glVertex2i(200 - 10*i, 280);
		bk_glVertex2i(200,200);
		bk_glVertex2i(280, 200 + 10*i);
		bk_glVertex2i(200,200);
		bk_glVertex2i(280, 200 - 10*i);
	  }
	 bk_glEnd();
	  break;
	case 2:
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(1,0,0);
		bk_glVertex2i(300,300);
		bk_glColor3f(0,1,0);
		bk_glVertex2i(600,300);
		bk_glColor3f(0,0,1);
		bk_glVertex2i(450,410);
		bk_glColor3f(1,1,0);
		bk_glVertex2i(100,400);
		bk_glColor3f(0,1,1);
		bk_glVertex2i(70,60);
		bk_glColor3f(1,0,1);
		bk_glVertex2i(350,100);
	  bk_glEnd();
	  break;
	case 3:
	  bk_glBegin(GL_LINE_STRIP);
		bk_glColor3f(0.42,0.27,0.11);
		bk_glVertex2i(250,30);
		bk_glVertex2i(270,60);
		bk_glVertex2i(270,210);
		bk_glColor3f(0.04,0.70,0.02);
		bk_glVertex2i(230,230);
		bk_glVertex2i(220,270);
		bk_glVertex2i(220,310);
		bk_glVertex2i(250,340);
		bk_glVertex2i(275,360);
		bk_glVertex2i(325,360);
		bk_glVertex2i(350,340);
		bk_glVertex2i(380,310);
		bk_glVertex2i(380,270);
		bk_glVertex2i(370,230);
		bk_glColor3f(0.42,0.27,0.11);
		bk_glVertex2i(330,210);
		bk_glVertex2i(330,60);
		bk_glVertex2i(350,30);
	  bk_glEnd();
	  break;
	case 4:
	  bk_glBegin(GL_LINE_LOOP);
		bk_glColor3f(0.42,0.27,0.11);
		bk_glVertex2i(250,30);
		bk_glVertex2i(270,60);
		bk_glVertex2i(270,210);
		bk_glColor3f(0.04,0.70,0.02);
		bk_glVertex2i(230,230);
		bk_glVertex2i(220,270);
		bk_glVertex2i(220,310);
		bk_glVertex2i(250,340);
		bk_glVertex2i(275,360);
		bk_glVertex2i(325,360);
		bk_glVertex2i(350,340);
		bk_glVertex2i(380,310);
		bk_glVertex2i(380,270);
		bk_glVertex2i(370,230);
		bk_glColor3f(0.42,0.27,0.11);
		bk_glVertex2i(330,210);
		bk_glVertex2i(330,60);
		bk_glVertex2i(350,30);
	  bk_glEnd();
	  break;
	case 5:
	  bk_glBegin(GL_TRIANGLE_STRIP);
		bk_glColor3f(1,0,0);
		bk_glVertex2i(40,70);
		bk_glColor3f(0,1,0);
		bk_glVertex2i(40,390);
		bk_glColor3f(1,1,0);
		bk_glVertex2i(130,30);
		bk_glColor3f(0,0,1);
		bk_glVertex2i(130,350);
		bk_glColor3f(1,0,1);
		bk_glVertex2i(330,80);
		bk_glColor3f(0,1,1);
		bk_glVertex2i(330,400);
		bk_glColor3f(1,0,0);
		bk_glVertex2i(480,40);
		bk_glColor3f(0,1,0);
		bk_glVertex2i(530,330);
	  bk_glEnd();
	  break;
	case 6:
	  bk_glBegin(GL_TRIANGLE_FAN);
		bk_glColor3f(1,0,0);
		bk_glVertex2i(250,170);
		bk_glColor3f(0,1,0);
		bk_glVertex2i(400,140);
		bk_glColor3f(1,1,0);
		bk_glVertex2i(300,50);
		bk_glColor3f(0,0,1);
		bk_glVertex2i(175,55);
		bk_glColor3f(1,0,1);
		bk_glVertex2i(100,170);
		bk_glColor3f(0,1,1);
		bk_glVertex2i(175,285);
		bk_glColor3f(1,0,0);
		bk_glVertex2i(300,290);
		bk_glColor3f(0,1,0);
		bk_glVertex2i(400,200);
	  bk_glEnd();
	  break;
	case 7:
	  bk_glBegin(GL_QUADS);
		bk_glColor3f(1,0,0);
		bk_glVertex2i(40,70);
		bk_glColor3f(0,1,0);
		bk_glVertex2i(40,390);
		bk_glColor3f(0,0,1);
		bk_glVertex2i(130,350);
		bk_glColor3f(1,1,0);
		bk_glVertex2i(130,30);
		bk_glColor3f(1,0,1);
		bk_glVertex2i(330,80);
		bk_glColor3f(0,1,1);
		bk_glVertex2i(330,400);
		bk_glColor3f(0,1,0);
		bk_glVertex2i(530,330);
		bk_glColor3f(1,0,0);
		bk_glVertex2i(480,40);
	  bk_glEnd();
	  break;
	case 8:
	  bk_glBegin(GL_QUAD_STRIP);
		bk_glColor3f(1,0,0);
		bk_glVertex2i(40,70);
		bk_glColor3f(0,1,0);
		bk_glVertex2i(40,390);
		bk_glColor3f(1,1,0);
		bk_glVertex2i(130,30);
		bk_glColor3f(0,0,1);
		bk_glVertex2i(130,350);
		bk_glColor3f(1,0,1);
		bk_glVertex2i(330,80);
		bk_glColor3f(0,1,1);
		bk_glVertex2i(330,400);
		bk_glColor3f(1,0,0);
		bk_glVertex2i(480,40);
		bk_glColor3f(0,1,0);
		bk_glVertex2i(530,330);
	  bk_glEnd();
	  break;
	case 9:
	  bk_glBegin(GL_POLYGON);
		bk_glColor3f(1,0,0);
		bk_glVertex2i(250,170);
		bk_glColor3f(0,1,0);
		bk_glVertex2i(400,140);
		bk_glColor3f(1,1,0);
		bk_glVertex2i(300,50);
		bk_glColor3f(0,0,1);
		bk_glVertex2i(175,55);
		bk_glColor3f(1,0,1);
		bk_glVertex2i(100,170);
		bk_glColor3f(0,1,1);
		bk_glVertex2i(175,285);
		bk_glColor3f(1,0,0);
		bk_glVertex2i(300,290);
		bk_glColor3f(0,1,0);
		bk_glVertex2i(400,200);
	  bk_glEnd();
	  break;
	case 10:
	  bk_glLineWidth(5);
	  bk_glBegin(GL_LINES);
		bk_glColor3f(1,0,0);
		bk_glVertex2i(200,270);
		bk_glColor3f(0,1,0);
		bk_glVertex2i(225,300);
		bk_glVertex2i(225,300);
		bk_glColor3f(1,1,0);
		bk_glVertex2i(255,300);
		bk_glVertex2i(255,300);
		bk_glColor3f(0,0,1);
		bk_glVertex2i(280,270);
		bk_glVertex2i(280,270);
		bk_glColor3f(1,0,1);
		bk_glVertex2i(280,230);
		bk_glVertex2i(280,230);
		bk_glColor3f(0,1,1);
		bk_glVertex2i(240,190);
		bk_glVertex2i(240,190);
		bk_glColor3f(1,0,0);
		bk_glVertex2i(240,160);
		bk_glVertex2i(240,150);
		bk_glColor3f(0,1,0);
		bk_glVertex2i(240,145);
	  bk_glEnd();
	  bk_glLineWidth(1);
	  break;
	default:
	  break;
  }
}

/**
 * Draws what is in the raster
 */
void myDraw()
{
  // Save the old state so that you can set it back after you draw
  GLint oldmatrixmode;
  GLboolean depthWasEnabled = glIsEnabled(GL_DEPTH_TEST);
  glDisable(GL_DEPTH_TEST);
  glGetIntegerv(GL_MATRIX_MODE,&oldmatrixmode);
  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
  
  // Draw the array of pixels (This is where you draw the values
  // you have stored in the array 'raster')
  glRasterPos2f(-1,-1);
  glDrawPixels(WIDTH,HEIGHT,GL_RGB,GL_FLOAT,raster);
  
  //Set the state back to what it was
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW); glPopMatrix();
  glMatrixMode(oldmatrixmode);
  
  if (depthWasEnabled)
	glEnable(GL_DEPTH_TEST);
}

/**
 * Create The Display Function
 */
void display(void)
{
  draw();
  if (drawMine) myDraw();
  glFlush();
  glutSwapBuffers(); // Swap The Buffers To Not Be Left With A Clear Screen
}

/**
 * Create The Reshape Function (the viewport)
 */
void reshape (int w, int h)   
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glOrtho(0, WIDTH, 0, HEIGHT, -1, 1); // TODO only for project 2; delete therafter!
}

// Create Keyboard Function
void keyboard (unsigned char key, int x, int y)  
{
  switch (key)
  {
	case 27:        // When Escape Is Pressed...
	  exit ( 0 );   // Exit The Program
	  break;
	case 49:		// When 1 is pressed..
	  drawMine = false;
	  display();
	  break;
	case 50:		// When 2 is pressed..
	  drawMine = true;
	  display();
	  break;
	default:
	  cout << "key pressed: " << (int)key << endl;
	  break;
  }
}

// Create Special Function (required for arrow keys)
void arrow_keys ( int a_keys, int x, int y ) 
{
  switch (a_keys)
  {
    case GLUT_KEY_UP:     				// When Up Arrow Is Pressed...
      drawMode = (drawMode+1)%11;
      display();
      break;
    case GLUT_KEY_DOWN:               	// When Down Arrow Is Pressed...
      if ((drawMode=drawMode-1) < 0) drawMode=10;
      display();
      break;
    default:
      break;
  }
}

// Create Main Function
int main ( int argc, char** argv )   
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); 	// Display Mode
  glutInitWindowSize(WIDTH, HEIGHT); 			// This is the window size
  glutCreateWindow("OpenGL Example Program"); 	// Window Title
  init();
  glutDisplayFunc(display);  					// Matching Earlier Functions To Their Counterparts
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(arrow_keys);
  glutMainLoop();  								// Initialize The Main Loop
  return 0;
}

