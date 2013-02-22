#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <set>
#include "mesh.h"

bool comparePairs(std::pair<float, std::pair<int, int> > pair1, std::pair<float, std::pair<int, int> > pair2) {
    return pair1.first > pair2.first;
}

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
    /*
    for (std::vector<std::pair<float, std::pair<int, int> > >::iterator it = _pairs.begin(); it != _pairs.end(); ++it) {
        std::pair<float, std::pair<int, int> >p = *it;
        std::cout << p.first << ", (" << p.second.first << ", " << p.second.second << ")\n";
    }
    std::cout << "********\n";
    */
}

namespace {
    void split_string(const std::string& str, char delimiter, std::vector<std::string>* parts) {
        parts->clear();
        std::string temp;
        std::stringstream s_str (str);
        while(std::getline(s_str, temp, delimiter)) {
            parts->push_back(temp);
        }
    }
}

void Mesh::loadEdgeCollapse(const char* filename) {
    std::string str = ""; 
    std::ifstream in; 
    
    in.open(filename);
    if (in.is_open()) {
    	while(std::getline (in, str)) {
    	    if (str != "Collapse") {
    	        std::cerr << "Line not 'Collapse'" << std::endl;
                throw 2;
    	    }
            EdgeCollapse collapse;
            std::getline (in, str);
    	    if (str != "Old") {
    	        std::cerr << "Line not 'Old'" << std::endl;
                throw 2;
    	    }
    	    // Old
    	    std::getline (in, str);
            std::vector<std::string> parts;
            split_string(str, ' ', &parts);
            if (parts.size() != 4) {                
                std::cerr << "v1 size not 4" << std::endl;
                throw 2;
            }
            collapse.v1 = std::make_pair(atoi(parts[0].c_str()), glm::vec3(atof(parts[1].c_str()), atof(parts[2].c_str()), atof(parts[3].c_str())));
            //----
            std::getline (in, str);
            split_string(str, ' ', &parts);
            if (parts.size() != 4) {
                std::cerr << "v2 size not 4" << std::endl;
                throw 2;
            }
            collapse.v2 = std::make_pair(atoi(parts[0].c_str()), glm::vec3(atof(parts[1].c_str()), atof(parts[2].c_str()), atof(parts[3].c_str())));
            //----
            std::getline (in, str);
            split_string(str, ' ', &parts);
            if (atoi(parts[0].c_str()) != collapse.v1.first) {
                std::cerr << "expected v1" << std::endl;
                throw 2;
            }
            bool first = true;
            for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it) {
                if (first) {
                    first = false;
                    continue;
                }
                collapse.v1_faces.push_back(atoi(it->c_str()));
            }
            //----
            std::getline (in, str);
            split_string(str, ' ', &parts);
            if (atoi(parts[0].c_str()) != collapse.v2.first) {
                std::cerr << "expected v2" << std::endl;
                throw 2;
            }
            first = true;
            for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it) {
                if (first) {
                    first = false;
                    continue;
                }
                collapse.v2_faces.push_back(atoi(it->c_str()));
            }
            //-----
            std::getline (in, str);
            while (str != "New") {
                first = true;
                split_string(str, ' ', &parts);
                std::vector<int> temp;
                for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it) {
                    if (first) {
                        first = false;
                        continue;
                    }
                    temp.push_back(atoi(it->c_str()));
                }                
                collapse.v2_affected_faces.push_back(std::make_pair(atoi(parts[0].c_str()), temp));
                std::getline (in, str);  
            }
            // New
    	    if (str != "New") {
    	        std::cerr << "Line not 'New'" << std::endl;
                throw 2;
    	    }
    	    //----
    	    std::getline (in, str);
            split_string(str, ' ', &parts);
            if (parts.size() != 4) {
                std::cerr << "v size not 4" << std::endl;
                throw 2;
            }
            collapse.v = std::make_pair(atoi(parts[0].c_str()), glm::vec3(atof(parts[1].c_str()), atof(parts[2].c_str()), atof(parts[3].c_str())));
            //----
            std::getline (in, str);
            split_string(str, ' ', &parts);
            if (atoi(parts[0].c_str()) != collapse.v.first) {
                std::cerr << "expected v" << std::endl;
                throw 2;
            }
            first = true;
            for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it) {
                if (first) {
                    first = false;
                    continue;
                }
                collapse.v_faces.push_back(atoi(it->c_str()));
            }
            // Degenerate
            std::getline (in, str);
    	    if (str != "Degenerate") {
    	        std::cerr << "Line not 'Degenerate'" << std::endl;
                throw 2;
    	    }
    	    //----
    	    std::getline (in, str);
    	    if (str != "Face") {
    	        std::cerr << "Line not 'Face'" << std::endl;
                throw 2;
    	    }            
    	    //----
    	    while (std::getline (in, str) && str != "Vert_to_face") {
    	        split_string(str, ' ', &parts);
                if (parts.size() != 4) {
                    std::cerr << "face size not 4" << std::endl;
                    throw 2;
                }
                collapse.d_faces.push_back(std::make_pair(atoi(parts[0].c_str()), glm::vec3(atoi(parts[1].c_str()), atoi(parts[2].c_str()), atoi(parts[3].c_str()))));
    	    }
    	    //----            
    	    while (std::getline (in, str) && str != "End") {
    	        split_string(str, ' ', &parts);
                first = true;
                std::vector<int> face_list;
                for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it) {
                    if (first) {
                        first = false;
                        continue;
                    }
                    face_list.push_back(atoi(it->c_str()));
                }
                collapse.vert_to_del_faces.push_back(std::make_pair(atoi(parts[0].c_str()), face_list));
    	    }
            _to_split.push(collapse);
        }
    }
    // start from full model so need ot move everything in _to_split to _to_collapse
    while (!_to_split.empty()) {
        _to_collapse.push(_to_split.top());
        _to_split.pop();
    }
}

