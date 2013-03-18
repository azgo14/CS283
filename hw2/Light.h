#ifndef LIGHT_H
#define LIGHT_H

#include "Triangle.h"
#include <glm/glm.hpp>
#include <iostream>


typedef glm::vec3 vec3;

class Light : public Triangle {
public:
    Light(vec3 vert1, vec3 vert2, vec3 vert3) ;
    virtual ~Light() {};
    virtual vec4 getLightPosn() = 0;
};

//can only be rectangular areas
class AreaLight : public Light {
public:
    AreaLight(vec3 vert1, vec3 vert2, vec3 vert3) ;
    virtual ~AreaLight() {};
    vec4 getLightPosn();
};

#endif
