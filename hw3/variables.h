/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

// This is the basic include file for the global variables in the program.  
// Since all files need access to it, we define EXTERN as either blank or 
// extern, depending on if included in the main program or not.
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
vec3 centerinit(0.0,0.0,0.0) ;
int w = 1024, h = 1024 ; // width and height 
float fovy = 90.0 ; // For field of view
int use_light = -1 ; // don't use any lights as default
bool shadow_bool = true;
bool pcf_bool = true;
bool reflect_diffuse_bool = false;
bool display_sm = false;
bool use_dmap = true;
bool use_bump = true;
bool use_tex = true;

#else 
EXTERN vec3 eyeinit ; 
EXTERN vec3 upinit ; 
EXTERN vec3 center ; 
EXTERN int w, h ; 
EXTERN float fovy ; 
EXTERN int use_light ;
EXTERN bool shadow_bool;
EXTERN bool pcf_bool;
EXTERN bool reflect_diffuse_bool;
EXTERN bool display_sm;
EXTERN bool use_dmap;
EXTERN bool use_bump;
EXTERN bool use_tex;

#endif 

EXTERN bool useGlu; // Toggle use of "official" opengl/glm transform vs user 
EXTERN GLuint vertexshader, fragmentshader, shaderprogram ; // shaders
static enum {view, translate, scale, ltranslate} transop ; // which operation to transform 
enum shape {cube, sphere, teapot, reflectsphere, ground} ;
EXTERN float sx, sy ; // the scale in x and y 
EXTERN float tx, ty ; // the translation in x and y

// Lighting parameter array, similar to that in the fragment shader
const int numLights = 10 ; 
EXTERN GLfloat lightposn [4*numLights] ; // Light Positions
EXTERN GLfloat lightcolor[4*numLights] ; // Light Colors
EXTERN GLfloat lightransf[4*numLights] ; // Lights transformed by modelview
EXTERN int numused ;                     // How many lights are used 

EXTERN float ltx[numLights]; // to move the lights
EXTERN float lty[numLights];

// Materials (read from file) 
// With multiple objects, these are colors for each.
EXTERN GLfloat ambient[4] ; 
EXTERN GLfloat diffuse[4] ; 
EXTERN GLfloat specular[4] ; 
EXTERN GLfloat emission[4] ; 
EXTERN GLfloat shininess ; 

// For multiple objects, read from a file.  
const int maxobjects = 10 ; 
EXTERN int numobjects ; 
EXTERN struct object {
  shape type ; 
  GLfloat size ;
  GLfloat ambient[4] ; 
  GLfloat diffuse[4] ; 
  GLfloat specular[4] ;
  GLfloat emission[4] ; 
  GLfloat shininess ;
  mat4 transform ; 
} objects[maxobjects] ;

// Variables to set uniform params for lighting fragment shader 
EXTERN GLuint lightcol ; 
EXTERN GLuint lightpos ; 
EXTERN GLuint numusedcol ; 
EXTERN GLuint enablelighting ; 
EXTERN GLuint ambientcol ; 
EXTERN GLuint diffusecol ; 
EXTERN GLuint specularcol ; 
EXTERN GLuint emissioncol ; 
EXTERN GLuint shininesscol ; 

EXTERN GLuint s_vertexshader, s_fragshader, shadowprogram;
EXTERN GLuint s_frame_id;
EXTERN GLuint s_depth_texture_id;
EXTERN GLuint depthmatrix;
EXTERN GLuint depthbiasmatrix;
EXTERN GLuint shadowmap;
EXTERN vec3 inverse_light_dir;
EXTERN GLuint use_shadow;
EXTERN GLuint use_pcf;
EXTERN GLuint shadow_buff_size;

EXTERN GLuint cubemap_texture_id;
EXTERN GLuint cubemap;
EXTERN GLuint modelmatrix;
EXTERN GLuint modelmatrixinversetranspose;
EXTERN GLuint eye_world;
EXTERN GLuint is_reflect;
EXTERN GLuint is_reflect_diffuse;
EXTERN GLuint is_skybox;

EXTERN GLuint floor_texture_map;
EXTERN GLuint floor_normal_map;
EXTERN GLuint floor_displacement_map;
EXTERN GLuint displacementsampler;
EXTERN GLuint is_displace;
EXTERN GLuint is_bump;
EXTERN GLuint is_tex;


EXTERN GLuint texsampler;
EXTERN GLuint bumpsampler;
EXTERN GLuint tangent;
EXTERN GLuint bitangent;
