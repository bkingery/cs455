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
using namespace std;

// Globals
#define SCREENHEIGHT 480
#define SCREENWIDTH 640

#define PI (3.141592653589793)



class bkgl
{
private:
    // typedefs
    typedef cml::matrix44d_c Matrix;
    typedef cml::vector4f Color;
    typedef cml::vector2i Point;
    typedef vector<Point> Line;
    
    float raster[SCREENWIDTH*SCREENHEIGHT*3];
    
    int drawMode;
    int lineWidth;
    GLenum pointMode;
    Point firstPoint;
    
    Color curColor;
    Color clearColor;
    std::vector<Point> savedPoints;
    
    std::stack<Matrix> *curMatrixStack;
    std::stack<Matrix> modelViewStack;
    std::stack<Matrix> projectionStack;
    
    Matrix I;
    
    cml::vector4i viewport;
    
public:
    bkgl();
    ~bkgl();
    
    bool isInScreen(int x, int y);
    bool isInViewPort(int x, int y);
    
    void setPixel(int x, int y, float r, float g, float b);
    void getPixelColor(int x, int y);
    
    bool savePoints(int x, int y, int n);
    void clearSavedPoints();
    float pointDistance(int x1, int y1, int x2, int y2);
    
    Color colorInterpolation(Color c1, Color c2, float fraction);
    
    Line drawLine(int x1, int y1, int x2, int y2);
    void drawTriangle(Point p1, Point p2, Point p3);
    void drawQuad(Point p1, Point p2, Point p3, Point p4);
    void drawStrip(int x, int y);
    void fillPolygon(Line l);
    
    Matrix createMatrix(const double* m);
    
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
    void bkTranslatef(flat x, float y, float z);
    void bkScalef(float x, float y, float z);
    void bkOrtho(double left, double right, double bottom, double top, double zNear, double zFar);
    
};

#endif






















