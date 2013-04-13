// Transform.cpp: implementation of the Transform class.


#include "Transform.h"

mat3 Transform::rotate(const float degrees, const vec3& axis) {
  vec3 axis_norm = glm::normalize(axis);
  const float radians = degrees * pi / 180;
  const float cos_value = cos(radians);
  const float sin_value = sin(radians);
  mat3 identity(1.0);
  const float x = axis_norm[0];
  const float y = axis_norm[1];
  const float z = axis_norm[2];
  mat3 matrix_from_axis(x*x,x*y,x*z,x*y,y*y,y*z,x*z,y*z,z*z);
  mat3 ssm_axis(0,-z,y,z,0,-x,-y,x,0);
  mat3 rotation_matrix =
    cos_value * identity + (1-cos_value) * matrix_from_axis + sin_value * ssm_axis;
  return rotation_matrix;
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
  vec3 w = glm::normalize(eye-center);
  vec3 u = glm::normalize(glm::cross(up,w));
  vec3 v = glm::cross(w,u);
  mat4 lookat_matrix(u[0],u[1],u[2],-glm::dot(u,eye),v[0],v[1],v[2],-glm::dot(v,eye),w[0],w[1],w[2],-glm::dot(w,eye),0,0,0,1);
  return lookat_matrix;
}

// Remember to transpose when needed for the returned matrix
mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
  const float d = 1 / tan((fovy / 2) * pi / 180);
  const float B = -(2 * zFar * zNear) / (zFar - zNear);
  const float A = -(zFar + zNear) / (zFar - zNear);
  mat4 perspective_matrix(d / aspect, 0, 0, 0, 0, d, 0, 0, 0, 0, A, B, 0, 0, -1, 0);
  return perspective_matrix;
}

// Remember to transpose when needed for the returned matrix
mat4 Transform::scale(const float &sx, const float &sy, const float &sz) {
  mat4 scale_matrix(sx,0,0,0,0,sy,0,0,0,0,sz,0,0,0,0,1);
  return scale_matrix;
}

// Remember to transpose when needed for the returned matrix
mat4 Transform::translate(const float &tx, const float &ty, const float &tz) {
  mat4 translation_matrix(1,0,0,tx,0,1,0,ty,0,0,1,tz,0,0,0,1);
  return translation_matrix;
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) {
  vec3 x = glm::cross(up,zvec) ; 
  vec3 y = glm::cross(zvec,x) ; 
  vec3 ret = glm::normalize(y) ; 
  return ret ; 
}

void Transform::left(float degrees, vec3& eye, vec3& up, vec3& center) {
    glm::mat4 trans = Transform::translate(-eye[0],-eye[1],-eye[2]);
    glm::vec4 homo_center(center,1);
    homo_center = homo_center * trans;
    center = glm::vec3(homo_center[0]/homo_center[3], homo_center[1]/homo_center[3], homo_center[2]/homo_center[3]);
    center = center * Transform::rotate(degrees, up);
    trans = Transform::translate(eye[0],eye[1],eye[2]);
    glm::vec4 homo_center2(center,1);
    homo_center = homo_center2 * trans;
    center = glm::vec3(homo_center[0]/homo_center[3], homo_center[1]/homo_center[3], homo_center[2]/homo_center[3]);
}

void Transform::up(float degrees, vec3& eye, vec3& up, vec3& center) {
    glm::vec3 direction = center-eye;
    glm::mat4 trans = Transform::translate(-eye[0],-eye[1],-eye[2]);
    glm::vec4 homo_center(center,1);
    homo_center = homo_center * trans;
    center = glm::vec3(homo_center[0]/homo_center[3], homo_center[1]/homo_center[3], homo_center[2]/homo_center[3]);
    center = center * Transform::rotate(degrees, glm::cross(direction,up));
    trans = Transform::translate(eye[0],eye[1],eye[2]);
    glm::vec4 homo_center2(center,1);
    homo_center = homo_center2 * trans;
    center = glm::vec3(homo_center[0]/homo_center[3], homo_center[1]/homo_center[3], homo_center[2]/homo_center[3]);
}

void Transform::forward(double amount, vec3& eye, vec3& center) {
    glm::vec3 direction = center-eye;
    direction = glm::normalize(direction);
    direction[0] = amount * direction[0];
    direction[1] = amount * direction[1];
    direction[2] = amount * direction[2];
    eye = eye + direction;
    center = center + direction;
}

void Transform::side(double amount, vec3& eye, vec3& up, vec3& center) {
    glm::vec3 f_direction = center-eye;
    glm::vec3 s_direction = glm::normalize(glm::cross(f_direction, up));
    s_direction[0] = amount * s_direction[0];
    s_direction[1] = amount * s_direction[1];
    s_direction[2] = amount * s_direction[2];
    eye = eye + s_direction;
    center = center + s_direction;
}

Transform::Transform()
{

}

Transform::~Transform()
{

}
