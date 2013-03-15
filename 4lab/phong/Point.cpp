
#include "Point.h"

/**
 * Default Constructor
 */
Point::Point()
{
    this->x = -1;
    this->y = -1;
    this->z = 0;
    this->w = 1;
    this->color.set(1,1,1,1);
    this->normal.set(1,1,1,0);
    //this->specularColor.set(0,0,0,0);
    //this->shininess = 0;
}

/**
 * Constructor
 */
Point::Point(int x, int y, float z, float w, Color c, cml::vector4f p, Normal n/*, cml::vector4f sc, float s*/)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    this->color.set(c[0], c[1], c[2], c[3]);
    this->world.set(p[0], p[1], p[2], p[3]);
    this->normal.set(n[0], n[1], n[2], n[3]);
    //this->specularColor.set(sc[0], sc[1], sc[2], sc[3]);
    //this->shininess = s;
}

/**
 * Destructor
 */
Point::~Point()
{
    return;
}

/**
 * Sets the color based on r,g,b
 */
void Point::setColor(float r, float g, float b, float a)
{
    this->color.set(r,g,b,a);
}

/**
 * Sets the color to the defined Color
 */
void Point::setColor(Color c)
{
    this->color.set(c[0], c[1], c[2], c[3]);
}

void Point::setCoordinates(int x, int y, float z=0, float w=1)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

void Point::set_x(int x)
{
    this->x = x;
}

void Point::set_y(int y)
{
    this->y = y;
}

void Point::set_z(float z)
{
    this->z = z;
}

void Point::set_w(float w)
{
    this->w = w;
}

Color Point::getColor()
{
    return this->color;
}

bool Point::operator<(const Point& p2)
{
    return this->y < p2.y;
}