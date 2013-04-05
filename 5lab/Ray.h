

#ifndef __RAY_H
#define __RAY_H

#include <cml/cml.h>

class Ray
{
private:
    cml::vector4f start;
    cml::vector4f direction;
    
public:
    Ray(){return;};
    Ray(cml::vector4f s, cml::vector4f d) : start(s), direction(d) {return;}
    ~Ray(){return;};
    
    cml::vector4f getStart(){return start;};
    cml::vector4f getDirection(){return direction;};
    
    void setStart(cml::vector4f s){this->start = s;};
    void setDirection(cml::vector4f d){this->direction = d;};
    
};

#endif