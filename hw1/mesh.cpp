#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <set>
#include <algorithm>

#include "mesh.h"

namespace {

glm::vec3 getNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
    return glm::normalize(glm::cross(v2-v1, v3-v1));
}
} // namespace

void Mesh::loadMesh(const char * filename) {
    std::string str = ""; 
    std::ifstream in; 
    int num_vertices;
    int num_faces;
    
    in.open(filename);
    if (in.is_open()) {
    	std::getline (in, str);
    	if (in) {
    	    if (str != "OFF") {
    	        std::cerr << "File " << filename << " is not an OFF file" << std::endl; 
                throw 2;
    	    }
    	} else {
    	    std::cerr << "File " << filename << " is not an OFF file" << std::endl; 
            throw 2;
    	}
    	std::getline (in,str);
    	if (in) {
            std::string num_holder = "";
    	    std::stringstream stream(str);
            std::getline(stream, num_holder, ' ');
            if (!stream) {
                std::cerr << "Could not get num of vertices" << std::endl;
                throw 2;
            }
            num_vertices = atoi(num_holder.c_str());
            
            std::getline(stream, num_holder, ' ');
            if (!stream) {
                std::cerr << "Could not get num of faces" << std::endl;
                throw 2; 
            }
            num_faces = atoi(num_holder.c_str());
	    } else {
	        std::cerr << "Could not parse second line. File " << filename << std::endl; 
            throw 2;
	    }
	    
        for (int i = 0; i < num_vertices; ++i) {
            std::getline(in, str);
            std::string num_holder = "";
    	    std::stringstream stream(str);
            float x,y,z;
            
    	    std::getline(stream, num_holder, ' ');
            if (!stream) {
                std::cerr << "Could not get x of vertices" << std::endl;
                throw 2;
            }
            x = atof(num_holder.c_str());
            
            std::getline(stream, num_holder, ' ');
            if (!stream) {
                std::cerr << "Could not get y of vertices" << std::endl;
                throw 2;
            }
            y = atof(num_holder.c_str());
            
            std::getline(stream, num_holder);
            if (!stream) {
                std::cerr << "Could not get z of vertices" << std::endl;
                throw 2;
            }
            z = atof(num_holder.c_str());
            _vertices.push_back(glm::vec3(x,y,z));
            _normals.push_back(glm::vec3(0,0,0)); // Set placeholder
            
        }
        
        for (int i = 0; i < num_faces; ++i) {
            std::getline(in, str);
            std::string num_holder = "";
    	    std::stringstream stream(str);
            int v1,v2,v3;

    	    std::getline(stream, num_holder, ' ');
    	    if (!stream || num_holder != "3") {
                std::cerr << "Expected only 3 vertices per face instead of " << num_holder << std::endl;
                throw 2;
            }
                	    
    	    std::getline(stream, num_holder, ' ');
    	    if (!stream) {
                std::cerr << "Could not get first of vertices" << std::endl;
                throw 2;
            }
            v1 = atoi(num_holder.c_str());
            
            std::getline(stream, num_holder, ' ');
            if (!stream) {
                std::cerr << "Could not get second of vertices" << std::endl;
                throw 2;
            }
            v2 = atoi(num_holder.c_str());
            
            std::getline(stream, num_holder);
            if (!stream) {
                std::cerr << "Could not get third of vertices" << std::endl;
                throw 2;
            }
            v3 = atoi(num_holder.c_str());
            _faces.push_back(v1);
            _faces.push_back(v2);
            _faces.push_back(v3);
            
            _normals[v1] += getNormal(_vertices[v1], _vertices[v2], _vertices[v3]);
            _normals[v2] += getNormal(_vertices[v1], _vertices[v2], _vertices[v3]);
            _normals[v3] += getNormal(_vertices[v1], _vertices[v2], _vertices[v3]);

	    _face_normals.push_back(getNormal(_vertices[v1], _vertices[v2], _vertices[v3]));
            
            _vertex_to_faces[v1].push_back(i);
            _vertex_to_faces[v2].push_back(i);
            _vertex_to_faces[v3].push_back(i);    
        }
    }
    else {
    	std::cerr << "Unable to Open File " << filename << "\n"; 
    	throw 2; 
    }
    calcNorms();
    normalizeVerts();
    calcQuadrics();
    getPairs();
}

