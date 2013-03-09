#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include "Object.h"

typedef glm::vec3 vec3;

class BoundingBox : public Object
{
public:
  BoundingBox(Object *object);
  BoundingBox(BoundingBox* left, BoundingBox* right);
  virtual ~BoundingBox();
  virtual std::pair<bool, vec3> intersect(const vec3& origin, const vec3& direction);
  vec3 getCenter();

  Object * _obj;    // only leaves will have this as non_null
  BoundingBox * _left_box;
  BoundingBox * _right_box;
private:
  vec3 _min;
  vec3 _max;
};
  
#endif  
