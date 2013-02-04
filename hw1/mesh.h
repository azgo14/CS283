#ifndef _MESH_H_
#define _MESH_H_

#include <GL/glut.h>
#include <string>
#include <vector>
#include <glm/glm.hpp> // Replace with Transform.h if needed

class Mesh {
public:
    Mesh(void) {};
  
    void loadMesh(const char * filename);
  
    std::vector<glm::vec3> _vertices;
    std::vector<glm::vec3> _faces;
};

#endif