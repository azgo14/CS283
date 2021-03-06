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
#include <vector>
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

void computeTangents(const vector<glm::vec3> & vertices, const vector<glm::vec3> & normals,
                     const vector<glm::vec2> & textures, vector<glm::vec3> * tangents,
                     vector<glm::vec3> * bitangents) {
    for (int tri = 0; tri < vertices.size(); tri=tri+3) {
        glm::vec3 changeVert1 = vertices[tri+1] - vertices[tri];
        glm::vec3 changeVert2 = vertices[tri+2] - vertices[tri];
        glm::vec2 changeTex1 = textures[tri+1] - textures[tri];
        glm::vec2 changeTex2 = textures[tri+2] - textures[tri];
       
        float scale = static_cast<float>(1) / (changeTex1.x * changeTex2.y - changeTex1.y * changeTex2.x);
        glm::vec3 tangent = scale * (changeVert1 * changeTex2.y - changeVert2 * changeTex1.y);
        glm::vec3 bitangent = scale * (changeVert2 * changeTex1.x - changeVert1 * changeTex2.x);
        tangent = glm::normalize(tangent - normals[tri] * glm::dot(normals[tri], tangent));
        tangents->push_back(tangent);
        tangents->push_back(tangent);
        tangents->push_back(tangent);
        bitangents->push_back(bitangent);
        bitangents->push_back(bitangent);
        bitangents->push_back(bitangent);
    }
}


