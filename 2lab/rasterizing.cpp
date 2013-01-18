
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
#define HIGHT 480
#define WIDTH 640

bool drawMine;
int drawMode;
GLenum pointMode;

// typedefs
typedef cml::vector4f Color;
typedef cml::vector2i Point;
typedef vector<Point> Line;

// Data Structures
float raster[WIDTH*HIGHT*3];

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
	for (int y=0; y<HIGHT; y++)
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

/**
 * Draws a line with interpolated colors
 */
Line drawLine(int x1, int y1, int x2, int y2)
{
  Line line;
  
  float dy = y2 - y1;
  float dx = x2 - x1;
  float m,b;
  
  Color cStart = getPixelColor(x1, y1);
  Color cEnd = getPixelColor(x2, y2);
  int xStart = x1;
  int yStart = y1;
  int xEnd = x2;
  int yEnd = y2;

  if (fabs(dx) > fabs(dy))						// slope < 1
  {
	  m = dy / dx;								// compute slope
	  b = y1 - m*(float)x1;
	  dx = (dx > 0) ? 1 : -1;					// determine direction
	  while (x1 != x2)
	  {	
		  x1 += dx;								// increment x
		  float ys = (m*(float)x1+b);
		  float d1 = pointDistance(xStart, yStart, x1, ys);
		  float d2 = pointDistance(xStart, yStart, x2, y2);
		  Color c = colorInterpolation(cStart, cEnd, d1/d2);
		  setPixel(x1, ys, c[0], c[1], c[2]);
		  line.push_back(Point(x1,ys));
	  }
  }
  else 											// slope >=1
  {
	  m = dy / dx;								// compute slope
	  b = y1 - m*(float)x1;
	  dy = (dy > 0) ? 1 : -1;					// determine direction
	  while (y1 != y2)
	  {
		  y1 += dy;								// increment y
		  x1 = (dx !=0) ? (((float)y1-b)/m) : x1;	// make sure dx != 0
		  float d1 = pointDistance(xStart, yStart, x1, y1);
		  float d2 = pointDistance(xStart, yStart, x2, y2);
		  Color c = colorInterpolation(cStart, cEnd, d1/d2);
		  setPixel(x1, y1, c[0], c[1], c[2]);
		  line.push_back(Point(x1,y1));
	  }
  }
  
  return line;
}

/**
 * Draws a smooth shaded triangle
 */
void drawTriangle(Point p1, Point p2, Point p3)
{
  Line l1 = drawLine(p1[0], p1[1], p2[0], p2[1]);
  Line l2 = drawLine(p2[0], p2[1], p3[0], p3[1]);
  Line l3 = drawLine(p3[0], p3[1], p1[0], p1[1]);
  
  l1.insert(l1.begin(), l2.begin(), l2.end());
  l1.insert(l1.begin(), l3.begin(), l3.end());
  
  sort(l1.begin(), l1.end(), pointCompareY);
  
  vector<Point> xmins;
  vector<Point> ymaxs;
  
  int curY = l1[0][1];
  int curXmin = l1[0][0];
  int curXmax = l1[0][0];
  for (Line::iterator it = l1.begin(); it != l1.end(); ++it)
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
	  curY = p[1];
	  curXmin = p[0];
	  curXmax = p[0];
	}
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
  pointMode = -1;
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
	  break;
	case 4:
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
  glDrawPixels(WIDTH,HIGHT,GL_RGB,GL_FLOAT,raster);
  
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
  glOrtho(0, WIDTH, 0, HIGHT, -1, 1); // TODO only for project 2; delete therafter!
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
      drawMode = (drawMode+1)%5;
      display();
      break;
    case GLUT_KEY_DOWN:               	// When Down Arrow Is Pressed...
      if ((drawMode=drawMode-1) < 0) drawMode=4;
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
  glutInitWindowSize(WIDTH, HIGHT); 			// This is the window size
  glutCreateWindow("OpenGL Example Program"); 	// Window Title
  init();
  glutDisplayFunc(display);  					// Matching Earlier Functions To Their Counterparts
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(arrow_keys);
  glutMainLoop();  								// Initialize The Main Loop
  return 0;
}

