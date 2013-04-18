/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

// This file is display.cpp.  It includes the skeleton for the display routine

// Basic includes to get this file to work.
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "Transform.h"

using namespace std ;
#include "variables.h"
#include "readfile.h"


void printMatrix(mat4 matrix) {
  matrix = glm::transpose(matrix);
  std::cout <<"\n"<< matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " << matrix[0][3] <<"\n"
            << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " << matrix[1][3] << "\n"
            << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " << matrix[2][3] << "\n"
          << matrix[3][0] << " " << matrix[3][1] << " " << matrix[3][2] << " " << matrix[3][3] << "\n" <<"\n";

}

// New helper transformation function to transform vector by modelview
// May be better done using newer glm functionality.
// Provided for your convenience.  Use is optional.
// Some of you may want to use the more modern routines in readfile.cpp
// that can also be used.

void transformvec (const GLfloat input[4], GLfloat output[4]) {
    GLfloat modelview[16] ; // in column major order
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview) ;

    for (int i = 0 ; i < 4 ; i++) {
        output[i] = 0 ;
        for (int j = 0 ; j < 4 ; j++)
            output[i] += modelview[4*j+i] * input[j] ;
    }
}

void drawScene() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glViewport(0,0,w,h);
    glUseProgram(shaderprogram);
    glClearColor(0, 0, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, s_depth_texture_id);
    glUniform1i(shadowmap, 0);
    // I'm including the basic matrix setup for model view to
    // give some sense of how this works.
    glUniform1i(use_pcf, pcf_bool) ; 
    glUniform1i(use_shadow, shadow_bool) ;
    glUniform1f(shadow_buff_size, 1024.0);
    glMatrixMode(GL_MODELVIEW);
    mat4 mv ;

    if (useGlu) mv = glm::lookAt(eye,center,up) ;
    else {
        mv = Transform::lookAt(eye,center,up) ;
        mv = glm::transpose(mv) ; // accounting for row major
    }
    glLoadMatrixf(&mv[0][0]) ;

    // Transformations for objects, involving translation and scaling
    mat4 sc(1.0) , tr(1.0), transf(1.0) ;
    sc = Transform::scale(sx,sy,1.0) ;
    mv = glm::transpose(mv); // mv already transposed above. Back to row-order

    // Set Light and Material properties for the teapot
    // Lights are transformed by current modelview matrix.
    // The shader can't do this globally.
    // So we need to do so manually.
    if (numused) {
        glUniform1i(enablelighting,true) ;
        for (int i = 0; i < numused; ++i) {
            tr = Transform::translate(ltx[i],lty[i],0.0) ;
            transf = glm::transpose(tr * mv);  // Scaling a light doesn't make sense
            glLoadMatrixf(&transf[0][0]) ;
            GLfloat temppos[4] = { lightposn[4*i], lightposn[4*i+1], lightposn[4*i+2], lightposn[4*i+3] };
            GLfloat transformpos[4];
            transformvec (temppos, transformpos);
            for (int j = 0; j < 4; ++j) {
                lightransf[4*i + j] = transformpos[j];
            }
        }
        glUniform4fv(lightcol, numused, lightcolor);
        glUniform1i(numusedcol, numused);
        glUniform4fv(lightpos, numused, lightransf);
        // You need to pass the lights to the shader.
        // Remember that lights are transformed by modelview first.

    }
    else glUniform1i(enablelighting,false) ;

    tr = Transform::translate(tx,ty,0.0) ;

    mv = glm::transpose(mv); // back to column major
    glLoadMatrixf(&mv[0][0]) ;  // set back to just mv

    transf = glm::transpose(sc * tr * glm::transpose(mv));  // column major
    // You need to use scale, translate and modelview to
    // set up the net transformation matrix for the objects.
    // Account for GLM issues etc.
    glLoadMatrixf(&transf[0][0]) ;

    for (int i = 0 ; i < numobjects ; i++) {
        object * obj = &(objects[i]) ;

        {            
            // Will be (in order of multiply) =  (mv) (tr) (scale) (obj->transform)
            mat4 modelview = glm::transpose((obj -> transform) * glm::transpose(transf)) ;
            glLoadMatrixf(&modelview[0][0]) ;
            glUniform4fv(ambientcol, 1, (obj -> ambient));
            glUniform4fv(diffusecol, 1, (obj -> diffuse));
            glUniform4fv(specularcol, 1, (obj -> specular));
            glUniform4fv(emissioncol, 1, (obj -> emission));
            glUniform1f(shininesscol, (obj -> shininess));
            
            // depth matrix
            mat4 transform = obj -> transform;
            glm::mat4 depthProjectionMatrix = glm::ortho<float>(-2,2,-2,2,-2,4);
            glm::mat4 depthViewMatrix = glm::lookAt(inverse_light_dir, glm::vec3(0,0,0), glm::vec3(0,1,0));
            glm::mat4 depthModelMatrix = glm::mat4(1.0);
            glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
            depthMVP = glm::transpose(sc*tr*glm::transpose(depthMVP));
            depthMVP = glm::transpose(transform * glm::transpose(depthMVP));
            glm::mat4 biasMatrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
            );
            depthMVP = biasMatrix * depthMVP;
            glUniformMatrix4fv(depthbiasmatrix, 1, GL_FALSE, &depthMVP[0][0]);
            

            // Set up the object transformations
            // And pass in the appropriate material properties
        }

        // Actually draw the object
        // We provide the actual glut drawing functions for you.
        if (obj -> type == cube) {
            glutSolidCube(obj->size) ;
        }
        else if (obj -> type == sphere) {
            const int tessel = 20 ;
            glutSolidSphere(obj->size, tessel, tessel) ;
        }
        else if (obj -> type == teapot) {
            glutSolidTeapot(obj->size) ;
        }

    }

}

