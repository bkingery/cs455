
#include "Light.h"

Light::Light()
{
    enabled = false;
    diffuse.set(0,0,0,0);
    ambient.set(0,0,0,1);
    position.set(0,0,0,1);
    specular.set(0,0,0,0);
}

Light::~Light()
{
    return;
}

void Light::setEnabled(bool e)
{
    this->enabled = e;
}

void Light::setDiffuse(const float *d)
{
    this->diffuse.set(d[0], d[1], d[2], d[3]);
}

void Light::setAmbient(const float *a)
{
    this->ambient.set(a[0], a[1], a[2], a[3]);
}

void Light::setPosition(const float *p)
{
    this->position.set(p[0], p[1], p[2], p[3]);
}

void Light::setSpecular(const float *s)
{
    this->position.set(s[0], s[1], s[2], s[3]);
}