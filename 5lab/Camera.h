

#ifndef __CAMERA_H
#define __CAMERA_H

#include <cml/cml.h>

class Camera
{
private:
    cml::vector4f position;
    cml::vector4f direction;
    cml::vector4f up;
    cml::vector4f right;
    
public:
    Camera(){return;};
    
    Camera(cml::vector4f p, cml::vector4f lookat, cml::vector4f u)
    {
        u = u.normalize();
        position = p;
        direction = (lookat - p).normalize();
        cml::vector3f tmp;
        tmp = cml::cross(cml::vector3f(direction.data()), cml::vector3f(u.data()));
        right.set(tmp[0], tmp[1], tmp[2], 0);
        right = right.normalize();
        tmp = cml::cross(cml::vector3f(right.data()), cml::vector3f(direction.data()));
        up.set(tmp[0], tmp[1], tmp[2], 0);
        up = up.normalize();
    }
    
    ~Camera(){return;};
    
    cml::vector4f getPosition(){return position;};
    cml::vector4f getDirection(){return direction;};
    cml::vector4f getRight(){return right;};
    cml::vector4f getUp(){return up;};
    
    void setPosition(cml::vector4f p){this->position = p;};
    void setDirection(cml::vector4f d){this->direction = d;};
    void setRight(cml::vector4f r){this->right = r;};
    void setUp(cml::vector4f u){this->up = u;};
};

#endif