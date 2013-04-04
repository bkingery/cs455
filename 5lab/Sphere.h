
#ifndef __SPHERE_H
#define __SPHERE_H

#include <cml/cml.h>

#include "Ray.h"

//struct Material
//{
//    Color diffuse;
//    float reflection;
//    Color specular;
//    //float power;
//};

class Sphere
{
private:
    cml::vector4f position;
	float size;
    
	cml::vector4f diffuse;
    cml::vector4f specular;
    float reflection;
    
public:
    Sphere(cml::vector4f p, float s);
    ~Sphere();
    
    void setMaterial(cml::vector4f d, cml::vector4f s, float r);
    
    cml::vector4f getDiffuse();
    cml::vector4f getSpecular();
    float getReflection();
    
    float hit(Ray r);
};

#endif