#include "Triangle.h"
#include <iostream>

Triangle::Triangle(vec3 vert1, vec3 vert2, vec3 vert3, vec3 norm1, vec3 norm2, vec3 norm3) {
    _vertices[0] = vert1;
    _vertices[1] = vert2;
    _vertices[2] = vert3;
    _normals[0] = norm1;
    _normals[1] = norm2;
    _normals[2] = norm3;
    _type = trinorm;
    isLight = false;    
}
Triangle::Triangle(vec3 vert1, vec3 vert2, vec3 vert3) {
    _vertices[0] = vert1;
    _vertices[1] = vert2;
    _vertices[2] = vert3;
    _type = tri;
    _normals[0] = glm::normalize(glm::cross(vert2 - vert1, vert3 - vert1));
    _normals[1] = _normals[0];
    _normals[2] = _normals[0];
    isLight = false;    
}

// Note that this way of doing transformations is not the same as in lecture but this way is faster ONLY when we generate pictures. Can't handle animation because we calculate only once.
void Triangle::calculateTransform(void) {
    if (!transformed) {
        for (int i = 0; i < 3; ++i) {
            glm::vec3 point = _vertices[i];
            glm::vec4 homo_vec = glm::vec4(point[0], point[1], point[2], 1);
            homo_vec = homo_vec * transform;
            _t_vertices[i] = glm::vec3(homo_vec[0] / homo_vec[3], homo_vec[1] / homo_vec[3], homo_vec[2] / homo_vec[3]);
        }
        glm::mat4 inverse_transpose = glm::transpose(glm::inverse(transform));
        for (int i = 0; i < 3; ++i) {
            glm::vec3 normal = _normals[i];
            glm::vec4 homo_norm = glm::vec4(normal[0], normal[1], normal[2], 0);
            homo_norm = homo_norm * inverse_transpose;
            _t_normals[i] = glm::normalize(glm::vec3(homo_norm[0], homo_norm[1], homo_norm[2]));
        }
        transformed = true;
    }
}

std::pair<bool,vec3> Triangle::intersect(const vec3& origin, const vec3& direction) {
    calculateTransform();
    vec3 p0 = _t_vertices[0], p1 = _t_vertices[1], p2 = _t_vertices[2];
    vec3 normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));
    GLfloat t = glm::dot((p0 - origin), normal)/glm::dot(direction, normal);
    if (t >= 0) {
        vec3 ray = origin + direction * t;
        bool hit = glm::dot(glm::cross(p1 - p0, ray - p0), normal) >= 0 &&
                   glm::dot(glm::cross(p2 - p1, ray - p1), normal) >= 0 &&
                   glm::dot(glm::cross(p0 - p2, ray - p2), normal) >= 0;
        return std::make_pair(hit, ray);
    } else {
        return std::make_pair(false, direction);
    }
}

vec3 Triangle::getNormal(const vec3& intersect) {
    if (!transformed) {
        calculateTransform();
    }
    vec3 normal;
    if (_type == tri) {
        normal = _t_normals[0];
    } else {
        vec3 temp1 = (_t_normals[0] * (intersect.y - _t_vertices[1].y) + _t_normals[1] * (_t_vertices[0].y - intersect.y))/(_t_vertices[0].y - _t_vertices[1].y);
        vec3 temp2 = (_t_normals[0] * (intersect.y - _t_vertices[2].y) + _t_normals[2] * (_t_vertices[0].y - intersect.y))/(_t_vertices[0].y - _t_vertices[2].y);
        normal = (temp1 * (temp2.x - intersect.x) + temp2 * (intersect.x - temp1.x))/(temp2.x - temp1.x);
        vec4 homo_normal = glm::normalize(vec4(normal.x, normal.y, normal.z, 0) * glm::transpose(glm::inverse(transform)));
        normal = glm::normalize(glm::vec3(homo_normal.x, homo_normal.y, homo_normal.z));
    }
    return normal;
}

std::pair<vec3, vec3> Triangle::getBoundingBoxDimensions() {
    calculateTransform();
    vec3 min, max;
    for (int i = 0; i < 3; i++) {
        float max_coord = std::max(std::max(_t_vertices[0][i], _t_vertices[1][i]), _t_vertices[2][i]);
        float min_coord = std::min(std::min(_t_vertices[0][i], _t_vertices[1][i]), _t_vertices[2][i]);
        max[i] = max_coord;
        min[i] = min_coord;
    }
    return std::make_pair(min, max);
}
