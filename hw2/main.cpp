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

using namespace std ;

// Main variables in the program.
#define MAINPROGRAM
#include "variables.h"
#include "readfile.h" // prototypes for readfile.cpp  
#include "Raytrace.h"

void saveScreenshot(FIBITMAP* img, string fname) {
    std::cout << "Saving screenshot: " << fname << "\n";
    FreeImage_Save(FIF_PNG, img, fname.c_str(), 0);
}

void init() {
    attenuation = glm::vec3(1, 0, 0);
    ambient = glm::vec4(0.2, 0.2, 0.2, 1);
}

BoundingBox* findClosestBB(BoundingBox* source, std::vector<BoundingBox*> candidates) {
  glm::vec3 source_center = source->getCenter();
  float min_distance = std::numeric_limits<float>::max();
  BoundingBox* closest;
  for ( std::vector<BoundingBox*>::iterator it = candidates.begin(); it != candidates.end(); ++it ) {
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
  if (nodes.size() == 1) {
    return nodes;
  }
  std::vector<BoundingBox*> new_nodes;
  std::vector<BoundingBox*> candidates = nodes;
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
      }
    }
  }
  if (new_nodes.size() == nodes.size()) {
    std::cout << "ERROR: new nodes size is same as old!\n";
    exit(1);
  }
  return BBTreeRecurse(new_nodes);
}

void createBBTree() {
  root_box = BBTreeRecurse(boxes)[0];
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        cerr << "Usage: transforms scenefile \n";
        exit(-1);
    }

    FreeImage_Initialise();
    init();
    output = "temp.png";
    readfile(argv[1]) ;
    FIBITMAP* bitmap = FreeImage_Allocate(w, h, bpp);
    float fovx = 2*glm::degrees(atan(tan(glm::radians((float)fovy/2))*((float)w/h))); 

    createBBTree();
    
    Raytrace rt;
    rt.raytrace(eye, center, up, fovx, fovy, w, h, bitmap, depth);

    saveScreenshot(bitmap, output);
    FreeImage_DeInitialise();

    delete root_box;   // all bounding boxes should be linked to this
    return 0;
}
