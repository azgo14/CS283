/*****************************************************************************/
/* This is the program skeleton for homework 1 in CS 184 by Ravi Ramamoorthi */
/* Based on an assignment designed by former TA Aner Ben-Artzi               */
/* Rewritten to be modern in terms of shaders etc.                           */
/* The idea is to draw a teapot, that can be moved by a crystal ball         */
/* interface                                                                 */
/*****************************************************************************/


#include <iostream>
#include <GL/glut.h>
#include "shaders.h"
#include "Transform.h"
#include <string>
#include <sstream>
#include <FreeImage.h>
#include <vector>
#include "mesh.h"
#include <algorithm>
#include "display.h"

int amount; // The amount of rotation for each arrow press

vec3 eye; // The (regularly updated) vector coordinates of the eye location 
vec3 up;  // The (regularly updated) vector coordinates of the up location 
const vec3 eyeinit(0.0,0.0,5.0); // Initial eye position, also for resets
const vec3 upinit(0.0,1.0,0.0); // Initial up position, also for resets
bool useGlu; // Toggle use of "official" opengl/glm transform vs user code
int w = 600, h = 600; // width and height 
int fovy = 90;
bool debug = false;
bool wireframe = false;

int slider = 0;
int slider_max;

GLuint vertexshader,fragmentshader,shaderprogram; // shaders

// Constants to set up lighting on the teapot
const GLfloat light_position[] = {0,5,10,1};    // Position of light 0
const GLfloat light_position1[] = {0,5,-10,1};  // Position of light 1
const GLfloat light_specular[] = {0.6,0.3,0,1};    // Specular of light 0
const GLfloat light_specular1[] = {0,0.3,0.6,1};   // Specular of light 1
const GLfloat one[] = {1,1,1,1};                 // Specular on teapot
const GLfloat medium[] = {0.5,0.5,0.5,1};        // Diffuse on teapot
const GLfloat small[] = {0.2,0.2,0.2,1};         // Ambient on teapot
const GLfloat high[] = {100};                      // Shininess of teapot
GLfloat light0[4],light1[4]; 

// Variables to set uniform params for lighting fragment shader 
GLuint islight; 
GLuint light0posn; 
GLuint light0color; 
GLuint light1posn; 
GLuint light1color; 
GLuint ambient; 
GLuint diffuse; 
GLuint specular; 
GLuint shininess; 
GLuint isdebug;

// Variables to keep track of mesh model
Mesh model;
int simplify_num;
bool pre_simplified = false;
char * collapse_file;

// New helper transformation function to transform vector by modelview 
// May be better done using newer glm functionality.
void transformvec (const GLfloat input[4],GLfloat output[4]) {
	GLfloat modelview[16]; // in column major order
	glGetFloatv(GL_MODELVIEW_MATRIX,modelview); 

	for (int i = 0; i < 4; i++) {
		output[i] = 0; 
		for (int j = 0; j < 4; j++) 
			output[i] += modelview[4*j+i] * input[j]; 
	}
}

std::string imgNumber(int num) {
	std::stringstream ss;
	//Return 3-digit number (or more if num > 999, but this case shouldn't be encountered)
	if(num < 10) {
		ss << "00" << num;
	} else if(num < 100) {
		ss << "0" << num;
	} else {
		ss << num;
	}
	return ss.str();
}


void printHelp() {
	std::cout << "\npress 'h' to print this message again.\n" 
		<< "press '+' or '-' to change the amount of resolution change.\n" 
		<< "press 'g' to switch between using glm::lookAt or your own LookAt.\n"     
		<< "press 'r' to reset the transformation (eye and up).\n"
		<< "press 'z' to zoom in. 'x' to zoom out.\n"
        << "press 'm' to decrease resolution. 'n' to increase.\n"
		<< "press ESC to quit.\n\n";  

}

// Uses the Projection matrices (technically deprecated) to set perspective 
// We could also do this in a more modern fashion with glm.  
void reshape(int width,int height){
	w = width;
	h = height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy,w/(float)h,0.1,99);
	glViewport(0,0,w,h);
}


void keyboard(unsigned char key,int x,int y) {
    switch(key) {
        case '+':
	    amount++;
            std::cout << "amount set to " << amount << "\n";
	    break;
	case '-':
	    amount--;
	    std::cout << "amount set to " << amount << "\n"; 
	    break;
	case 'g':
	    useGlu = !useGlu;
	    std::cout << "Using glm::LookAt set to: " 
		      << (useGlu ? " true " : " false ") << "\n"; 
	    break;
	case 'h':
            printHelp();
	    break;
	case 27:  // Escape to quit
            exit(0);
	    break;
	case 'r': // reset eye and up vectors 
	    eye = eyeinit; 
	    up = upinit; 
            break;
        case 'z':
            fovy -= amount;
            reshape(w,h);
            break;
        case 'x':
            fovy += amount;
            reshape(w,h);
            break;
        case 'd':
            debug = !debug;
            break;
        case 'w':
            wireframe = !wireframe;
            break;
        case 'm':
            if (slider < slider_max) {
                slider += amount;
                slider = std::min(slider, slider_max);
                model.setResolution(slider);
            }
            glutPostRedisplay();
            break;
        case 'n':
            if (slider > 0) {
                slider -= amount;
                slider = std::max(slider, 0);
                model.setResolution(slider);
            }
            glutPostRedisplay();      
            break;
        break;
    }
    glutPostRedisplay();
}

