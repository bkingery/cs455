#ifndef BKPOINT_H
#define BKPOINT_H

#include <cml/cml.h>
typedef cml::vector4f Color;
typedef cml::vector4f Normal;

class Point
{
private:
    
public:
    int x, y;
    float z, w;
    Color color;
    cml::vector4f world;
    Normal normal;
    //cml::vector4f specularColor;
    //float shininess;


    Point();
    Point(int x, int y, float z, float w, Color c, cml::vector4f p, Normal n/*, cml::vector4f sc, float s*/);
    ~Point();
    
    void setColor(Color c);
    void setColor(float r, float g, float b, float a);
    void setCoordinates(int x, int y, float z, float w);
    void set_x(int x);
    void set_y(int y);
    void set_z(float z);
    void set_w(float w);
    
    Color getColor();
    
    bool operator<(const Point& p2);
};

#endif