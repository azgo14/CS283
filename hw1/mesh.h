#ifndef _MESH_H_
#define _MESH_H_

#include <GL/glut.h>
#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp> // Replace with Transform.h if needed

class Mesh {
public:
    Mesh(void) {};
  
    void loadMesh(const char * filename);
    void calcNorms();
    void normalizeVerts();
    
    
    std::vector<glm::vec3> _vertices;
    std::vector<glm::vec3> _normals;
    std::vector<int> _faces;    

private:    
    std::map<int, std::vector<int> > _vertex_to_faces;  //vertex to list of adjacent faces
};

#endif