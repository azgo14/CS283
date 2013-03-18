#ifndef OBJECT_H
#define OBJECT_H
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <utility>

typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

enum shape {sphere, tri, trinorm, boundingbox} ;

class Object
{
public:
    Object() {};
    virtual ~Object() {};
    virtual std::pair<bool,vec3> intersect(const vec3& origin, const vec3& direction) {};
    virtual vec3 getNormal(const vec3& intersect) {};
    virtual std::pair<vec3, vec3> getBoundingBoxDimensions() {};

    shape _type ;
    vec4 _ambient;
    vec4 _diffuse ;
    vec4 _specular ;
    vec4 _emission ;
    GLfloat _shininess ;
    glm::mat4 transform ;
    bool isLight;
};

#endif