std::vector<glm::vec3> cube_vertices, cube_normals, cube_textures;
void init_cube() {
    double width = 100;
    double length = 100;
    double height = 100;
    double y_start = -50;
    double pad = 10;
    
    if (cube_vertices.size() == 0) {
        //left
        cube_vertices.push_back(glm::vec3(-width/2,y_start-pad,length/2+pad));
        cube_vertices.push_back(glm::vec3(-width/2,y_start-pad,-length/2-pad));
        cube_vertices.push_back(glm::vec3(-width/2,y_start+height+pad,length/2+pad));    
        cube_vertices.push_back(glm::vec3(-width/2,y_start-pad,-length/2-pad));
        cube_vertices.push_back(glm::vec3(-width/2,y_start+height+pad,-length/2-pad));
        cube_vertices.push_back(glm::vec3(-width/2,y_start+height+pad,length/2+pad));
        cube_normals.push_back(glm::vec3(1,0,0));
        cube_normals.push_back(glm::vec3(1,0,0));
        cube_normals.push_back(glm::vec3(1,0,0));
        cube_normals.push_back(glm::vec3(1,0,0));
        cube_normals.push_back(glm::vec3(1,0,0));
        cube_normals.push_back(glm::vec3(1,0,0));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2,y_start-pad,length/2+pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2,y_start-pad,-length/2-pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2,y_start+height+pad,length/2+pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2,y_start-pad,-length/2-pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2,y_start+height+pad,-length/2-pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2,y_start+height+pad,length/2+pad)));
    
    
        //right
        cube_vertices.push_back(glm::vec3(width/2,y_start-pad,length/2+pad));
        cube_vertices.push_back(glm::vec3(width/2,y_start+height+pad,length/2+pad));
        cube_vertices.push_back(glm::vec3(width/2,y_start-pad,-length/2-pad));
        cube_vertices.push_back(glm::vec3(width/2,y_start-pad,-length/2-pad));
        cube_vertices.push_back(glm::vec3(width/2,y_start+height+pad,length/2+pad));
        cube_vertices.push_back(glm::vec3(width/2,y_start+height+pad,-length/2-pad));
        cube_normals.push_back(glm::vec3(-1,0,0));
        cube_normals.push_back(glm::vec3(-1,0,0));
        cube_normals.push_back(glm::vec3(-1,0,0));
        cube_normals.push_back(glm::vec3(-1,0,0));
        cube_normals.push_back(glm::vec3(-1,0,0));
        cube_normals.push_back(glm::vec3(-1,0,0));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2,y_start-pad,length/2+pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2,y_start+height+pad,length/2+pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2,y_start-pad,-length/2-pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2,y_start-pad,-length/2-pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2,y_start+height+pad,length/2+pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2,y_start+height+pad,-length/2-pad)));

        //top
        cube_vertices.push_back(glm::vec3(-width/2-pad,y_start+height,length/2+pad));
        cube_vertices.push_back(glm::vec3(-width/2-pad,y_start+height,-length/2-pad));
        cube_vertices.push_back(glm::vec3(width/2+pad,y_start+height,-length/2-pad));
        cube_vertices.push_back(glm::vec3(width/2+pad,y_start+height,-length/2-pad));
        cube_vertices.push_back(glm::vec3(width/2+pad,y_start+height,length/2+pad));
        cube_vertices.push_back(glm::vec3(-width/2-pad,y_start+height,length/2+pad));
        cube_normals.push_back(glm::vec3(0,-1,0));
        cube_normals.push_back(glm::vec3(0,-1,0));
        cube_normals.push_back(glm::vec3(0,-1,0));
        cube_normals.push_back(glm::vec3(0,-1,0));
        cube_normals.push_back(glm::vec3(0,-1,0));
        cube_normals.push_back(glm::vec3(0,-1,0));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2-pad,y_start+height,length/2+pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2-pad,y_start+height,-length/2-pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2+pad,y_start+height,-length/2-pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2+pad,y_start+height,-length/2-pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2+pad,y_start+height,length/2+pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2-pad,y_start+height,length/2+pad)));

        //bottom
        cube_vertices.push_back(glm::vec3(-width/2-pad,y_start,length/2+pad));
        cube_vertices.push_back(glm::vec3(width/2+pad,y_start,-length/2-pad));
        cube_vertices.push_back(glm::vec3(-width/2-pad,y_start,-length/2-pad));
        cube_vertices.push_back(glm::vec3(width/2+pad,y_start,-length/2-pad));
        cube_vertices.push_back(glm::vec3(-width/2-pad,y_start,length/2+pad));
        cube_vertices.push_back(glm::vec3(width/2+pad,y_start,length/2+pad));
        cube_normals.push_back(glm::vec3(0,1,0));
        cube_normals.push_back(glm::vec3(0,1,0));
        cube_normals.push_back(glm::vec3(0,1,0));
        cube_normals.push_back(glm::vec3(0,1,0));
        cube_normals.push_back(glm::vec3(0,1,0));
        cube_normals.push_back(glm::vec3(0,1,0));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2-pad,y_start,length/2+pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2+pad,y_start,-length/2-pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2-pad,y_start,-length/2-pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2+pad,y_start,-length/2-pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2-pad,y_start,length/2+pad)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2+pad,y_start,length/2+pad)));

        //near
        cube_vertices.push_back(glm::vec3(-width/2-pad,y_start-pad,length/2));
        cube_vertices.push_back(glm::vec3(width/2+pad,y_start+height+pad,length/2));
        cube_vertices.push_back(glm::vec3(width/2+pad,y_start-pad,length/2));
        cube_vertices.push_back(glm::vec3(-width/2-pad,y_start-pad,length/2));
        cube_vertices.push_back(glm::vec3(-width/2-pad,y_start+height+pad,length/2));
        cube_vertices.push_back(glm::vec3(width/2+pad,y_start+height+pad,length/2));
        cube_normals.push_back(glm::vec3(0,0,-1));
        cube_normals.push_back(glm::vec3(0,0,-1));
        cube_normals.push_back(glm::vec3(0,0,-1));
        cube_normals.push_back(glm::vec3(0,0,-1));
        cube_normals.push_back(glm::vec3(0,0,-1));
        cube_normals.push_back(glm::vec3(0,0,-1));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2-pad,y_start-pad,length/2)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2+pad,y_start+height+pad,length/2)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2+pad,y_start-pad,length/2)));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2-pad,y_start-pad,length/2)));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2-pad,y_start+height+pad,length/2)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2+pad,y_start+height+pad,length/2)));

        //far
        cube_vertices.push_back(glm::vec3(-width/2-pad,y_start-pad,-length/2));
        cube_vertices.push_back(glm::vec3(width/2+pad,y_start-pad,-length/2));
        cube_vertices.push_back(glm::vec3(width/2+pad,y_start+height+pad,-length/2));
        cube_vertices.push_back(glm::vec3(-width/2-pad,y_start-pad,-length/2));
        cube_vertices.push_back(glm::vec3(width/2+pad,y_start+height+pad,-length/2));
        cube_vertices.push_back(glm::vec3(-width/2-pad,y_start+height+pad,-length/2));
        cube_normals.push_back(glm::vec3(0,0,1));
        cube_normals.push_back(glm::vec3(0,0,1));
        cube_normals.push_back(glm::vec3(0,0,1));
        cube_normals.push_back(glm::vec3(0,0,1));
        cube_normals.push_back(glm::vec3(0,0,1));
        cube_normals.push_back(glm::vec3(0,0,1));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2-pad,y_start-pad,-length/2)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2+pad,y_start-pad,-length/2)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2+pad,y_start+height+pad,-length/2)));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2-pad,y_start-pad,-length/2)));
        cube_textures.push_back(glm::normalize(glm::vec3(width/2+pad,y_start+height+pad,-length/2)));
        cube_textures.push_back(glm::normalize(glm::vec3(-width/2-pad,y_start+height+pad,-length/2)));
    }
}

