
#include "bkgl.h"

//TODO make a point class
bool pointCompareY(Point p1, Point p2) {return (p1[1] < p2[1]);}


/**
 * Constructor
 */
bkgl::bkgl()
{
  drawMode = 0;
  lineWidth = 0;
  pointMode -1;
  firstPoint(-1,-1);
  
  curColor = Color(1,1,1,1);
  clearColor = Color(1,1,1,1);
  
  Matrix I = Matrix( 1, 0, 0, 0,
					 0, 1, 0, 0,
					 0, 0, 1, 0,
					 0, 0, 0, 1 );
  
  viewport = cml::vector4i(0,0,SCREENWIDTH,SCREENHEIGHT);
  
  modelViewStack.push(I);
  projectionStack.push(I);
}

/**
 * Destructor
 */
bkgl::~bkgl()
{
  return;
}

/**
 * Returns true if x,y is in the screen space
 */
bool bkgl::isInScreen(int x, int y)
{
  return x>=0 && x<SCREENWIDTH && y>=0 && y<SCREENHEIGHT;
}

/**
 * Returns true if x,y is in the viewport
 */
bool bkgl::isInViewport(int x, int y)
{
  return x>=viewport[0] && x<viewport[2] && y>=viewport[1] && y<viewport[3];
}

/**
 * Set pixle color r,g,b in raster for x,y
 */
void bkgl::setPixel(int x, int y, float r, float g, float b)
{
  if (isInScreen(x,y) && isInViewport(x,y))
  {
	raster[((y*SCREENWIDTH) + x)*3 + 0] = r;
	raster[((y*SCREENWIDTH) + x)*3 + 1] = g;
	raster[((y*SCREENWIDTH) + x)*3 + 2] = b;
  }
}

/**
 * Gets the color of the pixel specified by x,y
 */
Color bkgl::getPixelColor(int x, int y)
{
  //TODO make one for point
  Color c = clearColor;
  if (isInScreen(x,y) && isInViewport(x,y))
  {
	c[0] = raster[((y*SCREENWIDTH) + x)*3 + 0];
	c[1] = raster[((y*SCREENWIDTH) + x)*3 + 1];
	c[2] = raster[((y*SCREENWIDTH) + x)*3 + 2];
  }
  
  return c;
}

/**
 * Saves n points for drawing 
 * return true when n or more points are saved.
 */
bool bkgl::savePoints(int x, int y, int n)
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
 * Draws a line with interpolated colors
 */
Line bkgl::drawLine(int x1, int y1, int x2, int y2)
{
  Line line;
  
  float dy = y2 - y1;
  float dx = x2 - x1;
  float m = dy / dx;			// slope
  float b = y1 - m*(float)x1;	// intercept
  
  Color cStart = getPixelColor(x1, y1);
  Color cEnd = getPixelColor(x2, y2);
  
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
void bkgl::fillPolygon(Line l)
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
	  curY = p[1];
	  curXmin = p[0];
	  curXmax = p[0];
	}
  }
  drawLine(curXmin, curY, curXmax, curY);	// Just in case no change in y
}

/**
 * Draws a smooth shaded triangle
 */
void bkgl::drawTriangle(Point p1, Point p2, Point p3)
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
void bkgl::drawQuad(Point p1, Point p2, Point p3, Point p4)
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
void bkgl::drawStrip(int x, int y)
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
  
  //TODO check for GL_DEPTH_BUFFER_BIT,
  //TODO and will clear only the current viewport, rather than the whole screen.
  
  //TODO what is this bit?!?!?!
  for (int x=0; x<SCREENWIDTH; x++)
	for (int y=0; y<SCREENHEIGHT; y++)
	  setPixel(x,y, clearColor[0], clearColor[1], clearColor[2]);
  return;
}

/**
 * This tells you how to interpret points.
 * with parameters GL_POINTS, GL_LINES, and GL_TRIANGLES
 */
void blgl::bkBegin(GLenum mode)
{
  pointMode = mode;
  firstPoint.set(-1, -1);
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
	  drawStrip(firstPoint[0], firstPoint[1]);
	  break;
	default:
	  break;
  }
  pointMode = -1;
  clearSavedPoints();
  //TODO clear more stuff
}

