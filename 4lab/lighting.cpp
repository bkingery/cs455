
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
bool reDraw;
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
  reDraw = true;
  drawMode = 0;
}

/**
 * Sets the line width
 */
void bk_glLineWidth(int w)
{
  glLineWidth(w);
  if (drawMine && reDraw) bk.bkLineWidth(w);
}

/**
 * Sets the clear color.
 * 
 */
void bk_glClearColor(float r, float g, float b, float a)
{
  glClearColor(r,g,b,a);
  if (drawMine && reDraw) bk.bkClearColor(r,g,b,a);
}

/**
 * Clears the entire screen to the clear color.
 */
void bk_glClear(GLint bit)
{
  glClear(bit);
  if (drawMine && reDraw) bk.bkClear(bit);
}

/**
 * This tells you how to interpret points.
 * with parameters GL_POINTS, GL_LINES, and GL_TRIANGLES
 */
void bk_glBegin(GLenum mode)
{
  glBegin(mode);
  if (drawMine && reDraw) bk.bkBegin(mode);
}

/**
 * There must be one glEnd for every glBegin.
 * glVertex only works between Begin & End pairs.
 */
void bk_glEnd()
{
  glEnd();
  if (drawMine && reDraw) bk.bkEnd();
}

/**
 *  enable various capabilities
 */
void bk_glEnable(GLenum cap)
{
  glEnable(cap);
  if (drawMine && reDraw) bk.bkEnable(cap);
}

/**
 * disable various capabilities
 */
void bk_glDisable(GLenum cap)
{
  glDisable(cap);
  if (drawMine && reDraw) bk.bkDisable(cap);
}

/**
 * Specifies the active viewport -- that is, the rectangle of pixels OpenGL should render to
 */
void bk_glViewport(int x, int y, int width, int height)
{
  glViewport(x,y,width,height);
  if (drawMine && reDraw) bk.bkViewport(x,y,width,height);
}

/**
 * Sets the current matrix stack
 * Implemented for GL_MODELVIEW and GL_PROJECTION
 */
void bk_glMatrixMode(GLenum e)
{
  glMatrixMode(e);
  if (drawMine && reDraw) bk.bkMatrixMode(e);
}

/**
 * Pushes the current matrix stack down by one, duplicating the current matrix.
 * That is, after a glPushMatrix call, the matrix on top of the stack
 * is identical to the one below it.
 */
void bk_glPushMatrix()
{
  glPushMatrix();
  if (drawMine && reDraw) bk.bkPushMatrix();
}

/**
 * Pops the current matrix stack, replacing the current matrix
 * with the one below it on the stack.
 */
void bk_glPopMatrix()
{
  glPopMatrix();
  if (drawMine && reDraw) bk.bkPopMatrix();
}

/**
 *  Replaces the current matrix with the identity matrix.
 *  It is semantically equivalent to calling glLoadMatrix
 *  with the identity matrix but in some cases it is more efficient.
 */
void bk_glLoadIdentity()
{
  glLoadIdentity();
  if (drawMine && reDraw) bk.bkLoadIdentity();
}

/**
 * Replaces the current matrix with the one whose elements are specified by m.
 * The current matrix is the projection matrix, or modelview matrix,
 * depending on the current matrix mode
 */
void bk_glLoadMatrixd(const double* m)
{
  glLoadMatrixd(m);
  if (drawMine && reDraw) bk.bkLoadMatrixd(m);
}

/**
 * Multiplies the current matrix with the one specified using m,
 * and replaces the current matrix with the product.
 */
void bk_glMultMatrixd(const double* m)
{
  glMultMatrixd(m);
  if (drawMine && reDraw) bk.bkMultMatrixd(m);
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
  if (drawMine && reDraw) bk.bkVertex2i(x,y);
}

/**
 * Specifies the 4-vector x,y,z,w
 */
void bk_glVertex4f(float x, float y, float z, float w)
{
  glVertex4f(x,y,z,w);
  if (drawMine && reDraw) bk.bkVertex4f(x,y,z,w);
}

/**
 * Specifies the 4-vector x,y,z,1
 */
void bk_glVertex3f(float x, float y, float z)
{
  glVertex3f(x,y,z);
  if (drawMine && reDraw) bk.bkVertex3f(x,y,z);
}

/**
 * Specifies the 3-vector x,y,0
 */
void bk_glVertex2f(float x, float y)
{
  glVertex2f(x,y);
  if (drawMine && reDraw) bk.bkVertex2f(x,y);
}

/**
 * There is one current color (a 4-vector) at all times in OpenGL.
 * Initially, it is (1,1,1,1) (white). Calling glColor3f(r,g,b) sets it to (r,g,b,1).
 */
