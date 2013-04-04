

#ifndef __CAMERA_H
#define __CAMERA_H

#include <cml/cml.h>

class Camera
{
private:
    cml::vector4f position;
    cml::vector4f direction;
    
public:
    Camera(){return;};
    Camera(cml::vector4f p, cml::vector4f d): position(p), direction(d) {return;}
    ~Camera(){return;};
    
    cml::vector4f getPosition(){return position;};
    cml::vector4f getDirection(){return direction;};
    
    void setPosition(cml::vector4f p){this->position = p;};
    void setDirection(cml::vector4f d){this->direction = d;};
};

#endif