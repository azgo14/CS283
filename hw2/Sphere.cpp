#include "Sphere.h"
#include <iostream>

Sphere::Sphere(GLfloat pos1, GLfloat pos2, GLfloat pos3, GLfloat rad) {
    _position[0] = pos1;
    _position[1] = pos2;
    _position[2] = pos3;
    _radius = rad;
    _type = sphere;
    isLight = false;
}

std::pair<bool,vec3> Sphere::intersect(const vec3& origin, const vec3& direction) {
    glm::vec4 homo_origin = glm::vec4(origin[0], origin[1], origin[2], 1);
    glm::vec4 homo_direction = glm::vec4(direction[0], direction[1], direction[2], 0);
    homo_origin = homo_origin * glm::inverse(transform);
    homo_direction = homo_direction * glm::inverse(transform);
    glm::vec3 new_origin = glm::vec3(homo_origin[0]/homo_origin[3], homo_origin[1]/homo_origin[3], homo_origin[2]/homo_origin[3]);
    glm::vec3 new_direction = glm::vec3(homo_direction[0], homo_direction[1], homo_direction[2]);

    vec3 center(_position[0], _position[1], _position[2]);
    GLfloat a = glm::dot(new_direction, new_direction);
    GLfloat b = 2 * glm::dot(new_direction, new_origin - center);
    GLfloat c = glm::dot(new_origin - center,  new_origin - center) - (_radius * _radius);

    GLfloat discriminant = b * b - 4 * a * c;
    GLfloat t;
    if (discriminant < 0) {
        return std::make_pair(false, center);
    }
    GLfloat root1 = (-b + glm::sqrt(discriminant))/(2 * a);
    GLfloat root2 = (-b - glm::sqrt(discriminant))/(2 * a);

    if (root1 > 0 && root2 > 0) {
        t = glm::min(root1, root2);
    } else if (root1 == root2) {
        t = root1;
    } else if (root1 > 0 && root2 < 0) {
        t = root1;
    } else if (root1 < 0 && root2 > 0) {
        t = root2;
    } else {
        return std::make_pair(false, center);
    }
    glm::vec3 intersect = new_origin + new_direction * t;
    glm::vec4 homo_inter = glm::vec4(intersect[0], intersect[1], intersect[2], 1);
    homo_inter = homo_inter * transform;
    intersect = glm::vec3(homo_inter[0]/homo_inter[3], homo_inter[1]/homo_inter[3], homo_inter[2]/homo_inter[3]);
    return std::make_pair(true, intersect);
}

vec3 Sphere::getNormal(const vec3& intersect) {
    glm::vec3 center = glm::vec3(_position[0], _position[1], _position[2]);
    glm::vec4 homo_inter = glm::vec4(intersect[0], intersect[1], intersect[2], 1);
    homo_inter = homo_inter * glm::inverse(transform);

    vec3 normal = glm::normalize(glm::vec3(homo_inter[0]/homo_inter[3], homo_inter[1]/homo_inter[3], homo_inter[2]/homo_inter[3]) - center);
    vec4 homo_normal = vec4(normal.x, normal.y, normal.z, 0) * glm::transpose(glm::inverse(transform));
    return glm::normalize(vec3(homo_normal.x, homo_normal.y, homo_normal.z));
}

std::pair<vec3, vec3> Sphere::getBoundingBoxDimensions() {
    /*
    vec3 min = glm::vec3(_position[0] - _radius, _position[1] - _radius, _position[2] - _radius);
    vec3 max = glm::vec3(_position[0] + _radius, _position[1] + _radius, _position[2] + _radius);
    return std::make_pair(min, max);
    */
    vec4 farBottomLeft = glm::vec4(_position[0] - _radius, _position[1] - _radius, _position[2] - _radius, 1);
    vec4 farBottomRight = glm::vec4(_position[0] + _radius, _position[1] - _radius, _position[2] - _radius, 1);
    vec4 nearBottomLeft = glm::vec4(_position[0] - _radius, _position[1] - _radius, _position[2] + _radius, 1);
    vec4 nearBottomRight = glm::vec4(_position[0] + _radius, _position[1] - _radius, _position[2] + _radius, 1);
    vec4 farTopLeft = glm::vec4(_position[0] - _radius, _position[1] + _radius, _position[2] - _radius, 1);
    vec4 farTopRight = glm::vec4(_position[0] + _radius, _position[1] + _radius, _position[2] - _radius, 1);
    vec4 nearTopLeft = glm::vec4(_position[0] - _radius, _position[1] + _radius, _position[2] + _radius, 1);
    vec4 nearTopRight = glm::vec4(_position[0] + _radius, _position[1] + _radius, _position[2] + _radius, 1);
    vec4 vertices[] = {farBottomLeft, farBottomRight, nearBottomLeft, nearBottomRight, farTopLeft, farTopRight, nearTopLeft, nearTopRight};
    std::vector<vec3> transformedVertices;
    for (int i = 0; i < 8; i++) {
        vec4 t_vertex = vertices[i] * transform;
        transformedVertices.push_back(vec3(t_vertex.x/t_vertex.w, t_vertex.y/t_vertex.w, t_vertex.z/t_vertex.w));
    }
    return getMaxMinVertices(transformedVertices);
}

std::pair<vec3, vec3> Sphere::getMaxMinVertices(std::vector<vec3> vertices) {
    float min_x = vertices[0].x, min_y = vertices[0].y, min_z = vertices[0].z;
    float max_x = vertices[0].x, max_y = vertices[0].y, max_z = vertices[0].z;
    for (int i = 1; i < vertices.size(); i++) {
        if (vertices[i].x < min_x) {
            min_x = vertices[i].x;
        } else if (vertices[i].x > max_x) {
            max_x = vertices[i].x;
        }
        if (vertices[i].y < min_y) {
            min_y = vertices[i].y;
        } else if (vertices[i].y > max_y) {
            max_y = vertices[i].y;
        }
        if (vertices[i].z < min_z) {
            min_z = vertices[i].z;
        } else if (vertices[i].z > max_z) {
            max_z = vertices[i].z;
        }
    }
    vec3 min = vec3(min_x, min_y, min_z);
    vec3 max = vec3(max_x, max_y, max_z);
    return std::make_pair(min, max);
}
