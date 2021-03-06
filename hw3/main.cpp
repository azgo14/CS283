/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/


#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "shaders.h"
#include "Transform.h"
#include <FreeImage.h>
#include "Simple OpenGL Image Library/src/SOIL.h"

using namespace std ; 

// Main variables in the program.  
#define MAINPROGRAM 
#include "variables.h" 
#include "readfile.h" // prototypes for readfile.cpp  
void display(void) ;  // prototype for display function.  

// Uses the Projection matrices (technically deprecated) to set perspective 
// We could also do this in a more modern fashion with glm.  
void reshape(int width, int height){
	w = width;
	h = height;
        mat4 mv ; // just like for lookat

	glMatrixMode(GL_PROJECTION);
        float aspect = w / (float) h, zNear = 0.1, zFar = 99.0 ;
        // I am changing the projection stuff to be consistent with lookat
        if (useGlu) mv = glm::perspective(fovy,aspect,zNear,zFar) ; 
        else {
          mv = Transform::perspective(fovy,aspect,zNear,zFar) ; 
          mv = glm::transpose(mv) ; // accounting for row major 
        }
        glLoadMatrixf(&mv[0][0]) ; 

	glViewport(0, 0, w, h);
}

GLuint load_texture(const char * filename) {
    GLuint tex2d = SOIL_load_OGL_texture(
                       filename,
                       SOIL_LOAD_AUTO,
                       SOIL_CREATE_NEW_ID,
                       SOIL_FLAG_POWER_OF_TWO| SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_INVERT_Y);
    if (tex2d == 0) {
        cout << SOIL_last_result() << "\n";
        exit(1);
    }
    return tex2d;
}

void saveScreenshot(string fname) {
	int pix = w * h;
	BYTE pixels[3*pix];	
	glReadBuffer(GL_FRONT);
	glReadPixels(0,0,w,h,GL_BGR,GL_UNSIGNED_BYTE, pixels);

	FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
	
	std::cout << "Saving screenshot: " << fname << "\n";

	FreeImage_Save(FIF_PNG, img, fname.c_str(), 0);
}


void printHelp() {
  std::cout << "\npress 'h' to print this message again.\n" 
            << "press '+' or '-' to change the amount of movement that\noccurs with each arrow press.\n" 
            << "press 'r' to reset the transformations.\n"
            << "press 's' to use shadows.\n"
            << "press 'p' to use pcf shadows when shadows are on\n"
            << "press 'd' to use diffuse shading with environment reflection\n"
            << "press 'm' to see how shadow map changes\n"
            << "press 't' to turn on texture (not environment textures)\n"
            << "press 'b' to turn on bump mapping\n"
            << "press 'e' to turn on displacement mapping\n"
            << "press '0' to move around light"
            << "press ESC to quit.\n" ;      
}

int oldx;
int oldy;
int angle_change;

void drag(int x, int y) {
    if (angle_change) {
        double diff_x = x - oldx;
        double diff_y = oldy - y;
        double x_ang = glm::degrees(glm::atan(diff_x * .01, 1.0));
        double y_ang = glm::degrees(glm::atan(diff_y*.01,1.0));
        Transform::up(y_ang, eye,  up, center);
        Transform::left(-x_ang, eye, up, center);
        oldx = x ;
        oldy = y ;
        glutPostRedisplay();
    }
}

// Mouse motion.  You need to respond to left clicks (to add points on curve)
// and right clicks (to delete points on curve)
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            oldx = x ;
            oldy = y ;
            angle_change = true;
        }
        if (state == GLUT_UP) {
            angle_change = false;
        }
    }
}

