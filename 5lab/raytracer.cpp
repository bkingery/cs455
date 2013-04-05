
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
	case 0:
	{
	  Sphere a(cml::vector4f(320.0, 240.0, 0.0, 1), 100.0);
	  Light l1(cml::vector4f(0.0, 240.0, -100.0, 1), cml::vector4f(2.0, 2.0, 2.0, 2.0));
	  Camera cam(cml::vector4f(0,0,0,1), cml::vector4f(0,0,-1,0));
	  
	  scene.addObject(a);
	  scene.addLight(l1);
	  scene.setCamera(cam);
	  break;
	}
	case 1:
	{
	  Sphere a(cml::vector4f(233.0, 290.0, 0.0, 1), 100.0);
	  a.setMaterial(cml::vector4f(1.0, 1.0, 0.0, 1.0), 0.5, cml::vector4f(1,1,1,1));
	  Sphere b(cml::vector4f(407.0, 290.0, 0.0, 1), 100.0);
	  b.setMaterial(cml::vector4f(0.0, 1.0, 1.0, 1.0), 0.5, cml::vector4f(1,1,1,1));
	  Sphere c(cml::vector4f(320.0, 140.0, 0.0, 1), 100.0);
	  c.setMaterial(cml::vector4f(1.0, 0.0, 1.0, 1.0), 0.5, cml::vector4f(1,1,1,1));
	  
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
	default:
	  break;
  }
}

Sphere getClosestHitObject(Ray r)
{
  vector<Sphere> objects = scene.getObjects();
  float t = -1;
  Sphere s;
  for (int i = 0; i< objects.size(); i++)
  {
	Sphere temp = objects[i];
	if (temp.hit(r) && (t == -1 || temp.getCurDist() < t))
	  s = temp;
  }
  return s;
}

bool inShadow(Ray lightRay, float lightDist)
{
  //use this in hit to make sure the object isn't beyond the light
  vector<Sphere> objects = scene.getObjects();
  for (int i = 0; i < objects.size(); i++)
  {
	Sphere s = objects[i];
	if (s.hit(lightRay) && s.getCurDist() < lightDist)
	  return true;
  }
  return false;
}

/**
 */
cml::vector4f elementwise_mult(const cml::vector4f& v1, const cml::vector4f& v2)
{
  return cml::vector4f(v1[0]*v2[0], v1[1]*v2[1], v1[2]*v2[2], v1[3]*v2[3]);
}

/**
 */
cml::vector4f applyLightToColor(cml::vector4f light, cml::vector4f color, cml::vector4f specColor)
{
  //cml::vector4f newcolor = color;
  //if (gllighting and !glcolormaterial)
//	newcolor = elementwise_mult(light + cml::vector4f(0.2, 0.2, 0.2, 0), cml::vector4f(0.8, 0.8, 0.8, 1))+specColor;
  
  //if (gllighting and glcolormaterial)
	return elementwise_mult(light + cml::vector4f(0.2, 0.2, 0.2, 0), color)+specColor;
  
  //return newcolor;
}

/**
 */
cml::vector4f shootRay(Ray viewRay)
{
  cml::vector4f output(0.0f, 0.0f, 0.0f, 1.0f); 
  float coef = 1.0f;
  int level = 0;
  do 
  {
	Sphere currentSphere = getClosestHitObject(viewRay);
	if (currentSphere.getSize() <= 0)
		break;
	
	cml::vector4f ptHitPoint  = viewRay.getStart() + currentSphere.getCurDist() * viewRay.getDirection();
		
	cml::vector4f vNormal = ptHitPoint - currentSphere.getPosition();
	vNormal = vNormal.normalize();	//Surface normal

	Ray lightRay;
	lightRay.setStart(ptHitPoint);
	
	vector<Light> lights = scene.getLights();
	cml::vector4f intensity(0,0,0,0);
	for (int j = 0; j < lights.size(); j++)
	{
	  Light currentLight = lights[j];
	  
	  lightRay.setDirection(currentLight.getPosition() - ptHitPoint);
	  float fLightProjection = cml::dot(lightRay.getDirection(), vNormal);
	  
	  if ( fLightProjection <= 0.0f )
		continue;
	  
	  float lightDist = cml::dot(lightRay.getDirection(), lightRay.getDirection());
	  
	  lightRay.setDirection(lightRay.getDirection().normalize());

	  if (!inShadow(lightRay, lightDist))
	  {
		intensity += max(0.0, (double)cml::dot(vNormal, lightRay.getDirection())) //TODO check this
				  * currentLight.getDiffuse();
	  }
	}
	output += applyLightToColor(intensity, currentSphere.getDiffuse(), cml::vector4f(0,0,0,0));
	
	coef *= currentSphere.getReflection();
	float reflect = 2.0f * cml::dot(viewRay.getDirection(), vNormal);
	viewRay.setStart(ptHitPoint);
	viewRay.setDirection(viewRay.getDirection() - reflect * vNormal);
	level++;
  } while ((coef > 0.0f) && (level < 10));
  
  return output;
}

/**
 */
void setPixel(int x, int y, cml::vector4f color)
{ 
  //if (isInScreen(p) && isInViewport(p) && checkzbuffer(p))
  {
	raster[((y*SCREENWIDTH) + x)*3 + 0] = color[0];
	raster[((y*SCREENWIDTH) + x)*3 + 1] = color[1];
	raster[((y*SCREENWIDTH) + x)*3 + 2] = color[2];
  }
}

/**
 */
void trace()
{
  for (int y=0; y < SCREENHEIGHT; y++)
  {
	for (int x=0; x < SCREENWIDTH; x++)
	{
	  cml::vector4f output(0.0f, 0.0f, 0.0f, 1.0f);
	  //for (float fragmentx = float(x); fragmentx < x + 1.0f; fragmentx += 0.5f)
	  //for (float fragmenty = float(y); fragmenty < y + 1.0f; fragmenty += 0.5f)
	  {
		  //float sampleRatio=0.25f;
		  
		  //TODO shoot rays from camera
		  //Ray viewRay(cml::vector4f(fragmentx, fragmenty, -1000.0f, 1), cml::vector4f(0.0f, 0.0f, 1.0f, 1));
		  Ray viewRay(cml::vector4f(x, y, -1000.0f, 1), cml::vector4f(0.0f, 0.0f, 1.0f, 1));
		  cml::vector4f temp = shootRay(viewRay);
		  
		  // pseudo photo exposure
		  //float exposure = -1.00f; // random exposure value. TODO : determine a good value automatically
		  //temp[0] = (1.0f - expf(temp[0] * exposure));
		  //temp[1] =  (1.0f - expf(temp[1] * exposure));
		  //temp[2] = (1.0f - expf(temp[2] * exposure));

		  //output += sampleRatio * temp;
		  output += temp;
	  }
	  // gamma correction
	  //output[0] = srgbEncode(output[0]);
	  //output[1] = srgbEncode(output[1]);
	  //output[2] = srgbEncode(output[2]);

	  //TODO set the raster
	  setPixel(x, y, output);
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