void Mesh::normalizeVerts() {
    float min_x = 999999;
    float min_y = 999999;
    float min_z = 999999;
    float max_x = -999999;
    float max_y = -999999;
    float max_z  = -999999;
    
    for (std::vector<glm::vec3>::iterator it = _vertices.begin(); it != _vertices.end(); ++it) {
        min_x = std::min(it->x, min_x);      
        min_y = std::min(it->y, min_y);      
        min_z = std::min(it->z, min_z);      

        max_x = std::max(it->x, max_x);      
        max_y = std::max(it->y, max_y);      
        max_z = std::max(it->z, max_z);      
    }
        
    float scale = std::max((max_x - min_x), std::max((max_y - min_y), (max_z - min_z)));
    for (std::vector<glm::vec3>::iterator it = _vertices.begin(); it != _vertices.end(); ++it) {
        it->x -= (min_x + max_x) / 2.0;
        it->y -= (min_y + max_y) / 2.0;
        it->z -= (min_z + max_z) / 2.0;
        
        it->x = 5 * it->x / scale;
        it->y = 5 * it->y / scale;
        it->z = 5 * it->z / scale;
        
    }
}

void Mesh::calcNorms() {
    for (std::map<int, std::vector<int> >::iterator it = _vertex_to_faces.begin(); it != _vertex_to_faces.end(); ++it) {
        _normals[it->first] = glm::normalize(
                                glm::vec3(_normals[it->first].x / static_cast<double>(it->second.size()),
                                          _normals[it->first].y / static_cast<double>(it->second.size()),
                                          _normals[it->first].z / static_cast<double>(it->second.size())));
    } 
}

void Mesh::debugVerts(std::vector<glm::vec3>* vertices) {
    for (std::vector<int>::iterator it = _faces.begin(); it != _faces.end(); ++it) {
        vertices->push_back(_vertices[*it]);
    }
}

void Mesh::debugNorms(std::vector<glm::vec3>* normals) {
    for (std::vector<int>::iterator it = _faces.begin(); it != _faces.end(); ++it) {
          normals->push_back(_normals[*it]);
    }
}

void Mesh::debugVertColors(std::vector<glm::vec3>* colors) {
    int vert_size = _vertices.size();
    for (std::vector<int>::iterator it = _faces.begin(); it != _faces.end(); ++it) {
        colors->push_back(glm::vec3((rand() % vert_size) / static_cast<float>(vert_size),
                                    (rand() % vert_size) / static_cast<float>(vert_size),
                                    (rand() % vert_size) / static_cast<float>(vert_size)));
    }
}

bool Mesh::hasEdge(int vert1, int vert2) {
    for (std::vector<int>::const_iterator v1_it = _vertex_to_faces[vert1].begin();
         v1_it != _vertex_to_faces[vert1].end(); ++v1_it) {
        for (std::vector<int>::const_iterator v2_it = _vertex_to_faces[vert2].begin();
             v2_it != _vertex_to_faces[vert2].end(); ++v2_it) {
            if (*v1_it == *v2_it) {  // if two vertices have the same face, they have an edge between them
                return true;
            }
        }   
    }
    return false;
}    

