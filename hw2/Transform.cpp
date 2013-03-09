// Transform.cpp: implementation of the Transform class.


#include "Transform.h"


// Helper rotation function.  Please implement this.
mat3 Transform::rotate(const float degrees, const vec3& axis) {
    // YOUR CODE FOR HW2 HERE
    // Please implement this.  Likely the same as in HW 1.
    float radians = glm::radians(degrees);
    vec3 axisNormalized = glm::normalize(axis);
    float x = axisNormalized[0], y = axisNormalized[1], z = axisNormalized[2];
    return (glm::cos(radians) * glm::mat3(1)) +
           ((1 - glm::cos(radians)) * glm::mat3(x * x, x * y, x * z, x * y, y * y, y * z, x * z, y * z, z * z)) + (glm::sin(radians) * glm::mat3(0, -z, y, z, 0, -x, -y, x, 0));

}

void Transform::left(float degrees, vec3& eye, vec3& up) {
    // YOUR CODE FOR HW2 HERE
    // Likely the same as in HW 1.
    eye = eye * Transform::rotate(degrees, up);
}

void Transform::up(float degrees, vec3& eye, vec3& up) {
    // YOUR CODE FOR HW2 HERE
    // Likely the same as in HW 1.
    eye = eye * Transform::rotate(degrees, glm::cross(eye, up));
    up = glm::cross(eye, glm::normalize(glm::cross(up, eye)));
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
    // YOUR CODE FOR HW2 HERE
    // Likely the same as in HW 1.
    vec3 w = glm::normalize(eye - center);
    vec3 u = glm::normalize(glm::cross(up, w));
    vec3 v = glm::cross(w, u);
    mat4 R = glm::mat4(u[0], u[1], u[2], 0, v[0], v[1], v[2], 0, w[0], w[1], w[2], 0, 0, 0, 0, 1);
    vec4 RT = glm::vec4(-eye[0], -eye[1], -eye[2], 0) * R;
    R[0][3] = RT[0];
    R[1][3] = RT[1];
    R[2][3] = RT[2];
    return R;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
    // YOUR CODE FOR HW2 HERE
    // New, to implement the perspective transform as well.
    float angle = glm::radians(fovy)/2;
    float d = 1/glm::tan(angle);
    float A = -(zFar + zNear)/(zFar - zNear);
    float B = -(2 * zFar * zNear)/(zFar - zNear);
    return glm::mat4(d/aspect, 0, 0, 0, 0, d, 0, 0, 0, 0, A, B, 0, 0, -1, 0);
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) {
    // YOUR CODE FOR HW2 HERE
    // Implement scaling
    return glm::mat4(sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1);
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) {
    // YOUR CODE FOR HW2 HERE
    // Implement translation
    return glm::mat4(1, 0, 0, tx, 0, 1, 0, ty, 0, 0, 1, tz, 0, 0, 0, 1);
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


Transform::Transform()
{

}

Transform::~Transform()
{

}
