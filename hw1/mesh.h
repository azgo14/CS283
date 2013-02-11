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
    void debugVerts(std::vector<glm::vec3>* vertex);
    void debugNorms(std::vector<glm::vec3>* norms);
    void debugVertColors(std::vector<glm::vec3>* colors);
    void collapse(int vert1, int vert2);
    bool hasEdge(int vert1, int vert2);
    void calcQuadrics();
    glm::mat4 calcQuadricMatrix(int vert);
    void printMatrix(glm::mat4 matrix); // for debugging purposes, remove when we're done
    
    std::vector<glm::vec3> _vertices;
    std::vector<glm::vec3> _normals;
    std::vector<int> _faces;
    std::vector<glm::vec3> _face_normals;
    std::vector<glm::mat4> _quadrics;

private:    
    std::map<int, std::vector<int> > _vertex_to_faces;  //vertex to list of adjacent faces
};

#endif
