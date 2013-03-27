
#include "bkgl.h"


/**
 * Constructor
 */
bkgl::bkgl()
{
  lineWidth = 0;
  pointMode -1;
  
  gldepthTest = false;
  gllighting = false;
  glnormalize = false;
  glcolormaterial = false;
  
  curColor.set(1,1,1,1);
  clearColor.set(1,1,1,1);
  curNormal.set(1,1,1,0);
  curShineColor.set(0,0,0,1);
  
  curShininess = 0;
  
  I.set( 1, 0, 0, 0,
		 0, 1, 0, 0,
		 0, 0, 1, 0,
		 0, 0, 0, 1 );
  
  viewport = cml::vector4i(0,0,SCREENWIDTH,SCREENHEIGHT);
  
  modelViewStack.push(I);
  projectionStack.push(I);
  
  zbuffer_init();
}

/**
 * Destructor
 */
bkgl::~bkgl()
{
  return;
}

void bkgl::zbuffer_init()
{
  for (int i=0; i<SCREENWIDTH*SCREENHEIGHT; i++)
	zbuffer[i] = 1;
}

const float * bkgl::getRaster()
{
  return raster;
}

/**
 * Returns true if x,y is in the screen space
 */
bool bkgl::isInScreen(Point p)
{
  return p.x>=0 && p.x<SCREENWIDTH &&
		 p.y>=0 && p.y<SCREENHEIGHT;
}

/**
 * Returns true if x,y is in the viewport
 */
bool bkgl::isInViewport(Point p)
{
  return p.x>=viewport[0] && p.x<viewport[0]+viewport[2] &&
		 p.y>=viewport[1] && p.y<viewport[1]+viewport[3];
}

/**
 * Returns true if z < zbuffer
 */
bool bkgl::checkzbuffer(Point p)
{
  if (gldepthTest)
	return p.z < zbuffer[(p.y*SCREENWIDTH) + p.x];
  else
	return true;
}

/**
 * Returns true if z is between -1 and 1
 */
bool bkgl::checkzclipping(Point p)
{
  return p.z >= -1 && p.z <= 1;
}

/**
 */
cml::vector4f bkgl::elementwise_mult(const cml::vector4f& v1, const cml::vector4f& v2)
{
  return cml::vector4f(v1[0]*v2[0], v1[1]*v2[1], v1[2]*v2[2], v1[3]*v2[3]);
}

/**
 */
Color bkgl::applyLightToColor(cml::vector4f light, Color color, Color specColor)
{
  Color newcolor = color;
  if (gllighting and !glcolormaterial)
	newcolor = elementwise_mult(light + cml::vector4f(0.2, 0.2, 0.2, 0), cml::vector4f(0.8, 0.8, 0.8, 1))+specColor;
  
  if (gllighting and glcolormaterial)
	newcolor = elementwise_mult(light + cml::vector4f(0.2, 0.2, 0.2, 0), color)+specColor;
  
  return newcolor;
}

/**
 */
cml::vector4f bkgl::calculateIntensity(cml::vector4f p, cml::vector4f normal)
{
  cml::vector4f light(0,0,0,0);
  for (int i=0; i<8; i++)
  {
	Light l = gllights[i];
	if (l.isEnabled())
	{
	  light += l.getAmbient()
				+ max(0.0, (double)cml::dot(normal, (l.getPosition()-p).normalize()))
				* l.getDiffuse();
	}
  }
  
  return light;
}

/**
 */
Color bkgl::calculateSpecularColor(cml:: vector4f p, cml::vector4f normal)
{
  Color specColor(0,0,0,0);
  for (int i=0; i<8; i++)
  {
	Light l = gllights[i];
	if (l.isEnabled())
	{
	  if (cml::dot(normal, (l.getPosition()-p).normalize()) > 0)
	  {
		cml::vector4f halfway = ((l.getPosition()-p).normalize() + cml::vector4f(0,0,1,0)).normalize();
		specColor += max(0.0, (double)pow(cml::dot(normal, halfway), curShininess))
				  * elementwise_mult(curShineColor, l.getSpecular());
	  }
	}
  }
  
  return specColor;
}

