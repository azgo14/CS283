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
}

void Mesh::calcNorms() {
    for (std::map<int, std::vector<int> >::iterator it = _vertex_to_faces.begin(); it != _vertex_to_faces.end(); ++it) {
        _normals[it->first] = glm::normalize(
                                glm::vec3(_normals[it->first].x / static_cast<double>(it->second.size()),
                                          _normals[it->first].y / static_cast<double>(it->second.size()),
                                          _normals[it->first].z / static_cast<double>(it->second.size())));
    } 
}

