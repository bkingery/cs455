
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <GL/glut.h>   // The GL Utility Toolkit (Glut) Header
#include <iostream>

#include "Scene.h"

#define SCREENHEIGHT 480
#define SCREENWIDTH 640

using namespace std;

int drawMode = 0;
float raster[640*480*3];

Scene scene;

void clear()
{
  for (int i=0; i<(640*480*3); i++)
	raster[i] = 0;
}

void init ()
{
  glShadeModel(GL_SMOOTH);						// Enable Smooth Shading
  glClearDepth(1.0f);							// Depth Buffer Setup
  glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
  glDepthFunc(GL_LEQUAL);						// The Type Of Depth Testing To Do
  glEnable ( GL_COLOR_MATERIAL );
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
  clear();
}

void loadScene()
{
  scene.clearObjects();
  scene.clearLights();
  
  switch (drawMode)
  {
	case 1:
	{
	  Sphere a(cml::vector4f(233.0, 290.0, 0.0, 1), 100.0);
	  Sphere b(cml::vector4f(407.0, 290.0, 0.0, 1), 100.0);
	  Sphere c(cml::vector4f(320.0, 140.0, 0.0, 1), 100.0);
	  Light l1(cml::vector4f(0.0, 240.0, -100.0, 1), cml::vector4f(2.0, 2.0, 2.0, 2.0));
	  Light l2(cml::vector4f(640.0, 240.0, -10000.0, 1), cml::vector4f(0.6, 0.7, 1.0, 2.0));
	  Camera cam(cml::vector4f(0,0,0,1), cml::vector4f(0,0,-1,0));
	  
	  scene.addObject(a);
	  scene.addObject(b);
	  scene.addObject(c);
	  scene.addLight(l1);
	  scene.addLight(l2);
	  scene.setCamera(cam);
	  break;
	}
	case 2:
	  break;
	default:
	  break;
  }
}

cml::vector4f shootRay(Ray r)
{
  return cml::vector4f();
}

void trace()
{
  for (int y=0; y < SCREENHEIGHT; y++)
  {
	for (int x=0; x < SCREENWIDTH; x++)
	{
	  cml::vector4f output(0.0f, 0.0f, 0.0f, 1.0f);
	  for (float fragmentx = float(x); fragmentx < x + 1.0f; fragmentx += 0.5f)
	  for (float fragmenty = float(y); fragmenty < y + 1.0f; fragmenty += 0.5f)
	  {
		  float sampleRatio=0.25f;
		  //TODO shoot rays from camera
		  Ray viewRay(cml::vector4f(fragmentx, fragmenty, -1000.0f, 1), cml::vector4f(0.0f, 0.0f, 1.0f, 1));
		  cml::vector4f temp = shootRay(viewRay);
		  // pseudo photo exposure
		  float exposure = -1.00f; // random exposure value. TODO : determine a good value automatically
		  temp[0] = (1.0f - expf(temp[0] * exposure));
		  temp[1] =  (1.0f - expf(temp[1] * exposure));
		  temp[2] = (1.0f - expf(temp[2] * exposure));

		  output += sampleRatio * temp;
	  }
	  // gamma correction
	  //output[0] = srgbEncode(output[0]);
	  //output[1] = srgbEncode(output[1]);
	  //output[2] = srgbEncode(output[2]);

	  //TODO set the raster
	  //imageFile.put((unsigned char)min(output.blue*255.0f,255.0f)).put((unsigned char)min(output.green*255.0f, 255.0f)).put((unsigned char)min(output.red*255.0f, 255.0f));
	}
  }
}

void display(void)   // Create The Display Function
{
  loadScene();
  trace();
  
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
	  //drawMode = 1;
	  //display();
	  break;
	case 50:		// When 2 is pressed..
	  //drawMode = 2;
	  //display();
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