void bk_glColor3f(float r, float g, float b)
{
  glColor3f(r,g,b);
  if (drawMine && reDraw) bk.bkColor3f(r,g,b);
}

/**
 * produces a rotation of angle degrees around the vector (x, y, z).
 */
void bk_glRotatef(float angle, float x, float y, float z)
{
  glRotatef(angle,x,y,z);
  if (drawMine && reDraw) bk.bkRotatef(angle,x,y,z);
}

/**
 * produces a translation by (x,y,z).
 */
void bk_glTranslatef(float x, float y, float z)
{
  glTranslatef(x,y,z);
  if (drawMine && reDraw) bk.bkTranslatef(x,y,z);
}

/**
 * produces a nonuniform scaling along the x, y, and z axes.
 */
void bk_glScalef(float x, float y, float z)
{
  glScalef(x,y,z);
  if (drawMine && reDraw) bk.bkScalef(x,y,z);
}


/**
 * describes a transformation that produces a parallel projection.
 */
void bk_glOrtho(double left, double right, double bottom, double top, double zNear, double zFar)
{
  glOrtho(left, right, bottom, top, zNear, zFar);
  if (drawMine && reDraw) bk.bkOrtho(left, right, bottom, top, zNear, zFar);
}

void bk_glFixedScalef(float sx, float sy, float sz, float cx, float cy, float cz)
{  
  Matrix FS(sx,	0,	0,	cx - cx * sx,
            0, 	sy,	0,	cy - cy * sy, 
            0, 	0,	sz,	cz,
            0, 	0,	0,	1);
  
  glMultMatrixd(FS.data());

  if (drawMine && reDraw) bk.bkFixedScalef(sx,sy,sz, cx,cy,cz);
}

void bk_glShearf(float sxy, float sxz, float syx, float syz, float szx, float szy)
{
  Matrix S(1, 	sxy, 	sxz, 	0,
		   syx,	1,		syz,	0,
		   szx,	szy,	1,		0,
		   0,	0,		0,		1);
  
  glMultMatrixd(S.data());
  
  if (drawMine && reDraw) bk.bkShearf(sxy, sxz, syx, syz, szx, szy);
}

void bk_glFullRotatef(float ang, float ax, float ay, float az, float bx, float by, float bz)
{
  glTranslatef(ax, ay, az);
  glRotatef(ang, bx, by, bz);
  glTranslatef(-ax, -ay, -az);
  
  if (drawMine && reDraw) bk.bkFullRotatef(ang, ax,ay,az, bx,by,bz);
}

void bk_glFrustum(double left, double right, double bottom, double top, double near, double far)
{
  glFrustum(left, right, bottom, top, near, far);
  if (drawMine && reDraw) bk.bkFrustum(left, right, bottom, top, near, far);
}

void bk_gluPerspective(double fovy, double aspect, double near, double far)
{
  gluPerspective(fovy, aspect, near, far);
  if (drawMine && reDraw) bk.bkuPerspective(fovy, aspect, near, far);
}

void bk_glNormal3f(float nx, float ny, float nz)
{
  glNormal3f(nx, ny, nz);
  if (drawMine && reDraw) bk.bkNormal3f(nx, ny, nz);
}