void keyboard(unsigned char key, int x, int y) {
	switch(key) {
	case 't':
        use_tex = !use_tex;
        break;
	case 'b':
        use_bump = !use_bump;
        break;
	case 'e':
        use_dmap = !use_dmap;
        break;
	case 'm':
        display_sm = !display_sm;
        break;
	case 'd':
        reflect_diffuse_bool = !reflect_diffuse_bool;
        break;
	case '+':
		amount++;
		std::cout << "amount set to " << amount << "\n" ;
		break;
	case '-':
		amount--;
		std::cout << "amount set to " << amount << "\n" ; 
		break;
    case 'p':
        pcf_bool = !pcf_bool;
        break;
    case 's':
        shadow_bool = !shadow_bool;
        break;
	case 'h':
		printHelp();
		break;
        case 27:  // Escape to quit
                exit(0) ;
                break ;
    case 'r': // reset eye and up vectors, scale and translate. 
		eye = eyeinit ; 
		up = upinit ;
		center = centerinit ; 
                sx = sy = 1.0 ; 
                tx = ty = 0.0 ; 
                for (int i = 0; i < numLights; ++i) {
                  ltx[i] = 0.0;
                  lty[i] = 0.0;
                }
		break ;   
    case 'v': 
        if (use_light == -1) {
            transop = view ;
            std::cout << "Operation is set to View\n" ; 
        }
        break ;  
  case '0':
    use_light = 0;
    transop = ltranslate;
    std::cout << "Operation is set to move light " << use_light << "\n" ; 
    break;
  case 'n': // back to nornal
    use_light = -1;
    transop = view ;
    std::cout << "Operation is set to View scene\n" ;
    break;
        }
	glutPostRedisplay();
}

//  You will need to enter code for the arrow keys 
//  When an arrow key is pressed, it will call your transform functions

void specialKey(int key, int x, int y) {
    switch(key) {
    case 100: //left
        if (transop == view) Transform::side(-amount * .1, eye,  up, center);
        else if (transop == scale) sx -= amount * 0.01 ;
        else if (transop == ltranslate) {
            lightposn[0] -= amount * 0.01;
        }
        break;
    case 101: //up
        if (transop == view) Transform::forward(amount * .1,  eye,  center);
        else if (transop == scale) sy += amount * 0.01 ;
        else if (transop == ltranslate) {
            lightposn[1] += amount * 0.01;
        }
        break;
    case 102: //right
        if (transop == view) Transform::side(amount* .1, eye,  up, center);
        else if (transop == scale) sx += amount * 0.01 ;
        else if (transop == ltranslate) {
            lightposn[0] += amount * 0.01;
        }
        break;
    case 103: //down
        if (transop == view) Transform::forward(-amount* .1,  eye,  center);
        else if (transop == scale) sy -= amount * 0.01 ;
        else if (transop == ltranslate) {
            lightposn[1] -= amount * 0.01;
        }
        break;
    }
    glutPostRedisplay();
}

void genShadowFrame(int frame_width, int frame_height) {
    // frame buffer
    glGenFramebuffersEXT(1, &s_frame_id);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, s_frame_id);
    
    // texture
    glGenTextures(1, &s_depth_texture_id);
    glBindTexture(GL_TEXTURE_2D, s_depth_texture_id);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, frame_width, frame_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, s_depth_texture_id, 0);
    
    if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT) {
        std::cerr << "glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) error code: " << glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) << std::endl;
    }
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    
}

void createCubeMap() {
    cubemap = glGetUniformLocation(shaderprogram, "cubemap");
    modelmatrix = glGetUniformLocation(shaderprogram, "modelmatrix");
    modelmatrixinversetranspose = glGetUniformLocation(shaderprogram, "modelmatrixinversetranspose");
    eye_world = glGetUniformLocation(shaderprogram, "eye_world");
    is_reflect = glGetUniformLocation(shaderprogram, "is_reflect");
    is_reflect_diffuse = glGetUniformLocation(shaderprogram, "is_reflect_diffuse");
    is_skybox = glGetUniformLocation(shaderprogram, "is_skybox");
    
    // cubemap
    std::vector<unsigned char *> cubemap_images;
    int cube_w, cube_h;
    cubemap_images.push_back(SOIL_load_image("environment/right.png", &cube_w, &cube_h, 0, SOIL_LOAD_RGBA));
    cubemap_images.push_back(SOIL_load_image("environment/front.png", &cube_w, &cube_h, 0, SOIL_LOAD_RGBA));
    cubemap_images.push_back(SOIL_load_image("environment/left.png", &cube_w, &cube_h, 0, SOIL_LOAD_RGBA));
    
    cubemap_images.push_back(SOIL_load_image("environment/back.png", &cube_w, &cube_h, 0, SOIL_LOAD_RGBA));
    cubemap_images.push_back(SOIL_load_image("environment/up.png", &cube_w, &cube_h, 0, SOIL_LOAD_RGBA));
    cubemap_images.push_back(SOIL_load_image("environment/down.png", &cube_w, &cube_h, 0, SOIL_LOAD_RGBA));
    glEnable(GL_TEXTURE_CUBE_MAP); 
    glGenTextures(1, &cubemap_texture_id); 
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture_id); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, cube_w, cube_h,0, GL_RGBA, GL_UNSIGNED_BYTE, cubemap_images[0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, cube_w, cube_h,0, GL_RGBA, GL_UNSIGNED_BYTE, cubemap_images[1]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, cube_w, cube_h,0, GL_RGBA, GL_UNSIGNED_BYTE, cubemap_images[2]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, cube_w, cube_h,0, GL_RGBA, GL_UNSIGNED_BYTE, cubemap_images[3]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, cube_w, cube_h,0, GL_RGBA, GL_UNSIGNED_BYTE, cubemap_images[4]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, cube_w, cube_h,0, GL_RGBA, GL_UNSIGNED_BYTE, cubemap_images[5]);
    
    for (std::vector<unsigned char *>::iterator it = cubemap_images.begin(); it != cubemap_images.end(); ++it) {
        SOIL_free_image_data( *it );
    }
    
}

