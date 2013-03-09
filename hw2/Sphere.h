#ifndef SPHERE_H
#define SPHERE_H
#include "Object.h"
#include <glm/glm.hpp>
#include <vector>
#include <iostream>


typedef glm::vec3 vec3; 

class Sphere : public Object
{
public:
	Sphere(GLfloat pos1, GLfloat pos2, GLfloat pos3, GLfloat radius) ;
	virtual ~Sphere() {};
	virtual std::pair<bool,vec3> intersect(const vec3& origin, const vec3& direction);
	vec3 getNormal(const vec3& intersect);
	std::pair<vec3, vec3> getBoundingBoxDimensions();
	std::pair<vec3, vec3> getMaxMinVertices(std::vector<vec3> vertices);

private:
  GLfloat _position[3];
  GLfloat _radius;
  GLfloat _t_position[3];
  GLfloat _t_radius;
};

#endif