void drawShadowMap() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, s_frame_id);
    glViewport(0,0,1024,1024);

    glUseProgram(shadowprogram);
    glClearColor(1, 1, 1, 1);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    mat4 mv ;
    if (useGlu) mv = glm::lookAt(eye,center,up) ;
    else {
        mv = Transform::lookAt(eye,center,up) ;
        mv = glm::transpose(mv) ; // accounting for row major
    }
    glLoadMatrixf(&mv[0][0]) ;
    for (int i = 0; i < numused; i++) {
        const GLfloat _light[] = {lightposn[4*i], lightposn[4*i+1], lightposn[4*i+2], lightposn[4*i+3]};
        if (_light[3] == 0) {
            inverse_light_dir = glm::vec3(_light[0],_light[1],_light[2]);
            break;
        }
    }

    // Transformations for objects, involving translation and scaling
    mat4 sc(1.0) , tr(1.0), transf(1.0) ;
    sc = Transform::scale(sx,sy,1.0) ;
    glLoadMatrixf(&transf[0][0]) ;
    transf = sc * glm::transpose(mv) * transf;
    tr = Transform::translate(tx,ty,0.0) ;

    for (int i = 0 ; i < numobjects ; i++) {
        object * obj = &(objects[i]) ;
        std::cout << i<<std::endl;
            mat4 transform = obj -> transform;
            glm::mat4 depthProjectionMatrix = glm::ortho<float>(-2,2,-2,2,-2,4);
            glm::mat4 depthViewMatrix = glm::lookAt(inverse_light_dir, glm::vec3(0,0,0), glm::vec3(0,1,0));
            glm::mat4 depthModelMatrix = glm::mat4(1.0);
            glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
            depthMVP = glm::transpose(sc*tr*glm::transpose(depthMVP));
            depthMVP = glm::transpose(transform * glm::transpose(depthMVP));
            if (obj -> type == teapot) 
                printMatrix(transform);
            
            glUniformMatrix4fv(depthmatrix, 1, GL_FALSE, &depthMVP[0][0]);

        
        // Actually draw the object
        // We provide the actual glut drawing functions for you.
        if (obj -> type == cube) {
            glutSolidCube(obj->size) ;
        }
        else if (obj -> type == sphere) {
            const int tessel = 20 ;
            glutSolidSphere(obj->size, tessel, tessel) ;
        }
        else if (obj -> type == teapot) {
            glutSolidTeapot(obj->size) ;
        }
    }
}


void display() {
    drawShadowMap();
    drawScene();
    glutSwapBuffers();
    
}
