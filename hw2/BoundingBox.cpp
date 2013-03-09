#include "BoundingBox.h"
#include <iostream>

BoundingBox::BoundingBox(Object * obj) {
  _obj = obj;
  _type = boundingbox;
  std::pair<vec3, vec3> dimensions = obj->getBoundingBoxDimensions();
  _min = dimensions.first;
  _max = dimensions.second;
  _left_box = NULL;
  _right_box = NULL;
}

BoundingBox::BoundingBox(BoundingBox* left, BoundingBox* right) {
  _obj = NULL;
  _left_box = left;
  _right_box = right;
  _type = boundingbox;
  _min = vec3(std::min(left->_min[0], right->_min[0]), std::min(left->_min[1], right->_min[1]), std::min(left->_min[2], right->_min[2]));
  _max = vec3(std::max(left->_max[0], right->_max[0]), std::max(left->_max[1], right->_max[1]), std::max(left->_max[2], right->_max[2]));
}
BoundingBox::~BoundingBox() {
  if (_left_box != NULL) {
    delete _left_box;
  }
  if (_right_box != NULL) {
    delete _right_box;
  }
  if (_obj != NULL) {
    delete _obj;
  }
}

std::pair<bool, vec3> BoundingBox::intersect(const vec3& origin, const vec3& direction) {
    float t, inf = std::numeric_limits<float>::max();
    vec3 x = vec3(1, 0, 0), y = vec3(0, 1, 0), z = vec3(0, 0, 1), rtn = vec3(inf, inf, inf);
    vec3 intersection;
    // near face
    if (glm::dot(direction, z) < 0) {
      t = -glm::dot((origin - _max), z)/glm::dot(direction, z);
        intersection = origin + direction * t;
        if (_min.x < intersection.x && intersection.x < _max.x && _min.y < intersection.y && intersection.y < _max.y) {
          return std::make_pair(true, intersection);
        }

    } 
    // far face
    if (glm::dot(direction, -z) < 0) {
      t = -glm::dot((origin - _min), -z)/glm::dot(direction, -z);
      //if (t > 0) {      
        intersection = origin + direction * t;
        if (_min.x < intersection.x && intersection.x < _max.x && _min.y < intersection.y && intersection.y < _max.y) {
          return std::make_pair(true, intersection);
        }
      //}
    }
    // right face
    if (glm::dot(direction, x) < 0) {
      t = -glm::dot((origin - _max), x)/glm::dot(direction, x);
      //if (t > 0) {
        intersection = origin + direction * t;
        if (_min.z < intersection.z && intersection.z < _max.z && _min.y < intersection.y && intersection.y < _max.y) {
          return std::make_pair(true, intersection);
        }
      //}
    }
    // left face
    if (glm::dot(direction, -x) < 0) {
      t = -glm::dot((origin - _min), -x)/glm::dot(direction, -x);
      //if (t > 0) {
        intersection = origin + direction * t;
        if (_min.z < intersection.z && intersection.z < _max.z && _min.y < intersection.y && intersection.y < _max.y) {
          return std::make_pair(true, intersection);
        }
    //  }
    }
    // top face
    if (glm::dot(direction, y) < 0) {
      t = -glm::dot((origin - _max), y)/glm::dot(direction, y);
      //if (t > 0) {
        intersection = origin + direction * t;
        if (_min.z < intersection.z && intersection.z < _max.z && _min.x < intersection.x && intersection.x < _max.x) {
          return std::make_pair(true, intersection);
        }
    //  }
    }
    // bottom face
    if (glm::dot(direction, -y) < 0) {
      t = -glm::dot((origin - _min), -y)/glm::dot(direction, -y);
      //if (t > 0) {
        intersection = origin + direction * t;
        if (_min.z < intersection.z && intersection.z < _max.z && _min.x < intersection.x && intersection.x < _max.x) {
          return std::make_pair(true, intersection);
        }
      //}
    }
    return std::make_pair(false, origin);
}

vec3 BoundingBox::getCenter() {
  float multiplier = .5;
  return (_min + _max) * multiplier;
}