void Mesh::collapse(int vert1, int vert2) {
    if (!hasEdge(vert1, vert2)) {
        return;
    }
    std::cout<<"here"<<std::endl;
    glm::vec3 vertex1 = _vertices[vert1];
    glm::vec3 vertex2 = _vertices[vert2];
    glm::vec3 new_vert(.5 * (vertex1.x + vertex2.x), .5 * (vertex1.y + vertex2.y), .5 * (vertex1.z + vertex2.z));
    
    _vertices[vert1] = new_vert;  // index of vert1 is now the new vert. update vert_face_adjacency to reflect this
    _vertices[vert2] = glm::vec3(10000,10000,10000);  // just junk data to placehold
    
    // change all occurrences of v1 and v2 with new v
    for (std::vector<int>::iterator v2_it = _vertex_to_faces[vert2].begin();
         v2_it != _vertex_to_faces[vert2].end(); ++v2_it) {
        if (_faces[3*(*v2_it)] == vert2) {  // vert2 doesn't exist anymore, need to replace with vert1 which is the new vertex index
            _faces[3*(*v2_it)] = vert1;
        }
        if (_faces[3*(*v2_it) + 1] == vert2) {
            _faces[3*(*v2_it) + 1] = vert1;
        }
        if (_faces[3*(*v2_it) + 2] == vert2) {
            _faces[3*(*v2_it) + 2] = vert1;
        }
        if (std::find(_vertex_to_faces[vert1].begin(), _vertex_to_faces[vert1].end(),*v2_it) == _vertex_to_faces[vert1].end()) {
            _vertex_to_faces[vert1].push_back(*v2_it);  // vert1 is the new vertex index. faces adjacent to v2 and v1 are now adjacent to new v
        }
    }
    _vertex_to_faces.erase(vert2); // cleanup
    

    // remove degenerate faces
    std::set<int> degenerate_faces;
    for (std::vector<int>::iterator v_it = _vertex_to_faces[vert1].begin();
         v_it != _vertex_to_faces[vert1].end(); ++v_it) {
        bool once = false;
        bool degenerate = false;
        if (_faces[3*(*v_it)] == vert1) {
            once = true;
        }
        if (_faces[3*(*v_it) + 1] == vert1) {
            if (once) {  // degenerate case (2 vertices are the same out of 3)
                degenerate = true;
            } else {
                once = true;
            }
        }
        if (_faces[3*(*v_it) + 2] == vert1) {
            if (once) {
                degenerate = true;
            } else {
                once = true;
            }
        }
        if (degenerate) {
            degenerate_faces.insert(*v_it);
        }
    }
    
    for (std::set<int>::iterator v_it = degenerate_faces.begin(); v_it != degenerate_faces.end(); ++v_it) {    
        std::set<int> temp_v;
        temp_v.insert(_faces[3*(*v_it)]);
        temp_v.insert(_faces[3*(*v_it)+1]);
        temp_v.insert(_faces[3*(*v_it)+2]);
        for (std::set<int>::iterator temp_it = temp_v.begin(); temp_it != temp_v.end(); ++temp_it) {                
            std::vector<int>::iterator location = std::find(_vertex_to_faces[*temp_it].begin(), _vertex_to_faces[*temp_it].end(), *v_it);
            while (location != _vertex_to_faces[*temp_it].end()) {
                _vertex_to_faces[*temp_it].erase(location);  // delete degenerate face in vertex to faces
                location = std::find(_vertex_to_faces[*temp_it].begin(), _vertex_to_faces[*temp_it].end(), *v_it);
            }
        }    
        _faces[3*(*v_it)] = 0; // need a placeholder so indices are the same. nothing will point to this face index (i hope)
        _faces[3*(*v_it)+1] = 0;
        _faces[3*(*v_it)+2] = 0;
    }
                 
    // calculate normal at vert1
    _normals[vert2] = glm::vec3(10000,10000,10000); // just junk data
    std::set<int> affected_v;
    for (std::vector<int>::iterator v_it = _vertex_to_faces[vert1].begin();
         v_it != _vertex_to_faces[vert1].end(); ++v_it) {
         affected_v.insert(_faces[3*(*v_it)]);
         affected_v.insert(_faces[3*(*v_it)+1]);
         affected_v.insert(_faces[3*(*v_it)+2]);
    }
    
    for (std::set<int>::iterator it = affected_v.begin(); it != affected_v.end(); ++it) {
        _normals[*it] = glm::vec3(0,0,0);
        for (std::vector<int>::iterator v_it = _vertex_to_faces[*it].begin();
             v_it != _vertex_to_faces[*it].end(); ++v_it) {
            _normals[*it] += getNormal(_vertices[_faces[3*(*v_it)]], _vertices[_faces[3*(*v_it)+1]], _vertices[_faces[3*(*v_it)+2]]);
        }
        _normals[*it] = glm::normalize(
                            glm::vec3(_normals[*it].x / static_cast<double>(_vertex_to_faces[*it].size()),
                                      _normals[*it].y / static_cast<double>(_vertex_to_faces[*it].size()),
                                      _normals[*it].z / static_cast<double>(_vertex_to_faces[*it].size())));
    }
}

