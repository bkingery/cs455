

#include "Sphere.h"

Sphere::Sphere(cml::vector4f p, float s)
{
    this->position = p;
    this->size = s;
    
    this->diffuse.set(1,0,0,1);
    this->specular.set(1,1,1,1);
    this->reflection = 0.0;
}

Sphere::~Sphere() {return;}

/**
 * Sets the material
 */
void Sphere::setMaterial(cml::vector4f d, cml::vector4f s, float r)
{
    this->diffuse = d;
    this->specular = s;
    this->reflection = r;
}

/**
 * Getters
 */
cml::vector4f Sphere::getDiffuse(){return diffuse;}
cml::vector4f Sphere::getSpecular(){return specular;}
float Sphere::getReflection(){return reflection;}

/**
 * If the Sphere is hit return the distance t
 * else return 0
 */
float Sphere::hit(Ray r)
{
    // Intersection of a ray and a sphere
	cml::vector4f dist = this->position - r.getStart();
	float B = (r.getDirection()[0] * dist[0] + r.getDirection()[1] * dist[1] + r.getDirection()[2] * dist[2]);
	float D = B*B - cml::dot(dist, dist) + this->size * this->size;
	
    if (D < 0.0f)
        return 0;
	
    float t0 = B - sqrtf(D);
	float t1 = B + sqrtf(D);
	if (t0 > 0.1f )
		return t0;
	if (t1 > 0.1f )
		return t1;
	
    return 0;
}