/**
 * Set pixle color r,g,b in raster for x,y
 */
void bkgl::setPixel(Point p)
{ 
  if (isInScreen(p) && isInViewport(p) && checkzbuffer(p))
  {
	Color light = calculateIntensity(p.world, p.normal.normalize());
	Color spec  = calculateSpecularColor(p.world, p.normal.normalize());
  
	Color newColor = applyLightToColor(light, p.color, spec);
	
	raster[((p.y*SCREENWIDTH) + p.x)*3 + 0] = newColor[0];
	raster[((p.y*SCREENWIDTH) + p.x)*3 + 1] = newColor[1];
	raster[((p.y*SCREENWIDTH) + p.x)*3 + 2] = newColor[2];
	if (gldepthTest)
	  zbuffer[(p.y*SCREENWIDTH) + p.x] = p.z;
  }
}

/**
 * Saves n points for drawing 
 * return true when n or more points are saved.
 */
bool bkgl::savePoints(Point p, int n)
{
  savedPoints.push_back(p);
  if (savedPoints.size() >= n)
	return true;
  else
	return false;
}

/**
 * Clears the saved points
 */
void bkgl::clearSavedPoints()
{
  savedPoints.clear();
  assert (savedPoints.size() == 0);
}

/**
 * Returns a matrix in row column for based on m
 * precondition: m is size 16
 */
Matrix bkgl::createMatrix(const double* m)
{
  return Matrix(m[0],	m[4],	m[8],	m[12],
				m[1],	m[5],	m[9],	m[13],
				m[2],	m[6],	m[10],	m[14],
				m[3],	m[7],	m[11],	m[15]);
}

/**
 *	Returns the distance between point1 and point2
 */
float bkgl::pointDistance(int x1, int y1, int x2, int y2)
{
  return sqrt(pow((x2-x1),2) + pow((y2-y1), 2));
}

/**
 * Returns the color interpolation based on 2 colors and a fraction
 */
Color bkgl::colorInterpolation(Color COLOR1, Color COLOR2, float fraction)
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
 * Interpolate the z coordinates
 */
float bkgl::zInterpolation(float z1, float z2, float fraction)
{
  fraction = min(fraction, 1.0f);
  fraction = max(fraction, 0.0f);
  return ((z2 - z1) * fraction) + z1;
}

/**
 * Interpolate Vectors
 */
cml::vector4f bkgl::interpolateVectors(cml::vector4f v1, cml::vector4f v2, float fraction)
{
  return ((v2 - v1) * fraction) + v1;
}

/**
 */
Point bkgl::interpolatePoint(int x, int y, Point p1, Point p2, float distance)
{
  Color c = colorInterpolation(p1.getColor(), p2.getColor(), distance);
  float z = zInterpolation(p1.z, p2.z, distance);
  
  cml::vector4f world = interpolateVectors(p1.world, p2.world, distance);
  Normal normal = interpolateVectors(p1.normal, p2.normal, distance);
  
  return Point(x, y, z, 0, c, world, normal);
}

/**
 * Draws a line with interpolated colors
 */
Line bkgl::drawLine(Point p1, Point p2)
{
  int x1 = p1.x;
  int y1 = p1.y;
  int x2 = p2.x;
  int y2 = p2.y;
  
  Line line;
  
  float dy = y2 - y1;
  float dx = x2 - x1;  
  float m = dy / dx;			// slope
  float b = y1 - m*(float)x1;	// intercept
  
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
	  float distance = d1/d2;
	  
	  Point p = interpolatePoint(x1, ys, p1, p2, distance);
	  
	  if (y1 == y2)
		  setPixel(p);
	  else
		  setPixel(p);
	  line.push_back(p); 
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
	  float distance = d1/d2;
	  
	  Point p = interpolatePoint(x1, y1, p1, p2, distance);
	  
	  setPixel(p);
	  line.push_back(p);
	}
  }
  
  //cerr << "Draw Line, Line size: " << line.size() << endl;
  return line;
}

