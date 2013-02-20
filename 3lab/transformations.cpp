
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <GL/glut.h>   		// The GL Utility Toolkit (Glut) Header
#include <cml/cml.h>
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

#include "bkgl.h"

using namespace std;

bool drawMine;
int drawMode;

bkgl bk;

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
}

/**
 * Sets the line width
 */
void bk_glLineWidth(int w)
{
  glLineWidth(w);
  bk.bkLineWidth(w);
}

/**
 * Sets the clear color.
 * 
 */
void bk_glClearColor(float r, float g, float b, float a)
{
  glClearColor(r,g,b,a);
  bk.bkClearColor(r,g,b,a);
}

/**
 * Clears the entire screen to the clear color.
 */
void bk_glClear(GLint bit)
{
  glClear(bit);
  bk.bkClear(bit);
}

/**
 * This tells you how to interpret points.
 * with parameters GL_POINTS, GL_LINES, and GL_TRIANGLES
 */
void bk_glBegin(GLenum mode)
{
  glBegin(mode);
  bk.bkBegin(mode);
}

/**
 * There must be one glEnd for every glBegin.
 * glVertex only works between Begin & End pairs.
 */
void bk_glEnd()
{
  glEnd();
  bk.bkEnd();
}

/**
 *  enable various capabilities
 */
void bk_glEnable(GLenum cap)
{
  glEnable(cap);
  bk.bkEnable(cap);
}

/**
 * disable various capabilities
 */
void bk_glDisable(GLenum cap)
{
  glDisable(cap);
  bk.bkDisable(cap);
}

/**
 * Specifies the active viewport -- that is, the rectangle of pixels OpenGL should render to
 */
void bk_glViewport(int x, int y, int width, int height)
{
  glViewport(x,y,width,height);
  bk.bkViewport(x,y,width,height);
}

/**
 * Sets the current matrix stack
 * Implemented for GL_MODELVIEW and GL_PROJECTION
 */
void bk_glMatrixMode(GLenum e)
{
  glMatrixMode(e);
  bk.bkMatrixMode(e);
}

/**
 * Pushes the current matrix stack down by one, duplicating the current matrix.
 * That is, after a glPushMatrix call, the matrix on top of the stack
 * is identical to the one below it.
 */
void bk_glPushMatrix()
{
  glPushMatrix();
  bk.bkPushMatrix();
}

/**
 * Pops the current matrix stack, replacing the current matrix
 * with the one below it on the stack.
 */
void bk_glPopMatrix()
{
  glPopMatrix();
  bk.bkPopMatrix();
}

/**
 *  Replaces the current matrix with the identity matrix.
 *  It is semantically equivalent to calling glLoadMatrix
 *  with the identity matrix but in some cases it is more efficient.
 */
void bk_glLoadIdentity()
{
  glLoadIdentity();
  bk.bkLoadIdentity();
}

/**
 * Replaces the current matrix with the one whose elements are specified by m.
 * The current matrix is the projection matrix, or modelview matrix,
 * depending on the current matrix mode
 */
void bk_glLoadMatrixd(const double* m)
{
  glLoadMatrixd(m);
  bk.bkLoadMatrixd(m);
}

/**
 * Multiplies the current matrix with the one specified using m,
 * and replaces the current matrix with the product.
 */
void bk_glMultMatrixd(const double* m)
{
  glMultMatrixd(m);
  bk.bkMultMatrixd(m);
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
  bk.bkVertex2i(x,y);
}

/**
 * Specifies the 4-vector x,y,z,w
 */
void bk_glVertex4f(float x, float y, float z, float w)
{
  glVertex4f(x,y,z,w);
  bk.bkVertex4f(x,y,z,w);
}

/**
 * Specifies the 4-vector x,y,z,1
 */
void bk_glVertex3f(float x, float y, float z)
{
  glVertex3f(x,y,z);
  bk.bkVertex3f(x,y,z);
}

/**
 * Specifies the 3-vector x,y,0
 */
void bk_glVertex2f(float x, float y)
{
  glVertex2f(x,y);
  bk.bkVertex2f(x,y);
}

/**
 * There is one current color (a 4-vector) at all times in OpenGL.
 * Initially, it is (1,1,1,1) (white). Calling glColor3f(r,g,b) sets it to (r,g,b,1).
 */
void bk_glColor3f(float r, float g, float b)
{
  glColor3f(r,g,b);
  bk.bkColor3f(r,g,b);
}

/**
 * produces a rotation of angle degrees around the vector (x, y, z).
 */
void bk_glRotatef(float angle, float x, float y, float z)
{
  glRotatef(angle,x,y,z);
  bk.bkRotatef(angle,x,y,z);
}

/**
 * produces a translation by (x,y,z).
 */
void bk_glTranslatef(float x, float y, float z)
{
  glTranslatef(x,y,z);
  bk.bkTranslatef(x,y,z);
}

/**
 * produces a nonuniform scaling along the x, y, and z axes.
 */
void bk_glScalef(float x, float y, float z)
{
  glScalef(x,y,z);
  bk.bkScalef(x,y,z);
}

/**
 * describes a transformation that produces a parallel projection.
 */
void bk_glOrtho(double left, double right, double bottom, double top, double zNear, double zFar)
{
  glOrtho(left, right, bottom, top, zNear, zFar);
  bk.bkOrtho(left, right, bottom, top, zNear, zFar);
}

void bk_glFixedScalef(float sx, float sy, float sz, float cx, float cy, float cz)
{
  //TODO OpenGL
  bk.bkFixedScalef(sx,sy,sz, cx,cy,cz);
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
	bk_glVertex2f(0,-r2);
	bk_glVertex2f(0, r2);
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
	  bk_glViewport(0,0,SCREENWIDTH,SCREENHEIGHT);
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
	  bk_glViewport(0,0,SCREENWIDTH,SCREENHEIGHT);
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
	case 10: //Fixed Scale
	{
	  bk_glLoadIdentity();
	  bk_gBegin(GL_TRIANGLES);
		bk_glColor3f(0.9,0.5,1);
		bk_glVertex3f(0.5,0.4,0);
		bk_glVertex3f(0.8,0.4,0);
		bk_glVertex3f(0.65,0.9,0);
	  bk_glEnd();
	  bk_glFixedScalef(0.8,0.7,1, 0.5, 0.4, 0);
	  bk_gBegin(GL_TRIANGLES);
		bk_glColor3f(0.9,0.5,1);
		bk_glVertex3f(0.5,0.4,0);
		bk_glVertex3f(0.8,0.4,0);
		bk_glVertex3f(0.65,0.9,0);
	  bk_glEnd();
	  break;
	}
	case 11: //Shear
	{
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
  glDrawPixels(SCREENWIDTH,SCREENHEIGHT,GL_RGB,GL_FLOAT, bk.getRaster());
  
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
  bk_glViewport(0, 0, w, h);
  bk_glMatrixMode(GL_PROJECTION);
  bk_glLoadIdentity();
  bk_glMatrixMode(GL_MODELVIEW);
  bk_glLoadIdentity();
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
      drawMode = (drawMode+1)%12;
      display();
      break;
    case GLUT_KEY_DOWN:               	// When Down Arrow Is Pressed...
      if ((drawMode=drawMode-1) < 0) drawMode=11;
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
  glutInitWindowSize(SCREENWIDTH, SCREENHEIGHT); 			// This is the window size
  glutCreateWindow("OpenGL Example Program"); 	// Window Title
  init();
  glutDisplayFunc(display);  					// Matching Earlier Functions To Their Counterparts
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(arrow_keys);
  glutMainLoop();  								// Initialize The Main Loop
  return 0;
}