void Mesh::calcQuadrics() {
    for (int v = 0; v < _vertices.size(); ++v) {
        _quadrics.push_back(calcQuadricMatrix(v));
    }
}

glm::mat4 Mesh::calcQuadricMatrix(int vert) {
    glm::mat4 Q(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    for (std::vector<int>::iterator f_it = _vertex_to_faces[vert].begin(); f_it != _vertex_to_faces[vert].end(); ++f_it) {
        glm::vec3 normal = _face_normals[*f_it];
        glm::vec4 p(normal.x, normal.y, normal.z, -glm::dot(_vertices[vert], normal));
	glm::mat4 K(p.x * p.x, p.x * p.y, p.x * p.z, p.x * p.w,
		    p.y * p.x, p.y * p.y, p.y * p.z, p.y * p.w,
		    p.z * p.x, p.z * p.y, p.z * p.z, p.z * p.w,
		    p.w * p.x, p.w * p.y, p.w * p.z, p.w * p.w);
        Q += K;
    }
    
    return Q;
}


void Mesh::getPairs() {
    for (std::map<int, std::vector<int> >::iterator it = _vertex_to_faces.begin(); it != _vertex_to_faces.end(); ++it) {
        int vertex = it->first;
        for (std::vector<int>::iterator f_it = (it->second).begin(); f_it != (it->second).end(); ++f_it) {
            int v1 = _faces[3*(*f_it)];
	    int v2 = _faces[3*(*f_it)+1];
            int v3 = _faces[3*(*f_it)+2];
            std::pair<int, int> pair1, pair2;
            if (vertex == v1) {
                vertex < v2 ? pair1 = std::make_pair(vertex, v2) : pair1 = std::make_pair(v2, vertex);
                vertex < v3 ? pair2 = std::make_pair(vertex, v3) : pair2 = std::make_pair(v3, vertex);
            } else if (vertex == v2) {
                vertex < v1 ? pair1 = std::make_pair(vertex, v1) : pair1 = std::make_pair(v1, vertex);
                vertex < v3 ? pair2 = std::make_pair(vertex, v3) : pair2 = std::make_pair(v3, vertex);
            } else if (vertex == v3) {
                vertex < v1 ? pair1 = std::make_pair(vertex, v1) : pair1 = std::make_pair(v1, vertex);
                vertex < v2 ? pair2 = std::make_pair(vertex, v2) : pair2 = std::make_pair(v2, vertex);
            } else {
                std::cout << "Error: Something went wrong...\n";
                exit(1);
            }
            _pairs.insert(pair1);
	    _pairs.insert(pair2);
        }
    }
 }

void Mesh::printMatrix(glm::mat4 matrix) {
    for (int row = 0; row < 4; row++) {
        for (int column = 0; column < 4; column++) {
            std::cout << matrix[row][column] << ", ";
        }
    }
    std::cout << "****\n";
}