/**
 * Fills a polygon
 * @precondition : l is a list of sorted points (on y) defining the outline
 */
void bkgl::fillPolygon(Line l)
{
  //cerr << "Line size: " << l.size() << endl;
  int curY = l[0].y;
  Point curXmin = l[0];
  Point curXmax = l[0];
  
  for (Line::iterator it = l.begin(); it != l.end(); ++it)
  {
	Point p = *it;
	if (p.y == curY)
	{
	  curXmin = (p.x < curXmin.x) ? p : curXmin;
	  curXmax = (p.x > curXmax.x) ? p : curXmax;
	}
	else
	{
	  drawLine(curXmin, curXmax);
	  curY = p.y;
	  curXmin = p;
	  curXmax = p;
	}
  }
  drawLine(curXmin, curXmax);	// Just in case no change in y
}

/**
 * Draws a smooth shaded triangle
 */
void bkgl::drawTriangle(Point p1, Point p2, Point p3)
{
  Line l1 = drawLine(p1, p2);
  Line l2 = drawLine(p2, p3);
  Line l3 = drawLine(p3, p1);
  
  l1.insert(l1.end(), l2.begin(), l2.end());
  l1.insert(l1.end(), l3.begin(), l3.end());
  
  sort(l1.begin(), l1.end(), pointCompareY);
  
  //cerr << "Line size: " << l1.size() << endl;
  
  fillPolygon(l1);
}

/**
 * Draws a quad
 */
void bkgl::drawQuad(Point p1, Point p2, Point p3, Point p4)
{
  Line l1 = drawLine(p1, p2);
  Line l2 = drawLine(p2, p3);
  Line l3 = drawLine(p3, p4);
  Line l4 = drawLine(p4, p1);
  
  l1.insert(l1.begin(), l2.begin(), l2.end());
  l1.insert(l1.begin(), l3.begin(), l3.end());
  l1.insert(l1.begin(), l4.begin(), l4.end());
  
  sort(l1.begin(), l1.end(), pointCompareY);
  
  //cerr << "Line size: " << l1.size() << endl;
  fillPolygon(l1);
}

/**
 * Draws a line strip and saves the last point
 */
void bkgl::drawStrip(Point p)
{
  if (firstPoint.x == -1)
	firstPoint.setCoordinates(p.x, p.y, p.z, p.w);
	
  if (savePoints(p,2))
  {
	Point p1 = savedPoints[0];
	Point p2 = savedPoints[1];
	drawLine(p1, p2);
	clearSavedPoints();
	savePoints(p,2);
  }
}

/**
 * Sets the line width
 */
void bkgl::bkLineWidth(int w)
{
  lineWidth = w;
}

/**
 * Sets the clear color.
 * 
 */
void bkgl::bkClearColor(float r, float g, float b, float a)
{
  clearColor.set(r,g,b,a);
}

/**
 * Clears the entire screen to the clear color.
 */
void bkgl::bkClear(GLint bit)
{
  Point p;
  if (bit & GL_DEPTH_BUFFER_BIT)
  {
	p.set_z(1);
	zbuffer_init();
  }
  
  p.setColor(clearColor);
  
  for (int x=0; x<SCREENWIDTH; x++)
	for (int y=0; y<SCREENHEIGHT; y++)
	{
	  p.set_x(x);
	  p.set_y(y);
	  setPixel(p);
	}
}

/**
 * This tells you how to interpret points.
 * with parameters GL_POINTS, GL_LINES, and GL_TRIANGLES
 */
