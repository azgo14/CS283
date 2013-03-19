#ifndef Pathtrace_H
#define Pathtrace_H
#include <glm/glm.hpp>

typedef glm::vec3 vec3;

class FIBITMAP;

class Pathtrace
{
public:
    Pathtrace() {};
    virtual ~Pathtrace() {};
    void pathtrace (const vec3& eye, const vec3& center, const vec3& up,
                    float fovx, float fovy, int width, int height,
                    FIBITMAP* bitmap, int depth);
    glm::vec3 calculateRay(const vec3& eye, const vec3& center,
                           const vec3& up, float fovx, float fovy,
                           int width, int height, float i, float j);
    glm::vec4 calculateColor(Object * obj, const vec3& intersect,
                             const vec3& eye, int recurse);
private:
    glm::vec4 getDirectLight(Object * obj, const vec3& intersection,
                             const vec3& eye, const vec3 eyedir,
                             const vec3& normal, vec4 * finalcolor);
    
    const static float INCREMENT;
};

#endif