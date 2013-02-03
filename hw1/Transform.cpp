// Transform.cpp: implementation of the Transform class.


#include "Transform.h"

//Please implement the following functions:

// Helper rotation function.  
mat3 Transform::rotate(const float degrees, const vec3& axis) {
  const float radians = degrees * pi / 180;
  const float cos_value = cos(radians);
  const float sin_value = sin(radians);
  mat3 identity(1.0);
  const float x = axis[0];
  const float y = axis[1];
  const float z = axis[2];
  mat3 matrix_from_axis(x*x,x*y,x*z,x*y,y*y,y*z,x*z,y*z,z*z);
  mat3 ssm_axis(0,-z,y,z,0,-x,-y,x,0);
  mat3 rotation_matrix =
    cos_value * identity + (1-cos_value) * matrix_from_axis + sin_value * ssm_axis;
  return rotation_matrix;
}

// Transforms the camera left around the "crystal ball" interface
void Transform::left(float degrees, vec3& eye, vec3& up) {
  mat3 rotation_matrix = rotate(degrees, up);
  eye = eye * rotation_matrix;
}

// Transforms the camera up around the "crystal ball" interface
void Transform::up(float degrees, vec3& eye, vec3& up) {
  vec3 axis = glm::normalize(glm::cross(eye,up));
  mat3 rotation_matrix = rotate(degrees, axis);
  eye = eye * rotation_matrix;
  up = up * rotation_matrix;
}

// Your implementation of the glm::lookAt matrix
mat4 Transform::lookAt(vec3 eye, vec3 up) {
  vec3 w = glm::normalize(eye);
  vec3 u = glm::normalize(glm::cross(up,w));
  vec3 v = glm::cross(w,u);
  mat4 lookat_matrix(u[0],u[1],u[2],-glm::dot(u,eye),v[0],v[1],v[2],-glm::dot(v,eye),w[0],w[1],w[2],-glm::dot(w,eye),0,0,0,1);
  return lookat_matrix;
}

Transform::Transform()
{

}

Transform::~Transform()
{

}