// change _vertices, _normals, _faces, _vertex_to_faces, _to_split, _to_collapse
bool Mesh::stackSplit() {
    if (!_to_split.empty()) {
        EdgeCollapse current = _to_split.top();
        _to_split.pop();
        // change _vertices
        _vertices[current.v1.first] = current.v1.second;
        _vertices[current.v2.first] = current.v2.second;
        
        // change _faces
        for (std::vector<std::pair<int, glm::vec3> >::iterator it = current.d_faces.begin(); it != current.d_faces.end(); ++it) {
            _faces[3*it->first] = it->second.x;
            _faces[3*it->first +1] = it->second.y;
            _faces[3*it->first +2] = it->second.z;
        }
        
        // change v1 to v2 for affected faces
        for (std::vector<std::pair<int, std::vector<int> > >::iterator it = current.v2_affected_faces.begin();
             it != current.v2_affected_faces.end(); ++it) {  
            for (std::vector<int>::iterator v_it = it->second.begin(); v_it != it->second.end(); ++v_it) {
                if (_faces[(3* it->first) + *v_it] != current.v1.first) {
                    std::cerr<<"location is not v1: " << current.v1.first << " vs " << _faces[(3* it->first) + *v_it]  <<std::endl;
                    throw 2;
                }
                _faces[(3* it->first) + *v_it] = current.v2.first;
            }
        }

        // change _vertex_to_faces
        for (std::vector<std::pair<int, std::vector<int> > >::iterator it = current.vert_to_del_faces.begin();
             it != current.vert_to_del_faces.end(); ++it) {
            for (std::vector<int>::iterator v_it = it->second.begin(); v_it != it->second.end(); ++v_it) {
                _vertex_to_faces[it->first].push_back(*v_it);
            }
        } 
        
        _vertex_to_faces[current.v1.first].clear();
        for (std::vector<int>::iterator it = current.v1_faces.begin(); it != current.v1_faces.end(); ++it) {
            _vertex_to_faces[current.v1.first].push_back(*it);
        }
        
        for (std::vector<int>::iterator it = current.v2_faces.begin(); it != current.v2_faces.end(); ++it) {
            _vertex_to_faces[current.v2.first].push_back(*it);
        }
    
        // calculate normal at vert1 and vert2
        std::set<int> affected_v;
        for (std::vector<int>::iterator v_it = _vertex_to_faces[current.v1.first].begin();
             v_it != _vertex_to_faces[current.v1.first].end(); ++v_it) {
             affected_v.insert(_faces[3*(*v_it)]);
             affected_v.insert(_faces[3*(*v_it)+1]);
             affected_v.insert(_faces[3*(*v_it)+2]);
        }
        
        for (std::vector<int>::iterator v_it = _vertex_to_faces[current.v2.first].begin();
             v_it != _vertex_to_faces[current.v2.first].end(); ++v_it) {
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
        
        _to_collapse.push(current);
        return true;
    }
    return false;
}

bool Mesh::stackCollapse() {
    if (!_to_collapse.empty()) {
        EdgeCollapse current = _to_collapse.top();
        _to_collapse.pop();
        
        std::ofstream useless;
        bool test = collapse(current.v1.first, current.v2.first, &useless, false);
        if (!test) {
            std::cerr << "Collapse didn't work but it should have" << std::endl;
            throw 2;
        }
        _to_split.push(current);
        return true;
    }
    return false;
}

int Mesh::numOfCollapse() {
    return _to_collapse.size();
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

bool Mesh::collapse(int vert1, int vert2, std::ofstream * edge_output, bool output) {
    if (!hasEdge(vert1, vert2)) {
        std::cerr << "Warning has no edge in between" << std::endl;
        return false;
    }
    std::stringstream old_output (std::stringstream::in | std::stringstream::out);
    std::stringstream new_output (std::stringstream::in | std::stringstream::out);
    std::stringstream degenerate_output (std::stringstream::in | std::stringstream::out);
    
    old_output << vert1 << " " << _vertices[vert1].x << " " << _vertices[vert1].y << " " << _vertices[vert1].z << std::endl;
    old_output << vert2 << " " << _vertices[vert2].x << " " << _vertices[vert2].y << " " << _vertices[vert2].z << std::endl;
    
    glm::vec3 vertex1 = _vertices[vert1];
    glm::vec3 vertex2 = _vertices[vert2];
    glm::vec3 new_vert(.5 * (vertex1.x + vertex2.x), .5 * (vertex1.y + vertex2.y), .5 * (vertex1.z + vertex2.z));
    
    new_output << vert1 << " " << new_vert.x << " " << new_vert.y << " " << new_vert.z << std::endl;
    
    _vertices[vert1] = new_vert;  // index of vert1 is now the new vert. update vert_face_adjacency to reflect this
    _vertices[vert2] = glm::vec3(10000,10000,10000);  // just junk data to placehold
    
    // output what was originally there for both
    old_output << vert1;
    for (std::vector<int>::iterator v1_it = _vertex_to_faces[vert1].begin();
         v1_it != _vertex_to_faces[vert1].end(); ++v1_it) {
        old_output << " " << *v1_it;
    }
    old_output << std::endl;
    old_output << vert2;
    for (std::vector<int>::iterator v2_it = _vertex_to_faces[vert2].begin();
         v2_it != _vertex_to_faces[vert2].end(); ++v2_it) {
        old_output << " " << *v2_it;   
    }
    old_output << std::endl;
    
    // change all occurrences of v1 and v2 with new v
    for (std::vector<int>::iterator v2_it = _vertex_to_faces[vert2].begin();
         v2_it != _vertex_to_faces[vert2].end(); ++v2_it) {
        old_output << *v2_it; 
        if (_faces[3*(*v2_it)] == vert2) {  // vert2 doesn't exist anymore, need to replace with vert1 which is the new vertex index
            _faces[3*(*v2_it)] = vert1;
            old_output << " " << 0;
        }
        if (_faces[3*(*v2_it) + 1] == vert2) {
            _faces[3*(*v2_it) + 1] = vert1;
            old_output << " " << 1;
        }
        if (_faces[3*(*v2_it) + 2] == vert2) {
            _faces[3*(*v2_it) + 2] = vert1;
            old_output << " " << 2;
        }
        old_output << std::endl;
        if (std::find(_vertex_to_faces[vert1].begin(), _vertex_to_faces[vert1].end(),*v2_it) == _vertex_to_faces[vert1].end()) {
            _vertex_to_faces[vert1].push_back(*v2_it);  // vert1 is the new vertex index. faces adjacent to v2 and v1 are now adjacent to new v
        }
    }
    _vertex_to_faces.erase(vert2); // cleanup

    // output what the change in vertex_to_face was for v1
    new_output << vert1;
    for (std::vector<int>::iterator v1_it = _vertex_to_faces[vert1].begin();
         v1_it != _vertex_to_faces[vert1].end(); ++v1_it) {
        new_output << " " << *v1_it;
    }
    new_output << std::endl;

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
    degenerate_output << "Face" << std::endl;
    for (std::set<int>::iterator v_it = degenerate_faces.begin(); v_it != degenerate_faces.end(); ++v_it) {    
        degenerate_output << *v_it << " " << _faces[3*(*v_it)] << " " << _faces[3*(*v_it)+1] << " " << _faces[3*(*v_it) + 2] << std::endl;
    }
    degenerate_output << "Vert_to_face" << std::endl;
    // remove face index from _vertex_to_faces
    for (std::set<int>::iterator v_it = degenerate_faces.begin(); v_it != degenerate_faces.end(); ++v_it) {    
        std::set<int> temp_v;
        temp_v.insert(_faces[3*(*v_it)]);
        temp_v.insert(_faces[3*(*v_it)+1]);
        temp_v.insert(_faces[3*(*v_it)+2]);
        for (std::set<int>::iterator temp_it = temp_v.begin(); temp_it != temp_v.end(); ++temp_it) {                
            std::vector<int>::iterator location = std::find(_vertex_to_faces[*temp_it].begin(), _vertex_to_faces[*temp_it].end(), *v_it);
            degenerate_output << *temp_it;
            while (location != _vertex_to_faces[*temp_it].end()) {
                degenerate_output << " " << *location;
                _vertex_to_faces[*temp_it].erase(location);  // delete degenerate face in vertex to faces
                location = std::find(_vertex_to_faces[*temp_it].begin(), _vertex_to_faces[*temp_it].end(), *v_it);
            }
            degenerate_output << std::endl;
            if (_vertex_to_faces[*temp_it].size() == 0) {
                _vertex_to_faces.erase(*temp_it);     
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
    std::string s_old_output = old_output.str();
    std::string s_new_output = new_output.str();
    std::string s_degenerate_output = degenerate_output.str();

    if (output) {
        (*edge_output) << "Collapse" << std::endl;
        (*edge_output) << "Old" << std::endl;
        (*edge_output) << s_old_output;
        (*edge_output) << "New" << std::endl;
        (*edge_output) << s_new_output;
        (*edge_output) << "Degenerate" << std::endl;
        (*edge_output) << s_degenerate_output;
        (*edge_output) << "End" << std::endl;
    }
    return true;
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
    std::set<std::pair<int, int> > pairs;
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
            pairs.insert(pair1);
            pairs.insert(pair2);
        }
    }
    for (std::set<std::pair<int, int> >::iterator p_it = pairs.begin(); p_it != pairs.end(); ++p_it) {
        _pairs.push_back(std::make_pair(calcError(*p_it), *p_it));
    }
 }

float Mesh::calcError(std::pair<int, int> pair) {
    if ( _vertices[pair.first].x > 100) {
        std::cout <<"FICL"<<std::endl;
    }
    if ( _vertices[pair.second].x > 100) {
        std::cout <<"FICL"<<std::endl;
    }
    glm::vec3 contracted_pair = _vertices[pair.first] + _vertices[pair.second];
    contracted_pair *= 0.5;
    glm::mat4 Q = _quadrics[pair.first] + _quadrics[pair.second];
    glm::vec4 v(contracted_pair.x, contracted_pair.y, contracted_pair.z, 1);
    return glm::dot(v, v * Q);
}

void Mesh::quadricSimplify(int simplify_num) {
    calcQuadrics();
    getPairs();
    std::ofstream edge_output("edge_collapse.txt");
    std::pair<int, int> pair, p;
    std::cout << "size: " << _pairs.size() << std::endl;
    for (int i = 0; i < simplify_num; ++i) {
        std::cout << "Currently " << i << " iteration" << std::endl;

        if (_pairs.size() == 0) {
            std::cout << "No more iterations left" << std::endl;
            return;
        }
        std::make_heap(_pairs.begin(), _pairs.end(), comparePairs);
        
        // remove the pair (u, v) of least cost from the heap
        pair = _pairs.front().second;
        std::pop_heap(_pairs.begin(), _pairs.end());
        _pairs.pop_back();

        if (_vertex_to_faces.find(pair.first) == _vertex_to_faces.end() || _vertex_to_faces.find(pair.second) == _vertex_to_faces.end()) {
            --i;
            continue;
        }
        
        std::cout << "Combine: " << pair.first << " " << pair.second << std::endl;
        // contract the pair (u, v)
        bool complete = collapse(pair.first, pair.second, &edge_output, true);

        _quadrics[pair.first] += _quadrics[pair.second];  // only do this for u v
        
        // update the costs of all valid pairs involving u and v (converting those involving v to be involving with u). ASSUMPTION: there is no (u,v) pair anymore
        std::set<std::pair<int, int> > to_add;            
        for (std::vector<std::pair<float, std::pair<int, int> > >::iterator it = _pairs.begin(); it != _pairs.end(); ) {
            p = it->second;
            if (pair.first == p.first || pair.second == p.first) {                
                it = _pairs.erase(it);
                to_add.insert(std::make_pair(pair.first, p.second));
            } else if (pair.first == p.second) {                
                it = _pairs.erase(it);
                to_add.insert(std::make_pair(p.first, pair.first));
            } else if (pair.second == p.second) {                
                it = _pairs.erase(it);
                if (pair.first < p.first) {
                    to_add.insert(std::make_pair(pair.first, p.first));
                } else {
                    to_add.insert(std::make_pair(p.first, pair.first));
                }
            } else {                
                ++it;
            }
        }
        
        // never make changes to datastructure while your iterating through datastructure
        for (std::set<std::pair<int, int> >::iterator to_add_it = to_add.begin(); to_add_it != to_add.end(); ++to_add_it) {
            _pairs.push_back(std::make_pair(calcError(*to_add_it), *to_add_it));
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

void Mesh::setResolution(int edge_collapse_done) {
    if (_to_split.size() > edge_collapse_done) {
        while (_to_split.size() > edge_collapse_done) {
            bool test = stackSplit();
            if (!test) {
                std::cerr << "stack split failed" << std::endl;
            }
        }
    } else {
        while (_to_split.size() < edge_collapse_done) {
            bool test = stackCollapse();
            if (!test) {
                std::cerr << "stack collapse failed" << std::endl;
            }
        }
    }
    
}