std::vector<glm::vec3> floor_vertices, floor_normals, floor_tangents, floor_bitangents;
std::vector<glm::vec2> floor_textures;
void init_floor() {
    if (floor_vertices.size() == 0) {
        double width = 1;
        double length = 1;
        double height = 0;
        float divide = 500.0;
        float increment_height = 4.0/divide;
        float increment_width = 4.0/divide;
        int t_x = 0;
        int t_y = 0;
        for (float x = -width/2; x < width/2; x+=width/divide) {
          t_y = 0;
          for (float y = length/2; y > -length/2; y-=length/divide) {
            glm::vec3 v1 = glm::vec3(x+width/divide, -height/2, y);
            glm::vec3 v2 = glm::vec3(x+width/divide, -height/2, y-length/divide);
            glm::vec3 v3 = glm::vec3(x, -height/2, y-length/divide);
            glm::vec3 v4 = glm::vec3(x, -height/2, y);
            // bottom face
            floor_vertices.push_back(v4);
            floor_vertices.push_back(v1);
            floor_vertices.push_back(v2);
            floor_vertices.push_back(v4);
            floor_vertices.push_back(v2);
            floor_vertices.push_back(v3);
            floor_normals.push_back(glm::vec3(0, 1, 0));
            floor_normals.push_back(glm::vec3(0, 1, 0));
            floor_normals.push_back(glm::vec3(0, 1, 0));
            floor_normals.push_back(glm::vec3(0, 1, 0));
            floor_normals.push_back(glm::vec3(0, 1, 0));
            floor_normals.push_back(glm::vec3(0, 1, 0));
            floor_textures.push_back(glm::vec2(increment_width * t_x, increment_height * t_y));
            floor_textures.push_back(glm::vec2(increment_width *(t_x+1), increment_height * t_y));
            floor_textures.push_back(glm::vec2(increment_width *(t_x+1), increment_height * (t_y + 1)));
            floor_textures.push_back(glm::vec2(increment_width * t_x, increment_height * t_y));
            floor_textures.push_back(glm::vec2(increment_width *(t_x+1), increment_height * (t_y + 1)));
            floor_textures.push_back(glm::vec2(increment_width * t_x, increment_height * (t_y + 1)));
            t_y++;
          }      
          t_x++;
        }
        computeTangents(floor_vertices, floor_normals, floor_textures, &floor_tangents, &floor_bitangents);
    }
    
}

