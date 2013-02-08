
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

#define PI (3.141592653589793)

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

/**
 * Gets the color of the pixel specified by x,y
 */
Color getPixelColor(int x, int y)
{
  Color c = Color(0,0,0,1);
  c[0] = raster[((y*WIDTH) + x)*3 + 0];
  c[1] = raster[((y*WIDTH) + x)*3 + 1];
  c[2] = raster[((y*WIDTH) + x)*3 + 2];
  
  return c;
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
 * Sets the clear color.
 * 
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
  return;
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
 *  enable various capabilities
 */
void bk_glEnable(GLenum cap)
{
  glEnable(cap);
  return;
}

/**
 * disable various capabilities
 */
void bk_glDisable(GLenum cap)
{
  glDisable(cap);
  return;
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

  if (fabs(dx) > fabs(dy))							// slope < 1
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

/**
 * Draws a quad
 */
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
 * For GL_MODELVIEW and GL_PROJECTION
 */
void bk_glMatrixMode(GLenum e)
{
  glMatrixMode(e);
  return;
}

/**
 * Specifies the active viewport -- that is, the rectangle of pixels OpenGL should render to
 */
void bk_glViewport(int x, int y, int width, int height)
{
  glViewport(x,y,width,height);
  return;
}

/**
 * Pushes the current matrix stack down by one, duplicating the current matrix.
 * That is, after a glPushMatrix call, the matrix on top of the stack
 * is identical to the one below it.
 */
void bk_glPushMatrix()
{
  glPushMatrix();
  return;
}

/**
 * Pops the current matrix stack, replacing the current matrix
 * with the one below it on the stack.
 */
void bk_glPopMatrix()
{
  glPopMatrix();
  return;
}

/**
 * 
 */
void bk_glVertex3f(float x, float y, float z)
{
  glVertex3f(x,y,z);
  return;
}

/**
 * 
 */
void bk_glVertex4f(float x, float y, float z, float w)
{
  glVertex4f(x,y,z,w);
  return;
}

/**
 *  Replaces the current matrix with the identity matrix.
 *  It is semantically equivalent to calling glLoadMatrix
 *  with the identity matrix but in some cases it is more efficient.
 */
void bk_glLoadIdentity()
{
  glLoadIdentity();
  return;
}

/**
 * Replaces the current matrix with the one whose elements are specified by m.
 * The current matrix is the projection matrix, modelview matrix, or texture matrix,
 * depending on the current matrix mode
 */
void bk_glLoadMatrixd(double* m)
{
  glLoadMatrixd(m);
  return;
}

/**
 * Multiplies the current matrix with the one specified using m,
 * and replaces the current matrix with the product.
 */
void bk_glMultMatrixd(const double* m)
{
  glMultMatrixd(m);
  return;
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
 * 
 */
void bk_glVertex2f(float x, float y)
{
  glVertex2f(x,y);
  return;
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

/**
 * Sets the line width
 */
void bk_glLineWidth(int w)
{
  glLineWidth(w);
  lineWidth = w;
}

/**
 * produces a rotation of angle degrees around the vector (x,  y,
       z).   The current matrix (see glMatrixMode) is multiplied by a rotation
       matrix  with  the  product  replacing  the  current   matrix,   as   if
       glMultMatrix were called with the following matrix as its argument:

         x^2(1-c)+c     xy(1-c)-zs     xz(1-c)+ys     0
         yx(1-c)+zs     y^2(1-c)+c     yz(1-c)-xs     0
         xz(1-c)-ys     yz(1-c)+xs     z^2(1-c)+c     0
              0              0               0        1

       Where  c = cos (angle), s = sin (angle), and ||(x, y, z)|| = 1 (if not,
       the GL will normalize this vector).

       If the matrix mode is either GL_MODELVIEW or GL_PROJECTION, all objects
       drawn  after  glRotate  is  called  are  rotated.  Use glPushMatrix and
       glPopMatrix to save and restore the unrotated coordinate system.
 */
void bk_glRotatef(float angle, float x, float y, float z)
{
  glRotatef(angle,x,y,z);
  return;
}

/**
 * produces a translation by (x,y,z).  The current matrix (see
	glMatrixMode)  is multiplied by this translation matrix, with the prod-
	uct replacing the current matrix, as if glMultMatrix were  called  with
	the following matrix for its argument:

						   1 0 0 x
						   0 1 0 y
						   0 0 1 z
						   0 0 0 1

	If the matrix mode is either GL_MODELVIEW or GL_PROJECTION, all objects
	drawn after a call to glTranslate are translated.

	Use glPushMatrix and glPopMatrix to save and restore  the  untranslated
	coordinate system.
 */
void bk_glTranslatef(float x, float y, float z)
{
  glTranslatef(x,y,z);
  return;
}

/**
 * produces a nonuniform scaling along the x, y, and z axes.  The
	three parameters indicate the desired scale factor along  each  of  the
	three axes.

	The  current  matrix  (see  glMatrixMode)  is  multiplied by this scale
	matrix, and the product replaces the current matrix as if glScale  were
	called with the following matrix as its argument:

						  x 0 0 0
						  0 y 0 0
						  0 0 z 0
						  0 0 0 1

	If the matrix mode is either GL_MODELVIEW or GL_PROJECTION, all objects
	drawn after glScale is called are scaled.

	Use glPushMatrix and glPopMatrix to save and restore the unscaled coor-
	dinate system.
 */
void bk_glScalef(float x, float y, float z)
{
  glScalef(x,y,z);
  return;
}

/**
 * describes a transformation that produces a parallel projection.
	The current matrix (see glMatrixMode) is multiplied by this matrix  and
	the  result replaces the current matrix, as if glMultMatrix were called
	with the following matrix as its argument:

		   2
	 ------------       0              0              tx
	 right - left

						2
		 0         ------------        0              ty
				   top - bottom


									   -2
		 0              0         ------------        tz
								   zFar-zNear

		 0              0              0              1

	where
					tx = - (right + left) / (right - left)

					ty = - (top + bottom) / (top - bottom)

					tz = - (zFar + zNear) / (zFar - zNear)

	Typically,  the  matrix  mode  is  GL_PROJECTION,  and  (left,  bottom,
	-zNear)  and (right, top,  -zNear) specify the points on the near clip-
	ping plane that are mapped to the lower left and upper right corners of
	the  window,  respectively,  assuming that the eye is located at (0, 0,
	0).  -zFar specifies the location of  the  far  clipping  plane.   Both
	zNear and zFar can be either positive or negative.

	Use glPushMatrix and glPopMatrix to save and restore the current matrix
	stack.
 */
void bk_glOrtho(double left, double right, double bottom, double top, double zNear, double zFar)
{
  glOrtho(left, right, bottom, top, zNear, zFar);
  return;
}

/**
 * Used for testing matrix stack functionality
 */
void tree(int depth)
{
  static const double r2 = 1/sqrt(2);
  static const double mdown[16] = { 0,-r2,0,0, r2,0,0,0, 0,0,1,0, 0,-r2,0,1 };
  static const double mup[16] = { 0,r2,0,0, -r2,0,0,0, 0,0,1,0, 0,r2,0,1 };
  if (depth <= 0) return;
  bk_glBegin(GL_LINES);
	bk_glVertex2f(0,-r2);	//TODO I changed this from 2f
	bk_glVertex2f(0, r2);	//TODO I changed this from 2f
  bk_glEnd();
  
  bk_glPushMatrix();
  bk_glMultMatrixd(mdown);
  
  tree(depth-1);
  
  bk_glPopMatrix();
  bk_glPushMatrix();
  bk_glMultMatrixd(mup);
  
  tree(depth-1);
  
  bk_glPopMatrix();
}

/**
 * Draws based on the drawMode
 */
void draw()
{
  bk_glClearColor(0,0,0,1);  // Set the clear color
  bk_glClear(GL_COLOR_BUFFER_BIT);
  bk_glClear(GL_DEPTH_BUFFER_BIT);
  
  switch (drawMode)
  {
	case 0://Viewport transformation
	{
	  bk_glViewport(0,0,320,240);
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0,1,0);
		bk_glVertex3f(-1,0,0);
		bk_glVertex3f(0,-0.8,0);
		bk_glVertex3f(0.5,0.8,0);
	  bk_glEnd();
	  bk_glViewport(320,240,320,240);
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0,0,1);
		bk_glVertex3f(-1,0.8,0);
		bk_glVertex3f(0.1,-0.8,0);
		bk_glVertex3f(0.5,0.8,0);
	  bk_glEnd();
	  //Restore your viewport to the whole screen
	  bk_glViewport(0,0,WIDTH,HEIGHT);
	  break;
	}
	case 1:	//Divide-by-w
	{
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(1,0,0);
		bk_glVertex4f(-1,0.2,0,1);
		bk_glVertex4f(0,0.8,0,1);
		bk_glVertex4f(1,0.2,0,1);
		bk_glColor3f(1,0,1);
		bk_glVertex4f(-1,-0.8,0,2);
		bk_glVertex4f(0,-0.2,0,2);
		bk_glVertex4f(1,-0.8,0,2);
	  bk_glEnd();
	  break;
	}
	case 2:	//Depth buffer
	{
	  bk_glEnable(GL_DEPTH_TEST);
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0,1,1);
		bk_glVertex3f(-0.5,0.2,0.5);
		bk_glVertex3f(0,-0.5,0);
		bk_glVertex3f(0.5,0.2,-0.5);
		bk_glColor3f(1,1,0);
		bk_glVertex3f(-0.5,-0.2,-0.5);
		bk_glVertex3f(0,0.5,0);
		bk_glVertex3f(0.5,-0.2,0.5);
	  bk_glEnd();
	  bk_glDisable(GL_DEPTH_TEST);
	  break;
	}
	case 3: //Easy point clipping
	{
	  bk_glEnable(GL_DEPTH_TEST);
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0.5,1,1);
		bk_glVertex3f(0.5,0,0);
		bk_glVertex3f(0,0.5,-2);
		bk_glVertex3f(0,-0.5,2);
	  bk_glEnd();
	  bk_glViewport(50,50,200,400);
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(1,1,0.5);
		bk_glVertex3f(-1.4,-1.2,-0.5);
		bk_glVertex3f(0,1.2,0);
		bk_glVertex3f(1.5,-0.2,0.5);
	  bk_glEnd();
	  //Restore your viewport to the whole screen
	  bk_glViewport(0,0,WIDTH,HEIGHT);
	  bk_glDisable(GL_DEPTH_TEST);
	  break;
	}
	case 4:	//Matrix manipulation
	{
	  double translate[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, -1.2,0.3,0,1};
	  double rotate[16] = {cos(PI/2),sin(PI/2),0,0, -sin(PI/2),cos(PI/2),0,0, 0,0,1,0, 0,0,0,1};
	  bk_glLoadIdentity();
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0.5,0.2,1);
		bk_glVertex3f(0.5,0.1,0);
		bk_glVertex3f(0.8,0.1,0);
		bk_glVertex3f(0.65,0.4,0);
	  bk_glEnd();
	  bk_glLoadMatrixd(translate);
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0.5,0.8,0.2);
		bk_glVertex3f(0.5,0.1,0);
		bk_glVertex3f(0.8,0.1,0);
		bk_glVertex3f(0.65,0.4,0);
	  bk_glEnd();
	  bk_glLoadIdentity();
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0.2,0.6,1);
		bk_glVertex3f(0.5,-0.4,0);
		bk_glVertex3f(0.8,-0.4,0);
		bk_glVertex3f(0.65,-0.7,0);
	  bk_glEnd();
	  bk_glLoadMatrixd(rotate);
	  bk_glMultMatrixd(translate);
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0.9,0.2,0.4);
		bk_glVertex3f(0.5,-0.4,0);
		bk_glVertex3f(0.8,-0.4,0);
		bk_glVertex3f(0.65,-0.7,0);
	  bk_glEnd();
	  bk_glLoadIdentity();
	  break;
	}
	case 5:	//Matrix stacks
	{
	  bk_glColor3f(0,255,255);
	  tree(8);
	  break;
	}
	case 6:	//Rotate
	{
	  bk_glLoadIdentity();
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0.5,0.2,1);
		bk_glVertex3f(0.5,0.1,0);
		bk_glVertex3f(0.8,0.1,0);
		bk_glVertex3f(0.65,0.4,0);
	  bk_glEnd();
	  bk_glRotatef(90,0,0,1);
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0.1,0.2,1);
		bk_glVertex3f(0.5,0.1,0);
		bk_glVertex3f(0.8,0.1,0);
		bk_glVertex3f(0.65,0.4,0);
	  bk_glEnd();
	  bk_glRotatef(-90,0,0,1);
	  break;
	}
	case 7: //Translate
	{
	  bk_glLoadIdentity();
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0.33,0.77,0);
		bk_glVertex3f(0.5,0.1,0);
		bk_glVertex3f(0.8,0.1,0);
		bk_glVertex3f(0.65,0.4,0);
	  bk_glEnd();
	  bk_glTranslatef(-1,-1,0);
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0.77,0.2,0.3);
		bk_glVertex3f(0.5,0.1,0);
		bk_glVertex3f(0.8,0.1,0);
		bk_glVertex3f(0.65,0.4,0);
	  bk_glEnd();
	  bk_glTranslatef(1,1,0);
	  break;
	}
	case 8:	//Scale
	{
	  bk_glLoadIdentity();
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0.9,0.5,1);
		bk_glVertex3f(0.5,0.4,0);
		bk_glVertex3f(0.8,0.4,0);
		bk_glVertex3f(0.65,0.9,0);
	  bk_glEnd();
	  bk_glScalef(0.8,0.7,1);
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0.8,0.7,0);
		bk_glVertex3f(0.5,0.1,0);
		bk_glVertex3f(0.8,0.1,0);
		bk_glVertex3f(0.65,0.4,0);
	  bk_glEnd();
	  bk_glScalef(-0.8, 0.7,1);
	  break;
	}
	case 9:	//Ortho
	{
	  bk_glLoadIdentity();
	  bk_glOrtho(0,640,0,480,-1,1);
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(1,0,0);
		bk_glVertex3f(300,300,0);
		bk_glColor3f(0,1,0);
		bk_glVertex3f(600,300,0);
		bk_glColor3f(0,0,1);
		bk_glVertex3f(450,410,0);
		bk_glColor3f(1,1,0);
		bk_glVertex3f(100,400,0);
		bk_glColor3f(0,1,1);
		bk_glVertex3f(70,60,0);
		bk_glColor3f(1,0,1);
		bk_glVertex3f(350,100,0);
	  bk_glEnd();
	  bk_glLoadIdentity();
	  break;
	}
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
      drawMode = (drawMode+1)%10;
      display();
      break;
    case GLUT_KEY_DOWN:               	// When Down Arrow Is Pressed...
      if ((drawMode=drawMode-1) < 0) drawMode=9;
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

