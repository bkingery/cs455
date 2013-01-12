
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <GL/glut.h>   		// The GL Utility Toolkit (Glut) Header
#include <cml/cml.h>
#include <iostream>
using namespace std;

// Globals
#define HIGHT 480
#define WIDTH 640

bool drawMine;
int drawMode;
GLenum pointMode;

// Data Structures
float raster[WIDTH*HIGHT*3];
cml::vector4f curColor;
cml::vector4f clearColor;
cml::vector2i p1;
cml::vector2i p2;
cml::vector2i p3;

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
  p1 = cml::vector2i(-1,-1);
  p2 = cml::vector2i(-1,-1);
  p3 = cml::vecotr2i(-1,-1);
  
  curColor = cml::vector4f(1,1,1,1);
  clearColor = cml::vector4f(1,1,1,1);
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
 * Saves 2 points for drawing a line
 * return true when 2 points are saved.
 */
bool saveTwoPoints(int x, int y)
{
  if (p1[0] == -1)
  {
	p1.set(x,y);
	return false;
  }
  p2.set(x,y);
  return true;
}

bool saveThreePoints(int x, int y)
{
  if(p1[0] == -1)
  {
	p1.set(x,y);
	return false;
  }
  else if (p2[0] == -1)
  {
	p2.set(x,y);
	return false;
  }
  p3.set(x,y);
  return true;
}

void clearSavedPoints()
{
  p1.set(-1,-1);
  p2.set(-1,-1);
  p3.set(-1,-1);
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
  
  switch (pointMode)
  {
	case GL_POINTS:
	  setPixel(x,y, curColor[0], curColor[1], curColor[2]);
	  break;
	case GL_LINES:
	  if (saveTwoPoints(x,y))
	  {
		//TODO drawLine
		clearSavedPoints();
	  }
	  break;
	case GL_TRIANGLES:
	  if (saveThreePoints(x,y))
	  {
		//TODO draw triangle
		clearSavedPoints();
	  }
	  break;
	default:
	  break;
  }
  //TODO begin must be called
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

