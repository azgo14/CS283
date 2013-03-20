#include "Light.h"
#include <iostream>
#include <stdlib.h>

Light::Light(vec3 vert1, vec3 vert2, vec3 vert3) : Triangle(vert1, vert2, vert3) {
    isLight = true;
}

AreaLight::AreaLight(vec3 vert1, vec3 vert2, vec3 vert3) : Light(vert1, vert2, vert3) {
    vec3 inter_area = glm::cross(vert2 - vert1, vert3 - vert1);
    _area = sqrt(glm::dot(inter_area,inter_area)) / 2; 
}

namespace {
bool pointInside(vec3 point1, vec3 point2, vec3 vert1, vec3 vert2) {
    vec3 result1 = glm::cross(vert2 - vert1, point1 - vert1);
    vec3 result2 = glm::cross(vert2 - vert1, point2 - vert1);
    if (glm::dot(result1, result2) >= 0) {
        return true;
    }
    //std::cout<<"Not in triangle: " << point1[0] << " " << point1[1] << " " << point1[2] << std::endl;
    return false;
}
bool inArea(vec3 point, vec3 vert1, vec3 vert2, vec3 vert3) {
    if (pointInside(point, vert1, vert2, vert3) &&
        pointInside(point, vert2, vert1, vert3) &&
        pointInside(point, vert3, vert1, vert2)) {
        return true;
    }
    return false;
}
} // namespace
vec4 AreaLight::getLightPosn() {
    if (!transformed) {
        calculateTransform();
    }
    vec3 point;
    do {
        float u1 = rand() / static_cast<float>(RAND_MAX);
        float u2 = rand() / static_cast<float>(RAND_MAX);
        //std::cout<<"In getLightPos - u1: " << u1 <<std::endl;
        //std::cout<<"In getLightPos - u2: " << u2 <<std::endl;
        point = _t_vertices[0] + u1 * (_t_vertices[1] - _t_vertices[0]) + u2 * (_t_vertices[2] - _t_vertices[1]);
    } while (!inArea(point, _t_vertices[0], _t_vertices[1], _t_vertices[2]));
    return vec4(point,1);
}

vec4 AreaLight::getColor(const vec3& normal, const vec3& dir_to_light, float distance) {
    if (!transformed) {
        calculateTransform();
    }
    float area_n_dot_l = glm::dot(_t_normals[0], -dir_to_light);
    if (area_n_dot_l < 0) {
        area_n_dot_l = 0;
    }
    float n_dot_l = glm::dot(normal, dir_to_light);
    if (n_dot_l < 0) {
        n_dot_l = 0;
    }
    return (_emission * n_dot_l * area_n_dot_l * _area ) / (distance * distance);
}