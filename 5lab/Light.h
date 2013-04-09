

#ifndef __LIGHT_H
#define __LIGHT_H

#include <cml/cml.h>

class Light
{
private:
    cml::vector4f diffuse;
    cml::vector4f ambient;
    cml::vector4f specular;
    cml::vector4f position;
    //cml::vector4f intensity;
    
    bool enabled;
    
public:    
    Light(){return;};
    Light(cml::vector4f p, cml::vector4f d) : position(p), diffuse(d) {return;}
    ~Light(){return;};
    
    void setEnabled(bool e){this->enabled = e;};
    bool isEnabled(){return enabled;};
    
    void setDiffuse(cml::vector4f d){this->diffuse = d;};
    void setAmbient(cml::vector4f a){this->ambient = a;};
    void setPosition(cml::vector4f p){this->position = p;};
    void setSpecular(cml::vector4f s){this->specular = s;};
    
    cml::vector4f getDiffuse(){return diffuse;};
    cml::vector4f getAmbient(){return ambient;};
    cml::vector4f getSpecular(){return specular;};
    cml::vector4f getPosition(){return position;};
};

#endif