//  You will need to enter code for the arrow keys 
//  When an arrow key is pressed, it will call your transform functions

void specialKey(int key,int x,int y) {
	switch(key) {
		case 100: //left
			Transform::left(5,eye,up);
			break;
		case 101: //up
			Transform::up(5,eye,up);
			break;
		case 102: //right
			Transform::left(-5,eye,up);
			break;
		case 103: //down
			Transform::up(-5,eye,up);
			break;
	}
	glutPostRedisplay();
}


void init(const char* filename) {

	// Set up initial position for eye,up and amount
	// As well as booleans 

	eye = eyeinit; 
	up = upinit; 
	amount = 5;
	useGlu = true;

	glEnable(GL_DEPTH_TEST);

	// The lighting is enabled using the same framework as in mytest 3 
	// Except that we use two point lights
	// For now, lights and materials are set in display.  Will move to init 
	// later, per update lights

	vertexshader = initshaders(GL_VERTEX_SHADER,"shaders/light.vert.glsl");
	fragmentshader = initshaders(GL_FRAGMENT_SHADER,"shaders/light.frag.glsl");
	shaderprogram = initprogram(vertexshader,fragmentshader); 
	islight = glGetUniformLocation(shaderprogram,"islight"); 
        isdebug = glGetUniformLocation(shaderprogram, "debug");     
	light0posn = glGetUniformLocation(shaderprogram,"light0posn");       
	light0color = glGetUniformLocation(shaderprogram,"light0color");       
	light1posn = glGetUniformLocation(shaderprogram,"light1posn");       
	light1color = glGetUniformLocation(shaderprogram,"light1color");       
	ambient = glGetUniformLocation(shaderprogram,"ambient");       
	diffuse = glGetUniformLocation(shaderprogram,"diffuse");       
	specular = glGetUniformLocation(shaderprogram,"specular");       
	shininess = glGetUniformLocation(shaderprogram,"shininess");     
	
	std::cout << "Loading in file: " << filename << std::endl;
    if (!pre_simplified) {
        Mesh temp;
        temp.loadMesh(filename);
        simplify_num = temp._faces.size();
        temp.quadricSimplify(simplify_num);
    }
    
    model.loadMesh(filename);
    if (!pre_simplified) {
        model.loadEdgeCollapse("edge_collapse.txt");
        
    } else {
        model.loadEdgeCollapse(collapse_file);
    }
    slider_max = model.numOfCollapse();
    std::cout << "Done loading" << std::endl;  
}

void shared::display() {
    glClearColor(0,0,1,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (wireframe) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    } else {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
    glMatrixMode(GL_MODELVIEW);
    mat4 mv; 
    const vec3 center(0.0,0.0,0.0); 

    if (useGlu) mv = glm::lookAt(eye,center,up); 
    else {
        mv = Transform::lookAt(eye,up); 
        mv = glm::transpose(mv); // accounting for row major
    }
    glLoadMatrixf(&mv[0][0]); 

    // Set Light and Material properties for the teapot
    // Lights are transformed by current modelview matrix. 
    // The shader can't do this globally. 
    // So we need to do so manually.  
    transformvec(light_position,light0); 
    transformvec(light_position1,light1); 

    glUniform4fv(light0posn,1,light0); 
    glUniform4fv(light0color,1,light_specular); 
    glUniform4fv(light1posn,1,light1); 
    glUniform4fv(light1color,1,light_specular1); 

    //glUniform4fv(ambient,1,small); 
    //glUniform4fv(diffuse,1,medium); 
    glUniform4fv(ambient,1,small); 
    glUniform4fv(diffuse,1,small); 
    glUniform4fv(specular,1,one); 
    glUniform1fv(shininess,1,high); 
    glUniform1i(islight,true);
    glUniform1i(isdebug, false);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    if (debug) {
        glUniform1i(isdebug, true);
        glEnableClientState(GL_COLOR_ARRAY);
        std::vector<glm::vec3> colors;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        model.debugVerts(&vertices);
        model.debugVertColors(&colors);
        model.debugNorms(&normals);
        glColorPointer(3, GL_FLOAT, 0, &(colors)[0]);  
        glVertexPointer(3, GL_FLOAT, 0, &(vertices)[0]);
        glNormalPointer(GL_FLOAT, 0, &(normals)[0]);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        glDisableClientState(GL_COLOR_ARRAY);
    } else {
        glVertexPointer(3, GL_FLOAT, 0, &(model._vertices)[0]);
        glNormalPointer(GL_FLOAT, 0, &(model._normals)[0]);
        glDrawElements(GL_TRIANGLES, model._faces.size(), GL_UNSIGNED_INT, &(model._faces)[0]);  
        
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    
    glutSwapBuffers();
}

int main(int argc,char* argv[]) {
    if(argc != 2 && argc != 3) {
        std::cerr << "Usage: path to the OFF file" << std::endl;
        throw 2;
    }
    
    if (argc == 3) {
        pre_simplified = true;
        collapse_file = argv[2];
    }
    printHelp();
	
	//Initialize GLUT
	FreeImage_Initialise();
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("HW1: Meshes");
	init(argv[1]);

	glutDisplayFunc(shared::display);
	glutSpecialFunc(specialKey);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutReshapeWindow(w,h);
	
	glutMainLoop();	
	FreeImage_DeInitialise();

	return 0;
}