void drawFloor() {
    init_floor();
    glUniform1i(is_displace, use_dmap);
    glUniform1i(is_bump, use_bump);
    glUniform1i(is_tex, use_tex);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floor_normal_map);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glUniform1i(bumpsampler, 0);
    
    glActiveTexture(GL_TEXTURE1);    
    glBindTexture(GL_TEXTURE_2D, floor_texture_map);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glUniform1i(texsampler, 1);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, floor_displacement_map);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glUniform1i(displacementsampler, 2);
    
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, &floor_textures[0]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &floor_vertices[0]);
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, &floor_normals[0]);
    glEnableVertexAttribArray(tangent);
    glVertexAttribPointer(tangent, 3, GL_FLOAT, GL_FALSE, 0, &floor_tangents[0]);
    glEnableVertexAttribArray(bitangent);
    glVertexAttribPointer(bitangent, 3, GL_FLOAT, GL_FALSE, 0, &floor_bitangents[0]);
    glDrawArrays(GL_TRIANGLES, 0, floor_vertices.size());
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glUniform1i(is_displace, false);
    glUniform1i(is_bump, false);
    glUniform1i(is_tex, false);
    
}

void drawSkyBox() {
    init_cube();
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, 0, &cube_textures[0]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &cube_vertices[0]);
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, &cube_normals[0]);
    glDrawArrays(GL_TRIANGLES, 0, cube_vertices.size());
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void drawScene() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glViewport(0,0,w,h);
    glUseProgram(shaderprogram);
    glClearColor(0, 0, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    mat4 mv ;

    if (useGlu) mv = glm::lookAt(eye,center,up) ;
    else {
        mv = Transform::lookAt(eye,center,up) ;
        mv = glm::transpose(mv) ; // accounting for row major
    }
    glLoadMatrixf(&mv[0][0]) ;
    
    glActiveTexture(GL_TEXTURE4);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture_id);
    glUniform1i(cubemap, 4);
    glUniform1i(is_skybox, true);
    drawSkyBox();
    glUniform1i(is_skybox, false);
    
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, s_depth_texture_id);
    glUniform1i(shadowmap, 3);
    // I'm including the basic matrix setup for model view to
    // give some sense of how this works.
    glUniform1i(use_pcf, pcf_bool) ; 
    glUniform1i(use_shadow, shadow_bool) ;
    glUniform1f(shadow_buff_size, 1024.0);
    

    glUniform3fv(eye_world, 1, &eye[0]);
    glUniform1i(is_reflect_diffuse, reflect_diffuse_bool) ;


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
            mat4 modelm = glm::transpose((obj -> transform) * sc * tr);
            modelm = glm::transpose((obj -> transform) * sc * tr);
            
            glUniformMatrix4fv(modelmatrix, 1, GL_FALSE, &modelm[0][0]);
            
            modelm = glm::inverse(glm::transpose((obj -> transform) * sc * tr)); // still in row order so naturally transposed in col order glsl
            
            glUniformMatrix4fv(modelmatrixinversetranspose, 1, GL_FALSE, &modelm[0][0]);
            
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
        } else if (obj -> type == ground) {
            drawFloor();
        } else if (obj -> type == reflectsphere) {
            glUniform1i(is_reflect, true) ; 
            const int tessel = 100 ;
            glutSolidSphere(obj->size, tessel, tessel) ;
        } else if (obj -> type == sphere) {
            const int tessel = 100 ;
            glutSolidSphere(obj->size, tessel, tessel) ;
        } else if (obj -> type == teapot) {
            glutSolidTeapot(obj->size) ;
        }
        glUniform1i(is_reflect, false) ;

    }

}

void drawShadowMap() {
    if (display_sm) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    } else {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, s_frame_id);
    }
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
        } else if (obj -> type == ground) {
            drawFloor();
        }
        else if (obj -> type == sphere || obj -> type == reflectsphere) {
            const int tessel = 100 ;
            glutSolidSphere(obj->size, tessel, tessel) ;
        }
        else if (obj -> type == teapot) {
            glutSolidTeapot(obj->size) ;
        }
    }
}


void display() {
    drawShadowMap();
    if (!display_sm) {
        drawScene();
    }
    glutSwapBuffers();
    
}