void bkgl::bkBegin(GLenum mode)
{
  pointMode = mode;
  firstPoint.setCoordinates(-1, -1, 0, 1);
  clearSavedPoints();
}

/**
 * There must be one glEnd for every glBegin.
 * glVertex only works between Begin & End pairs.
 */
void bkgl::bkEnd()
{
  switch (pointMode)
  {
	case GL_LINE_LOOP:
	  drawStrip(firstPoint);
	  break;
	default:
	  break;
  }
  pointMode = -1;
  clearSavedPoints();
}

/**
 *  enable various capabilities
 */
void bkgl::bkEnable(GLenum cap)
{
  switch (cap)
  {
	case GL_DEPTH_TEST:
	  gldepthTest = true;
	  break;
	case GL_LIGHTING:
	  gllighting = true;
	  break;
	case GL_NORMALIZE:
	  glnormalize = true;
	  break;
	case GL_COLOR_MATERIAL:
	  glcolormaterial = true;
	  break;
	case GL_LIGHT0:
	case GL_LIGHT1:
	case GL_LIGHT2:
	case GL_LIGHT3:
	case GL_LIGHT4:
	case GL_LIGHT5:
	case GL_LIGHT6:
	case GL_LIGHT7:
	  gllights[cap - GL_LIGHT0].setEnabled(true);
	  break;
	default:
	  break;
  }
}

/**
 * disable various capabilities
 */
void bkgl::bkDisable(GLenum cap)
{
  switch (cap)
  {
	case GL_DEPTH_TEST:
	  gldepthTest = false;
	  break;
	case GL_LIGHTING:
	  gllighting = false;
	  break;
	case GL_NORMALIZE:
	  glnormalize = false;
	  break;
	case GL_COLOR_MATERIAL:
	  glcolormaterial = false;
	  break;
	case GL_LIGHT0:
	case GL_LIGHT1:
	case GL_LIGHT2:
	case GL_LIGHT3:
	case GL_LIGHT4:
	case GL_LIGHT5:
	case GL_LIGHT6:
	case GL_LIGHT7:
	  gllights[cap - GL_LIGHT0].setEnabled(false);
	  break;
	default:
	  break;
  }
}

/**
 * Specifies the active viewport -- that is, the rectangle of pixels OpenGL should render to
 */
void bkgl::bkViewport(int x, int y, int width, int height)
{
  viewport.set(x,y,width,height);
  return;
}

/**
 * Sets the current matrix stack
 * Implemented for GL_MODELVIEW and GL_PROJECTION
 */
void bkgl::bkMatrixMode(GLenum e)
{
  //cerr << "\n************************bkMatrixMode";
  switch (e)
  {
	case GL_MODELVIEW:
	  //cerr << "Setting stack to modelview" << endl;
	  curMatrixStack = &modelViewStack;
	  break;
	case GL_PROJECTION:
	  //cerr << "Setting stack to projection" << endl;
	  curMatrixStack = &projectionStack;
	  break;
	default:
	  break;
  }
}

/**
 * 
 */
void bkgl::updateInverseTransposeModelview()
{
  if (curMatrixStack == &modelViewStack)
  {
	Matrix M = curMatrixStack->top();
	inverseTransposeModelview = M.transpose().inverse();
  }
}

/**
 * Pushes the current matrix stack down by one, duplicating the current matrix.
 * That is, after a glPushMatrix call, the matrix on top of the stack
 * is identical to the one below it.
 */
void bkgl::bkPushMatrix()
{
  Matrix m = curMatrixStack->top();
  curMatrixStack->push(m);
}

/**
 * Pops the current matrix stack, replacing the current matrix
 * with the one below it on the stack.
 */
void bkgl::bkPopMatrix()
{
  if (curMatrixStack->size() > 1)
  {
	curMatrixStack->pop();
	updateInverseTransposeModelview();
  }
}

