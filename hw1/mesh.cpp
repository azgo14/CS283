#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

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