void setupDisplacementMap() {
    displacementsampler = glGetUniformLocation(shaderprogram, "displacemap");
    
    floor_texture_map = load_texture("environment/floor_texture.jpg");
    floor_normal_map = load_texture("environment/floor_normal.jpg");
    floor_displacement_map = load_texture("environment/floor_height.jpg");
    is_displace = glGetUniformLocation(shaderprogram, "is_displace");
    is_bump = glGetUniformLocation(shaderprogram, "is_bump");
    is_tex = glGetUniformLocation(shaderprogram, "is_tex");
    
    texsampler = glGetUniformLocation(shaderprogram, "texmap");
    bumpsampler = glGetUniformLocation(shaderprogram, "bumpmap");
    tangent = glGetAttribLocation(shaderprogram, "tangent");
    bitangent = glGetAttribLocation(shaderprogram, "bitangent");
}

void init() {
    // Initialize shaders
    vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl") ;
    fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl") ;
    shaderprogram = initprogram(vertexshader, fragmentshader) ; 
    enablelighting = glGetUniformLocation(shaderprogram,"enablelighting") ;
    lightpos = glGetUniformLocation(shaderprogram,"lightposn") ;          
    lightcol = glGetUniformLocation(shaderprogram,"lightcolor") ;       
    numusedcol = glGetUniformLocation(shaderprogram,"numused") ;       
    ambientcol = glGetUniformLocation(shaderprogram,"ambient") ;       
    diffusecol = glGetUniformLocation(shaderprogram,"diffuse") ;       
    specularcol = glGetUniformLocation(shaderprogram,"specular") ;       
    emissioncol = glGetUniformLocation(shaderprogram,"emission") ;       
    shininesscol = glGetUniformLocation(shaderprogram,"shininess") ;       

    createCubeMap();
    setupDisplacementMap();
    
    // Init shadow map shaders + variables
    s_vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/shadow.vert.glsl");
    s_fragshader = initshaders(GL_FRAGMENT_SHADER, "shaders/shadow.frag.glsl");
    shadowprogram = initprogram(s_vertexshader, s_fragshader);
    depthmatrix = glGetUniformLocation(shadowprogram, "depthmatrix");
    depthbiasmatrix = glGetUniformLocation(shaderprogram, "depthbiasmatrix");
    shadowmap = glGetUniformLocation(shaderprogram, "shadowmap"); 
    use_shadow = glGetUniformLocation(shaderprogram, "use_shadow");
    use_pcf = glGetUniformLocation(shaderprogram, "use_pcf");
    shadow_buff_size = glGetUniformLocation(shaderprogram, "shadow_buff_size");
    genShadowFrame(1024, 1024);
}

int main(int argc, char* argv[]) {

	if (argc < 2) {
		cerr << "Usage: transforms scenefile [grader input (optional)]\n"; 
		exit(-1); 
	}

  	FreeImage_Initialise();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("High Quality Image Rendering");
	init();
        readfile(argv[1]) ; 
	glutDisplayFunc(display);
	glutSpecialFunc(specialKey);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutReshapeWindow(w, h);
    glutMotionFunc(drag);

	printHelp();
	glutMainLoop();
	FreeImage_DeInitialise();
	return 0;
}