/**
 *  Replaces the current matrix with the identity matrix.
 *  It is semantically equivalent to calling glLoadMatrix
 *  with the identity matrix but in some cases it is more efficient.
 */
void bkgl::bkLoadIdentity()
{
  bkPopMatrix();
  curMatrixStack->push(I);
  
  updateInverseTransposeModelview();
}

/**
 * Replaces the current matrix with the one whose elements are specified by m.
 * The current matrix is the projection matrix, or modelview matrix,
 * depending on the current matrix mode
 */
void bkgl::bkLoadMatrixd(const double* m)
{
  Matrix M = createMatrix(m);
  curMatrixStack->pop();
  curMatrixStack->push(M);
  
  updateInverseTransposeModelview();
}

/**
 * Multiplies the current matrix with the one specified using m,
 * and replaces the current matrix with the product.
 */
void bkgl::bkMultMatrixd(const double* m)
{
  Matrix M = createMatrix(m);
  Matrix C = curMatrixStack->top();
  curMatrixStack->pop();
  curMatrixStack->push(C*M);
  
  updateInverseTransposeModelview();
}

void bkgl::drawCurMode(Point p)
{
   //TODO move this set pixle
  //setPixel(p);
  
  switch (pointMode)
  {
	case GL_POINTS:
	  setPixel(p);
	  break;
	case GL_LINES:
	  if (savePoints(p,2))
	  {
		Point p1 = savedPoints[0];
		Point p2 = savedPoints[1];
		drawLine(p1, p2);
		clearSavedPoints();
	  }
	  break;
	case GL_TRIANGLES:
	  if (savePoints(p,3))
	  {
		Point p1 = savedPoints[0];
		Point p2 = savedPoints[1];
		Point p3 = savedPoints[2];
		drawTriangle(p1, p2, p3);
		clearSavedPoints();
	  }
	  break;
	case GL_LINE_STRIP:
	  drawStrip(p);
	  break;
	case GL_LINE_LOOP:
	  drawStrip(p);
	  break;
	case GL_TRIANGLE_STRIP:
	  if (savePoints(p,3))
	  {
		Point p1 = savedPoints[0];
		Point p2 = savedPoints[1];
		Point p3 = savedPoints[2];
		drawTriangle(p1, p2, p3);
		clearSavedPoints();
		savePoints(p2,3);
		savePoints(p3,3);
	  }
	  break;
	case GL_POLYGON:
	case GL_TRIANGLE_FAN:
	  if (savePoints(p,3))
	  {
		Point p1 = savedPoints[0];
		Point p2 = savedPoints[1];
		Point p3 = savedPoints[2];
		drawTriangle(p1, p2, p3);
		clearSavedPoints();
		savePoints(p1,3);
		savePoints(p3,3);
	  }
	  break;
	case GL_QUADS:
	  if (savePoints(p,4))
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
	  if (savePoints(p,4))
	  {
		Point p1 = savedPoints[0];
		Point p2 = savedPoints[1];
		Point p3 = savedPoints[2];
		Point p4 = savedPoints[3];
		drawQuad(p1, p2, p4, p3);
		clearSavedPoints();
		savePoints(p3,3);
		savePoints(p4,3);
	  }
	  break;
	default:
	  //TODO begin must be called
	  break;
  }
  return;
}

/**
 * glVertex specifies a point for drawing, though how it is drawn depends
 * on the mode specified by glBegin.
 * glVertex2i(x,y) specifies the 4-vector point (x,y,0,1).
 *
 * plus possibly other glVertex calls
 */
void bkgl::bkVertex2i(int x, int y)
{
  bkVertex4f(x,y,0,1);
}

/**
 * Specifies the 4-vector x,y,z,w
 * Default value for w is 1
 */
