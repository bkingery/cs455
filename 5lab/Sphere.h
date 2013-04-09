
#ifndef __SPHERE_H
#define __SPHERE_H

#include <cml/cml.h>
#include <limits>

#include "Ray.h"

class Sphere
{
private:
    cml::vector4f position;
	float size;
    
	cml::vector4f diffuse;
    cml::vector4f specular;
    float reflection;
    
    float curDist;
    
public:
    Sphere();
    Sphere(cml::vector4f p, float s);
    ~Sphere();
    
    void setMaterial(cml::vector4f d, float r, cml::vector4f s);
    
    cml::vector4f getPosition();
    
    cml::vector4f getDiffuse();
    cml::vector4f getSpecular();
    float getSize();
    float getReflection();
    
    float getCurDist();
    
    bool hit(Ray r);
};

#endif