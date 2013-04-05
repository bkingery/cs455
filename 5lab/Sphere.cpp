

#include "Sphere.h"

Sphere::Sphere()
{
	this->size = 0;
}

Sphere::Sphere(cml::vector4f p, float s)
{
    this->position = p;
    this->size = s;
    
    this->diffuse.set(1,0,0,1);
    this->specular.set(1,1,1,1);
    this->reflection = 0.0;
	
	this->curDist = -1;
}

Sphere::~Sphere() {return;}

/**
 * Sets the material
 */
void Sphere::setMaterial(cml::vector4f d, float r, cml::vector4f s)
{
    this->diffuse = d;
    this->specular = s;
    this->reflection = r;
}

/**
 * Getters
 */
cml::vector4f Sphere::getPosition(){return position;}
cml::vector4f Sphere::getDiffuse(){return diffuse;}
cml::vector4f Sphere::getSpecular(){return specular;}
float Sphere::getSize(){return size;}
float Sphere::getReflection(){return reflection;}
float Sphere::getCurDist(){return curDist;}

/**
 * Return true if sphere is hit by ray
 */
bool Sphere::hit(Ray r)
{
    // Intersection of a ray and a sphere
	cml::vector4f dist = this->position - r.getStart();
	float B = (r.getDirection()[0] * dist[0] + r.getDirection()[1] * dist[1] + r.getDirection()[2] * dist[2]);
	float D = B*B - cml::dot(dist, dist) + this->size * this->size;
	
    if (D < 0.0f)
        return false;
	
    float t0 = B - sqrtf(D);
	float t1 = B + sqrtf(D);
	if (t0 > 0.1f )
	{
		this->curDist = t0;
		return true;
	}
	if (t1 > 0.1f )
	{
		this->curDist = t1;
		return true;
	}
	
    return false;
}