#ifndef BKGL_H
#define BKGL_H

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <GL/glut.h>   		// The GL Utility Toolkit (Glut) Header
#include <cml/cml.h>
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

#include <assert.h>

#include "Point.h"
#include "Light.h"
using namespace std;

// Globals
#define SCREENHEIGHT 480
#define SCREENWIDTH 640

#define PI (3.141592653589793)

// typedefs
typedef cml::matrix44d_c Matrix;
typedef cml::vector4f Color;
typedef cml::vector4f Normal;
typedef vector<Point> Line;

class bkgl
{
private:    
    float raster[SCREENWIDTH*SCREENHEIGHT*3];
    float zbuffer[SCREENWIDTH*SCREENHEIGHT];
    
    Light gllights[8];
    
    bool gldepthTest, gllighting, glnormalize, glcolormaterial;
    
    int drawMode;
    int lineWidth;
    GLenum pointMode;
    Point firstPoint;
    
    Color curColor;
    Color clearColor;
    Color curShineColor;
    
    float curShininess;
    
    Normal curNormal;
    Matrix inverseTransposeModelview;
    
    std::vector<Point> savedPoints;
    
    std::stack<Matrix> *curMatrixStack;
    std::stack<Matrix> modelViewStack;
    std::stack<Matrix> projectionStack;
    
    Matrix I;
    
    cml::vector4i viewport;
    
    void zbuffer_init(void);
    
    bool isInScreen(Point p);
    bool isInViewport(Point p);
    bool checkzbuffer(Point p);
    bool checkzclipping(Point p);
    
    void setPixel(int x, int y, float r, float g, float b);
    void setPixel(Point p);
    
    bool savePoints(Point p, int n);
    void clearSavedPoints();
    float pointDistance(int, int, int, int);
    
    Color colorInterpolation(Color c1, Color c2, float fraction);
    float zInterpolation(float z1, float z2, float fraction);
    cml::vector4f interpolateVectors(cml::vector4f v1, cml::vector4f v2, float fraction);
    Point interpolatePoint(int x, int y, Point start, Point end, float distance);
    
    void drawCurMode(Point p);
    Line drawLine(Point p1, Point p2);
    void drawTriangle(Point p1, Point p2, Point p3);
    void drawQuad(Point p1, Point p2, Point p3, Point p4);
    void drawStrip(Point p);
    void fillPolygon(Line l);
    
    Matrix createMatrix(const double* m);
    
    void updateInverseTransposeModelview();
    
    cml::vector4f elementwise_mult(const cml::vector4f& v1, const cml::vector4f& v2);
    Color applyLightToColor(cml::vector4f light, Color color, Color specColor);
    Color calculateIntensity(cml::vector4f p, cml::vector4f normal);
    Color calculateSpecularColor(cml:: vector4f p, cml::vector4f normal);
    
public:
    bkgl();
    ~bkgl();
    
    //TODO make a point class
    static bool pointCompareY(Point p1, Point p2) {return (p1 < p2);}
    
    const float * getRaster(void);
    
    void bkLineWidth(int w);
    void bkClearColor(float r, float g, float b, float a);
    void bkColor3f(float r, float g, float b);
    
    void bkClear(GLint bit);
    void bkBegin(GLenum mode);
    void bkEnd();
    void bkEnable(GLenum cap);
    void bkDisable(GLenum cap);
    void bkViewport(int x, int y, int width, int height);
    
    void bkMatrixMode(GLenum e);
    void bkPushMatrix();
    void bkPopMatrix();
    void bkLoadIdentity();
    void bkLoadMatrixd(const double* m);
    void bkMultMatrixd(const double* m);
    
    void bkVertex2i(int x, int y);
    void bkVertex4f(float x, float y, float z, float w);
    void bkVertex3f(float x, float y, float z);
    void bkVertex2f(float x, float y);
    
    void bkRotatef(float angle, float x, float y, float z);
    void bkTranslatef(float x, float y, float z);
    void bkScalef(float x, float y, float z);
    void bkOrtho(double left, double right, double bottom, double top, double zNear, double zFar);
    
    void bkFixedScalef(float sx, float sy, float sz, float cx, float cy, float cz);
    void bkShearf(float sxy, float sxz, float syx, float syz, float szx, float szy);
    void bkFullRotatef(float ang, float ax, float ay, float az, float bx, float by, float bz);
    
    void bkFrustum(double left, double right, double bottom, double top, double near, double far);
    void bkuPerspective(double fovy, double aspect, double near, double far);
    
    void bkNormal3f(float nx, float ny, float nz);
    void bkLightfv(GLenum light, GLenum pname, const float *params);
    void bkMaterialf(GLenum face, GLenum pname, float param);
    void bkMaterialfv(GLenum face, GLenum pname, const float *params);
    
};

#endif






