/**
 *  enable various capabilities
 */
void bkgl::bkEnable(GLenum cap)
{
  //TODO
  return;
}

/**
 * disable various capabilities
 */
void bkgl::bkDisable(GLenum cap)
{
  //TDOO
  return;
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
  //cerr << "\n************************bk_glMatrixMode";
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
  }
}

/**
 *  Replaces the current matrix with the identity matrix.
 *  It is semantically equivalent to calling glLoadMatrix
 *  with the identity matrix but in some cases it is more efficient.
 */
void bkgl::bkLoadIdentity()
{
  bk_glPopMatrix();
  curMatrixStack->push(I);
}

/**
 * Replaces the current matrix with the one whose elements are specified by m.
 * The current matrix is the projection matrix, or modelview matrix,
 * depending on the current matrix mode
 */
void bkgl::bkLoadMatrixd(const double* m)
{
  //TODO fix pop/push to be loadmatrix?
  Matrix M = createMatrix(m);
  curMatrixStack->pop();
  curMatrixStack->push(M);
}

/**
 * Multiplies the current matrix with the one specified using m,
 * and replaces the current matrix with the product.
 */
void bkgl::bkMultMatrixd(const double* m)
{
  //TODO fix pop/push to be loadmatrix?
  Matrix M = createMatrix(m);
  Matrix C = curMatrixStack->top();
  curMatrixStack->pop();
  curMatrixStack->push(C*M);
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
  //TODO move this set pixle
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
 * Specifies the 4-vector x,y,z,w
 */
void bkgl::bkVertex4f(float x, float y, float z, float w)
{
  Matrix P = projectionStack.top();
  Matrix M = modelViewStack.top();
  
  cml::vector4f world(x,y,z,w);
  cml::vector4f tmp = P*M*world;
  tmp = (1/tmp[3])*tmp;
  
  double newX = (((tmp[0]+1)/2.0)*viewport[2])+viewport[0];
  double newY = (((tmp[1]+1)/2.0)*viewport[3])+viewport[1];
  
  bk_glVertex2i((int)(newX+0.5), (int)(newY+0.5));
  
  return;
}

/**
 * Specifies the 4-vector x,y,z,1
 */
void bkgl::bkVertex3f(float x, float y, float z)
{
  bk_glVertex4f(x,y,z,1);
  return;
}

/**
 * Specifies the 3-vector x,y,0
 */
void bkgl::bkVertex2f(float x, float y)
{
  bk_glVertex3f(x,y,0);
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
  double c = cos(angle);
  double s = sin(angle);
  
  cml::vector3f v(x,y,z);
  
  double norm = sqrt(cml::dot(v,v));
  cout << "The Norm is: " << norm << endl;
  if (norm != 1)
  {
	cout << "Normalizing" << endl;
	cml::vector3f n = cml::normalize(v);
	x = n[0];
	y = n[1];
	z = n[2];
  }
  
  Matrix R(	pow(x,2)*(1-c)+c, 	x*y*(1-c)-z*s, 		x*z*(1-c)+y*s,		0,
			y*x*(1-c)+z*s,		pow(y,2)*(1-c)+c,	y*z*(1-c)-x*s,		0,
			x*z*(1-c)-y*s,		y*z*(1-c)+x*s,		pow(z,2)*(1-c)+c,	0,
			0,					0,					0,					1 );
  
  bk_glMultMatrixd(R.data());
  
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
void bkgl::bkTranslatef(float x, float y, float z)
{
  Matrix T(	1,	0,	0,	x,
			0,	1,	0,	y,
			0,	0,	1,	z,
			0,	0,	0,	1 );
  
  bk_glMultMatrixd(T.data());
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
  Matrix S(	x,	0,	0,	0,
			0,	y,	0,	0,
			0,	0,	z,	0,
			0,	0,	0,	1 );
  
  bk_glMultMatrixd(S.data());
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
  
  Matrix O(	2/(right-left),	0,				0,					tx,
			0,				2/(top-bottom),	0,					ty,
			0,				0,				-2/(zFar-zNear),	tz,
			0,				0,				0,					1 );
  
  bk_glMultMatrixd(O.data());
}