void bkgl::bkVertex4f(float x, float y, float z, float w=1)
{
  Matrix P = projectionStack.top();
  Matrix M = modelViewStack.top();
  
  //Normal
  Normal n = inverseTransposeModelview*curNormal;
  
  cml::vector4f v(x,y,z,w);
  
  //Modelview
  cml::vector4f p = M*v;
  
  //cml::vector4f light = calculateIntensity(p, n);
  //Color specColor = calculateSpcularColor(p, n);
  
  //Projection
  cml::vector4f tmp = P*p;
  
  //Divide by w
  tmp = (1.0/tmp[3])*tmp;
  
  //Size to viewport
  double newX = (((tmp[0]+1)/2.0)*viewport[2])+viewport[0];
  double newY = (((tmp[1]+1)/2.0)*viewport[3])+viewport[1];
  
  //Color newColor = applyLightToColor(light, curColor, specColor);
  
  Point point((int)(newX+0.5), (int)(newY+0.5), tmp[2], tmp[3], curColor, p, n/*, specColor, curShininess*/);
  
  drawCurMode(point);
}

/**
 * Specifies the 4-vector x,y,z,1
 * Default value for z is 0
 */
void bkgl::bkVertex3f(float x, float y, float z=0)
{
  bkVertex4f(x,y,z,1);
  return;
}

/**
 * Specifies the 3-vector x,y,0
 */
void bkgl::bkVertex2f(float x, float y)
{
  bkVertex3f(x,y,0);
  return;
}

/**
 * There is one current color (a 4-vector) at all times in OpenGL.
 * Initially, it is (1,1,1,1) (white). Calling glColor3f(r,g,b) sets it to (r,g,b,1).
 */
void bkgl::bkColor3f(float r, float g, float b)
{
  curColor.set(r,g,b,1);
}

/**
 * produces a rotation of angle degrees around the vector (x, y, z).
  	The current matrix (see glMatrixMode) is multiplied by a rotation
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
void bkgl::bkRotatef(float angle, float x, float y, float z)
{
  double c = cos(PI*angle/180);
  double s = sin(PI*angle/180);
  
  cml::vector3f v(x,y,z);
  
  double norm = sqrt(cml::dot(v,v));
  //cout << "The Norm is: " << norm << endl;
  if (norm != 1)
  {
	//cout << "Normalizing" << endl;
	cml::vector3f n = cml::normalize(v);
	x = n[0];
	y = n[1];
	z = n[2];
  }
  
  Matrix R(	pow(x,2)*(1-c)+c, 	x*y*(1-c)-z*s, 		x*z*(1-c)+y*s,		0,
			y*x*(1-c)+z*s,		pow(y,2)*(1-c)+c,	y*z*(1-c)-x*s,		0,
			x*z*(1-c)-y*s,		y*z*(1-c)+x*s,		pow(z,2)*(1-c)+c,	0,
			0,					0,					0,					1 );
  
  bkMultMatrixd(R.data());
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
void bkgl::bkTranslatef(float x, float y, float z)
{
//  Matrix T(	1,	0,	0,	x,
//			0,	1,	0,	y,
//			0,	0,	1,	z,
//			0,	0,	0,	1 );
  
  double data[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, x, y, z, 1};
  //bkMultMatrixd(T.data());
  bkMultMatrixd(data);
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
void bkgl::bkScalef(float x, float y, float z)
{
//  Matrix S(	x,	0,	0,	0,
//			0,	y,	0,	0,
//			0,	0,	z,	0,
//			0,	0,	0,	1 );
  
  double data[] = {x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1};
  //bkMultMatrixd(S.data());
  bkMultMatrixd(data);
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
void bkgl::bkOrtho(double left, double right, double bottom, double top, double zNear, double zFar)
{
  double tx = - (right + left) / (right - left);
  double ty = - (top + bottom) / (top - bottom);
  double tz = - (zFar + zNear) / (zFar - zNear);
  
//  Matrix O(	2/(right-left),	0,				0,					tx,
//			0,				2/(top-bottom),	0,					ty,
//			0,				0,				-2/(zFar-zNear),	tz,
//			0,				0,				0,					1 );
  
  double data[] = {2/(right-left), 0, 0, 0, 0, 2/(top-bottom), 0, 0, 0, 0, -2/(zFar-zNear), 0, tx, ty, tz, 1};
  
  //bkMultMatrixd(O.data());
  bkMultMatrixd(data);
}

/**
 * Scale such that cx, cy,cz does not move
 */
