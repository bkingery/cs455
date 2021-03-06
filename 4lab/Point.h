#ifndef BKPOINT_H
#define BKPOINT_H

#include <cml/cml.h>
typedef cml::vector4f Color;
typedef cml::vector4f Normal;
typedef cml::vector4f LightValue;

class Point
{
private:
    
public:
    Color color;
    Normal normal;
    LightValue lightvalue;
    int x, y;
    float z, w;

    Point();
    Point(int x, int y, float z, float w, Color c, Normal n);
    ~Point();
    
    void setLightValue(LightValue l);
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