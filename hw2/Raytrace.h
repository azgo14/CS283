#ifndef RAYTRACE_H
#define RAYTRACE_H
#include <glm/glm.hpp>

typedef glm::vec3 vec3; 

class FIBITMAP;

class Raytrace  
{
public:
	Raytrace() {};
	virtual ~Raytrace() {};
	void raytrace (const vec3& eye, const vec3& center, const vec3& up, float fovx, float fovy, int width, int height, FIBITMAP* bitmap, int depth);
	glm::vec3 calculateRay(const vec3& eye, const vec3& center, const vec3& up, float fovx, float fovy, int width, int height, float i, float j);
	glm::vec4 calculateColor(Object * obj, const vec3& intersect, const vec3& eye, int recurse);
};

#endif
