
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <GL/glut.h>   // The GL Utility Toolkit (Glut) Header
#include <iostream>
using namespace std;

int drawMode = 0;
float raster[640*480*3];

void init ()
{
  glShadeModel(GL_SMOOTH);						// Enable Smooth Shading
  glClearDepth(1.0f);							// Depth Buffer Setup
  glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
  glDepthFunc(GL_LEQUAL);						// The Type Of Depth Testing To Do
  glEnable ( GL_COLOR_MATERIAL );
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
  for (int i=0; i<(640*480*3); i++)
	raster[i] = ((float)i)/(640*480*3);
}

void display(void)   // Create The Display Function
{
  glClearColor(1,0,0,1);  // Set the clear color
  // Clear the screen to the clear color (i.e. if the clear color
  // is red, the screen turns red);
  glClear(GL_COLOR_BUFFER_BIT);
        
  if (drawMode == 2)
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
	glDrawPixels(640,480,GL_RGB,GL_FLOAT,raster);
	
	//Set the state back to what it was
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW); glPopMatrix();
	glMatrixMode(oldmatrixmode);
	
	if (depthWasEnabled)
	  glEnable(GL_DEPTH_TEST);
  }
  glFlush();
  
  // Swap The Buffers To Not Be Left With A Clear Screen
  glutSwapBuffers();
}
  
// Create The Reshape Function (the viewport)
void reshape ( int w, int h )   
{
  glViewport( 0, 0, w, h );
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

// Create Keyboard Function
void keyboard ( unsigned char key, int x, int y )  
{
  switch ( key )
  {
	case 27:        // When Escape Is Pressed...
	  exit ( 0 );   // Exit The Program
	  break;
	case 49:		// When 1 is pressed..
	  drawMode = 1;
	  display();
	  break;
	case 50:		// When 2 is pressed..
	  drawMode = 2;
	  display();
	  break;
	default:
	  break;
  }
}

// Create Special Function (required for arrow keys)
void arrow_keys ( int a_keys, int x, int y ) 
{
  switch ( a_keys ) {
    case GLUT_KEY_UP:     				// When Up Arrow Is Pressed...
      drawMode = (drawMode+1)%5;
      display();
      break;
    case GLUT_KEY_DOWN:               	// When Down Arrow Is Pressed...
      if ((drawMode=drawMode-1) < 0)drawMode=4;
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
  glutInitWindowSize(640, 480); 				// This is the window size
  glutCreateWindow("OpenGL Example Program"); 	// Window Title
  init();
  glutDisplayFunc(display);  					// Matching Earlier Functions To Their Counterparts
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(arrow_keys);
  glutMainLoop();  								// Initialize The Main Loop
  return 0;
}

