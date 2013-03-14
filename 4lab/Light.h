#ifndef BKLIGHT_H
#define BKLIGHT_H

#include <cml/cml.h>
typedef cml::vector4f Color;
//typedef cml::vector4f Position;

class Light
{
private:
    Color diffuse;
    Color ambient;
    //Position position;
    cml::vector4f position;
    
    bool enabled;
    
public:    
    Light();
    ~Light();
    
    void setEnabled(bool e);
    bool isEnabled(){return enabled;};
    
    void setDiffuse(const float *d);
    void setAmbient(const float *a);
    void setPosition(const float *p);
    
    Color getDiffuse(){return diffuse;};
    Color getAmbient(){return ambient;};
    //Position getPosition(){return position;};
    cml::vector4f getPosition(){return position;};
};

#endif