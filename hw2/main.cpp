/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "Transform.h"
#include <FreeImage.h>
#include <unistd.h>

using namespace std ;

// Main variables in the program.
#define MAINPROGRAM
#include "variables.h"
#include "readfile.h" // prototypes for readfile.cpp  
#include "Pathtrace.h"

void saveScreenshot(FIBITMAP* img, string fname) {
    std::cout << "Saving screenshot: " << fname << "\n";
    FreeImage_Save(FIF_PNG, img, fname.c_str(), 0);
}

void init() {
    attenuation = glm::vec3(1, 0, 0);
}

BoundingBox* findClosestBB(BoundingBox* source, const std::vector<BoundingBox*>& candidates) {
    glm::vec3 source_center = source->getCenter();
    float min_distance = std::numeric_limits<float>::max();
    BoundingBox* closest;
    for ( std::vector<BoundingBox*>::const_iterator it = candidates.begin(); it != candidates.end(); ++it ) {
        float temp_dist = glm::dot(source_center-(*it)->getCenter(),source_center-(*it)->getCenter());
        if (temp_dist < min_distance) {
            closest = *it;
            min_distance = temp_dist;
        }
    }
    if (closest == NULL) {
        std::cout << "ERROR: no closest Bounding Box found\n";
        exit(1);
    }
    return closest;
}

std::vector<BoundingBox*> BBTreeRecurse( std::vector<BoundingBox*> nodes ) {
    std::cout << "Current number of boxes: " << nodes.size() << std::endl;
    if (nodes.size() == 1) {
        return nodes;
    }
    std::vector<BoundingBox*> new_nodes;
    std::vector<BoundingBox*> candidates(nodes);
    for ( std::vector<BoundingBox*>::iterator it = nodes.begin(); it != nodes.end(); ++it ) {
        if (candidates.size() > 1) {
            std::vector<BoundingBox*>::iterator position = std::find(candidates.begin(), candidates.end(), *it);
            if (position != candidates.end()) {
                candidates.erase(position);
            } else {
                continue;
            }
            BoundingBox* closest_match = findClosestBB(*it, candidates);
            new_nodes.push_back(new BoundingBox(*it, closest_match));
            position = std::find(candidates.begin(), candidates.end(), closest_match);
            if (position != candidates.end()) {
                candidates.erase(position);
            } else {
                std::cerr << "Candidate that should have been in list is not" <<std::endl;
                exit(1);
            }
        } else {
            if (candidates.size() == 1) {
                new_nodes.push_back(candidates[0]);
                break;
            }
        }
    }
    if (new_nodes.size() == nodes.size()) {
        std::cout << "ERROR: new nodes size (" << new_nodes.size() << ") is same as old!\n";
        exit(1);
    }
    return BBTreeRecurse(new_nodes);
}

void createBBTree() {
    root_box = BBTreeRecurse(boxes)[0];
}

namespace {
void usage() {
    std::cout << "USAGE: ./transform -s SCENE_FILE [optional options]" << std::endl
              << "\tOPTIONAL OPTIONS:" << std::endl
              << "\t[-r rays_per_pixel]\tRay traced per pixel [DEFAULT: 5]" << std::endl
              << "\t[-d direct]\t\tSpecify flag to output only direct light scene" << std::endl
              << "\t[-i indirect]\t\tSpecify flag to output only indirect light scene" << std::endl;
}
} // namespace

void areaLightTest() {
    AreaLight test = AreaLight(vec3(-1,1,1), vec3(-1,-1,1), vec3(1,1,1));
    vec4 position = test.getLightPosn();
    std::cout << "Point in triangle: " << position[0] << " " << position[1] << " " << position[2] << " " << position[3] << std::endl;
}
void tests() {
    areaLightTest();
}

int main(int argc, char* argv[]) {
    //tests();
    int option;
    char * filename = NULL;
    while ((option = getopt(argc, argv, "r:s:d:i:u")) != -1) {
        switch (option) {
        case 'r':
            ray_per_pixel = atoi(optarg);
            break;
        case 's':
            filename = optarg;
            break;
        case 'd':
            indirect = false;
            break;
        case 'i':
            direct = false;
            break;
        case 'u':
            uniform = true;
            break;
        default:
            usage();
            exit(1);
        }
    }
    if (filename == NULL) {
        usage();
        exit(-1);
    }

    FreeImage_Initialise();
    init();
    output = "temp.png";
    readfile(filename) ;
    FIBITMAP* bitmap = FreeImage_Allocate(w, h, bpp);
    float fovx = 2*glm::degrees(atan(tan(glm::radians((float)fovy/2))*((float)w/h)));

    createBBTree();
    Pathtrace rt;
    rt.pathtrace(eye, center, up, fovx, fovy, w, h, bitmap, depth);

    saveScreenshot(bitmap, output);
    FreeImage_DeInitialise();

    delete root_box;   // all bounding boxes should be linked to this
    return 0;
}