void bkgl::bkFixedScalef(float sx, float sy, float sz, float cx, float cy, float cz)
{
    Matrix FS(sx,	0,	0,	cx - cx * sx,
			  0, 	sy,	0,	cy - cy * sy, 
			  0, 	0,	sz,	cz,
			  0, 	0,	0,	1);
  
  bkMultMatrixd(FS.data());
}

/**
 * Shearing, where x moves sxy * y, etc
 */
void bkgl::bkShearf(float sxy, float sxz, float syx, float syz, float szx, float szy)
{
  Matrix S(1, 	sxy, 	sxz, 	0,
		   syx,	1,		syz,	0,
		   szx,	szy,	1,		0,
		   0,	0,		0,		1);
  
  bkMultMatrixd(S.data());
}

/**
 */
void bkgl::bkFullRotatef(float ang, float ax, float ay, float az, float bx, float by, float bz)
{
  bkTranslatef(ax, ay, az);
  bkRotatef(ang, bx, by, bz);
  bkTranslatef(-ax, -ay, -az);
}

/**
 */
void bkgl::bkFrustum(double left, double right, double bottom, double top, double zNear, double zFar)
{
  double A = (right + left) / (right - left);
  double B = (top + bottom) / (top - bottom);
  double C = -(zFar + zNear) / (zFar - zNear);
  double D = -(2 * zFar * zNear) / (zFar - zNear);
  
  Matrix F((2*zNear)/(right-left),	0,						A,	0,
		   0,						(2*zNear)/(top-bottom),	B,	0,
		   0,						0,						C,	D,
		   0,						0,						-1,	0);

  
  bkMultMatrixd(F.data());
}

/**
 */
void bkgl::bkuPerspective(double fovy, double aspect, double zNear, double zFar)
{
  double radians = PI*fovy/180;
  double f = cos(radians/2)/sin(radians/2); //cotangent
  
  Matrix P(f/aspect,	0,		0,							0,
		   0,			f,		0,							0,
		   0,			0,		(zFar+zNear)/(zNear-zFar),	(2*zFar*zNear)/(zNear-zFar),
		   0,			0,		-1,							0);
  
  bkMultMatrixd(P.data());
}

/**
 */
void bkgl::bkNormal3f(float nx, float ny, float nz)
{
  curNormal.set(nx, ny, nz, 0);
}

/**
 */
void bkgl::bkLightfv(GLenum light, GLenum pname, const float *params)
{
  int i = light-GL_LIGHT0; //GL_LIGHT0 == 16384
  
  switch (pname)
  {
	case GL_DIFFUSE:
	  gllights[i].setDiffuse(params);
	  break;
	case GL_AMBIENT:
	  gllights[i].setAmbient(params);
	  break;
	case GL_POSITION:
	  gllights[i].setPosition(params);
	  break;
	case GL_SPECULAR:
	  gllights[i].setSpecular(params);
	default:
	  break;
  }
}

/**
 */
void bkgl::bkMaterialf(GLenum face, GLenum pname, float param)
{
  switch (face)
  {
	case GL_FRONT_AND_BACK:
	  curShininess = param;
	  break;
	default:
	  break;
  }
}

void bkgl::bkMaterialfv(GLenum face, GLenum pname, const float *params)
{
  switch (face)
  {
	case GL_FRONT_AND_BACK:
	  curShineColor.set(params[0], params[1], params[2], params[3]);
	  break;
	default:
	  break;
  }
}
















