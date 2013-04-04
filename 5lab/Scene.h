

#ifndef __SCENE_H
#define __SCENE_H

#include <vector>

#include "Sphere.h"
#include "Light.h"
#include "Camera.h"

class Scene
{
private:
    std::vector<Sphere> objects;
    std::vector<Light> lights;
    Camera camera;
    
public:
    Scene(){return;};
    ~Scene(){return;};
    
    std::vector<Sphere> getObjects() {return objects;};
    std::vector<Light> getLights() {return lights;};
    
    void addObject(Sphere s) {objects.push_back(s);};
    void addLight(Light l) {lights.push_back(l);};
    
    void clearObjects(){objects.clear();};
    void clearLights(){lights.clear();};
    
    Camera getCamera() {return camera;};
    void setCamera(Camera c){this->camera = c;};
};

#endif