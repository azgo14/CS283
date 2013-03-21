/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

// This is the basic include file for the global variables in the program.
// Since all files need access to it, we define EXTERN as either blank or
// extern, depending on if included in the main program or not.
#include <vector>
#include <string>
#include "Object.h"
#include "Sphere.h"
#include "Triangle.h"
#include "BoundingBox.h"
#include "Light.h"

#ifdef MAINPROGRAM
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN int amount; // The amount of rotation for each arrow press
EXTERN vec3 eye; // The (regularly updated) vector coordinates of the eye
EXTERN vec3 up;  // The (regularly updated) vector coordinates of the up

#ifdef MAINPROGRAM
vec3 eyeinit(0.0,0.0,5.0) ; // Initial eye position, also for resets
vec3 upinit(0.0,1.0,0.0) ; // Initial up position, also for resets
vec3 center(0.0,0.0,0.0) ; // Center look at point
int bpp = 24; // bits per pixel RGB. we have three 8 bit numbers
int w = 600, h = 400 ; // width and height
int depth = 5;
float fovy = 90.0 ; // For field of view
int ray_per_pixel = 5;
bool direct = true;
bool indirect = true;
bool uniform = false;
bool antilasing = false;
#else
EXTERN vec3 eyeinit ;
EXTERN vec3 upinit ;
EXTERN vec3 center ;
EXTERN int w, h ;
EXTERN float fovy ;
EXTERN int depth ;
EXTERN int ray_per_pixel;
EXTERN bool indirect;
EXTERN bool direct;
EXTERN bool uniform;
EXTERN bool antilasing;
#endif

EXTERN std::string output;

EXTERN bool useGlu; // Toggle use of "official" opengl/glm transform vs user
EXTERN GLuint vertexshader, fragmentshader, shaderprogram ; // shaders
static enum {view, translate, scale} transop ; // which operation to transform
EXTERN float sx, sy ; // the scale in x and y
EXTERN float tx, ty ; // the translation in x and y

EXTERN std::vector<Light *> lights;
EXTERN vec3 attenuation ;

// Materials (read from file)
// With multiple objects, these are colors for each.
/*
EXTERN GLfloat diffuse[4] ;
EXTERN GLfloat specular[4] ;
EXTERN GLfloat emission[4] ;
EXTERN GLfloat shininess ;
*/
EXTERN glm::vec4 diffuse ;
EXTERN glm::vec4 specular ;
EXTERN glm::vec4 emission ;
EXTERN GLfloat shininess ;

// For multiple objects, read from a file.
EXTERN int numobjects ;
EXTERN std::vector<BoundingBox*> boxes ;
EXTERN BoundingBox* root_box;

// Variables for specifying geometry
EXTERN std::vector<vec3> vertices;
EXTERN std::vector<vec3> normals;
EXTERN int maxvertnorms;
EXTERN int maxverts;

