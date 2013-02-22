#ifndef _MESH_H_
#define _MESH_H_

#include <GL/glut.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <glm/glm.hpp> // Replace with Transform.h if needed
#include <stack>

struct EdgeCollapse {
    std::pair<int, glm::vec3> v1;  // (index, vector)
    std::pair<int, glm::vec3> v2;
    std::pair<int, glm::vec3> v;
    std::vector<int> v1_faces;
    std::vector<int> v2_faces;
    std::vector<std::pair<int, std::vector<int> > > v2_affected_faces;
    std::vector<int> v_faces;
    std::vector<std::pair<int, glm::vec3> > d_faces; // (degenerate face index, vertex indices of face)
    std::vector<std::pair<int, std::vector<int> > > vert_to_del_faces; // (vertex index, list of degenerate faces deleted in _vertex_to_faces[vertex index])
} ;

class Mesh {
public:
    Mesh(void) {};
  
    void loadMesh(const char * filename);
    void loadEdgeCollapse(const char * filename);
    void calcNorms();
    void normalizeVerts();
    void debugVerts(std::vector<glm::vec3>* vertex);
    void debugNorms(std::vector<glm::vec3>* norms);
    void debugVertColors(std::vector<glm::vec3>* colors);
    bool collapse(int vert1, int vert2, std::ofstream* output, bool output);
    bool hasEdge(int vert1, int vert2);
    void calcQuadrics();
    glm::mat4 calcQuadricMatrix(int vert);
    void printMatrix(glm::mat4 matrix); // for debugging purposes, remove when we're done
    void getPairs();
    float calcError(std::pair<int, int> pair);
    void quadricSimplify(int simplify_num);
    bool stackSplit();
    bool stackCollapse();
    int numOfCollapse();
    void setResolution(int slider);
    
    
    std::vector<glm::vec3> _vertices;
    std::vector<glm::vec3> _normals;
    std::vector<int> _faces;
    std::vector<glm::vec3> _face_normals;
    std::vector<glm::mat4> _quadrics;
    std::vector<std::pair<float, std::pair<int, int> > > _pairs;

private:    
    std::map<int, std::vector<int> > _vertex_to_faces;  //vertex to list of adjacent faces

    std::stack<EdgeCollapse> _to_collapse;
    std::stack<EdgeCollapse> _to_split;
};

#endif