void bk_glLightfv(GLenum light, GLenum pname, const float *params)
{
  glLightfv(light, pname, params);
  if (drawMine && reDraw) bk.bkLightfv(light, pname, params);
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
	case 0: //Perspective projection Frustrum
	{
	  bk_glMatrixMode(GL_PROJECTION);
	  bk_glLoadIdentity();
	  bk_glFrustum(-0.1,0.1, -0.1*480/640,0.1*480/640, 0.1,10);
	  bk_glMatrixMode(GL_MODELVIEW);
	  bk_glLoadIdentity();
	  
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0,0,1);
		bk_glVertex3f(-0.4,-0.6,-1);
		bk_glVertex3f(0.4,-0.6,-1);
		bk_glVertex3f(0.4,-0.1,-1);
		bk_glVertex3f(0.4,-0.1,-1);
		bk_glVertex3f(-0.4,-0.1,-1);
		bk_glVertex3f(-0.4,-0.6,-1);
		bk_glColor3f(1,0,1);
		bk_glVertex3f(-0.4,-0.1,-1);
		bk_glVertex3f(0.4,-0.1,-1);
		bk_glVertex3f(0.3,0,-2);
		bk_glVertex3f(0.3,0,-2);
		bk_glVertex3f(-0.3,0,-2);
		bk_glVertex3f(-0.4,-0.1,-1);
	  glEnd();
	  break;
	}
	case 1: //Perspective projection uPerspective
	{
	  bk_glMatrixMode(GL_PROJECTION);
	  bk_glLoadIdentity();
	  bk_gluPerspective(90, double(640)/480, 0.1, 10);
	  bk_glMatrixMode(GL_MODELVIEW);
	  bk_glLoadIdentity();
	  
	  bk_glBegin(GL_TRIANGLES);
		bk_glColor3f(0,0,1);
		bk_glVertex3f(-0.4,-0.6,-1);
		bk_glVertex3f(0.4,-0.6,-1);
		bk_glVertex3f(0.4,-0.1,-1);
		bk_glVertex3f(0.4,-0.1,-1);
		bk_glVertex3f(-0.4,-0.1,-1);
		bk_glVertex3f(-0.4,-0.6,-1);
		bk_glColor3f(1,0,1);
		bk_glVertex3f(-0.4,-0.1,-1);
		bk_glVertex3f(0.4,-0.1,-1);
		bk_glVertex3f(0.3,0,-2);
		bk_glVertex3f(0.3,0,-2);
		bk_glVertex3f(-0.3,0,-2);
		bk_glVertex3f(-0.4,-0.1,-1);
	  bk_glEnd();
	  break;
	}
	case 2: //Gouraud Shading
	{
	  bk_glMatrixMode(GL_PROJECTION);
	  bk_glLoadIdentity();
	  bk_glMatrixMode(GL_MODELVIEW);
	  bk_glLoadIdentity();
	  bk_glEnable(GL_NORMALIZE);
	  bk_glEnable(GL_LIGHTING);
	  bk_glEnable(GL_COLOR_MATERIAL);
	  bk_glEnable(GL_LIGHT0);
	  float diffuse_color[4] = {1.0,1.0,1.0,1};
	  float ambient_color[4] = {0.1,0.1,0.1,1};
	  float position[4] = {0,3,-10,1};
	  bk_glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_color);
	  bk_glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color);
	  bk_glLightfv(GL_LIGHT0, GL_POSITION, position);
	  
	  bk_glColor3f(1,0,0);
	  float dp = PI/16; // 16 picked arbitrarily; try other numbers too
	  bk_glBegin(GL_TRIANGLES);
	  for(float theta = 0; theta < 2*PI; theta+=dp){
		for(float phi = 0; phi < PI; phi+=dp){
		  bk_glNormal3f(cos(theta)*sin(phi), cos(phi), sin(theta)*sin(phi));
		  bk_glVertex3f(cos(theta)*sin(phi), cos(phi), sin(theta)*sin(phi));
		  bk_glNormal3f(cos(theta+dp)*sin(phi), cos(phi), sin(theta+dp)*sin(phi));
		  bk_glVertex3f(cos(theta+dp)*sin(phi), cos(phi), sin(theta+dp)*sin(phi));
		  bk_glNormal3f(cos(theta+dp)*sin(phi+dp), cos(phi+dp), sin(theta+dp)*sin(phi+dp));
		  bk_glVertex3f(cos(theta+dp)*sin(phi+dp), cos(phi+dp), sin(theta+dp)*sin(phi+dp));
		  bk_glNormal3f(cos(theta)*sin(phi), cos(phi), sin(theta)*sin(phi));
		  bk_glVertex3f(cos(theta)*sin(phi), cos(phi), sin(theta)*sin(phi));
		  bk_glNormal3f(cos(theta+dp)*sin(phi+dp), cos(phi+dp), sin(theta+dp)*sin(phi+dp));
		  bk_glVertex3f(cos(theta+dp)*sin(phi+dp), cos(phi+dp), sin(theta+dp)*sin(phi+dp));
		  bk_glNormal3f(cos(theta)*sin(phi+dp), cos(phi+dp), sin(theta)*sin(phi+dp));
		  bk_glVertex3f(cos(theta)*sin(phi+dp), cos(phi+dp), sin(theta)*sin(phi+dp));
		}
	  }
	  bk_glEnd();
	  bk_glDisable(GL_LIGHTING);
	  break;
	}
	case 3:
	{
	  break;
	}
	case 4:
	{
	  break;
	}
	case 5:
	{
	  break;
	}
	case 6:
	{
	  break;
	}
	case 7:
	{
	  break;
	}
	case 8:
	{
	  break;
	}
	case 9:
	{
	  break;
	}
	case 10:
	{
	  break;
	}
	case 11:
	{
	  break;
	}
	case 12:
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
  reDraw = false;
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
      drawMode = (drawMode+1)%13;
	  reDraw = true;
      display();
      break;
    case GLUT_KEY_DOWN:               	// When Down Arrow Is Pressed...
      if ((drawMode=drawMode-1) < 0) drawMode=12;
	  reDraw = true;
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

