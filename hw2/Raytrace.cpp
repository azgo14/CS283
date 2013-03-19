#include <limits>
#include <iostream>

#include <utility>
#include <FreeImage.h>
#include "variables.h"
#include "Raytrace.h"
#include <omp.h>
namespace {
void getPrunnedObjs(BoundingBox* node, const vec3& source, const vec3& direction, std::vector<std::pair<Object*, vec3> > * results) {
    std::pair<bool, glm::vec3> bb_result = node->intersect(source,direction);
    if (bb_result.first) {
        if (node->_obj != NULL) {
            results->push_back(std::make_pair(node->_obj, bb_result.second));
        }
        if (node->_left_box != NULL) {
            getPrunnedObjs(node->_left_box, source, direction, results);
        }
        if (node->_right_box != NULL) {
            getPrunnedObjs(node->_right_box, source, direction, results);
        }
    }
}

std::pair<Object*, vec3> calculateIntersection(const vec3& eye, const vec3& ray_direction) {
    float min_distance = std::numeric_limits<float>::max();
    Object* i_obj = NULL;
    glm::vec3 intersection;
    std::vector<std::pair<Object*, vec3> > prunned_objects;
    getPrunnedObjs(root_box, eye, ray_direction, &prunned_objects);
    //std::cout << "Eye: " << eye.x << " " << eye.y << " " << eye.z << std::endl;
    //std::cout << "Ray Direction: " << ray_direction.x << " " << ray_direction.y << " " << ray_direction.z << std::endl;
    //std::cout << "Prunned size: " << prunned_objects.size() << std::endl;
    for (std::vector<std::pair<Object*, vec3> >::iterator it = prunned_objects.begin(); it != prunned_objects.end(); ++it) {
        //std::cout << "Intersect location: " << it->second.x << " " << it->second.y << " " << it->second.z << std::endl;
        if (min_distance != std::numeric_limits<float>::max() &&
            it->second.x != std::numeric_limits<float>::max() &&
            it->second.y != std::numeric_limits<float>::max() &&
            it->second.z != std::numeric_limits<float>::max()) {  // another optimization
            glm::vec3 difference = it->second - eye;
            float dist = glm::dot(difference, difference);
            if (dist > min_distance) {
                continue;
            }
        }
        std::pair<bool, glm::vec3> result = (it->first)->intersect(eye, ray_direction);
        if (result.first) {
            glm::vec3 difference = result.second - eye;
            //std::cout << "True intersect: " << result.second.x << " " << result.second.y << " " << result.second.z << std::endl;
            float dist = glm::dot(difference, difference);
            if (dist < min_distance) {
                min_distance = dist;
                i_obj = it->first;
                intersection = result.second;
            }
        }
    }
    return std::make_pair(i_obj, intersection);
}

// Returns true if in shadow
bool checkInShadow(vec3 shadow_origin, vec3 shadow_direction, float dist_to_light) {
    std::vector<std::pair<Object*, vec3> > prunned_objects;
    getPrunnedObjs(root_box, shadow_origin, shadow_direction, &prunned_objects);

    for (std::vector<std::pair<Object*, vec3> >::iterator it = prunned_objects.begin(); it != prunned_objects.end(); ++it) {
        if (it->first->isLight) {
            //std::cout <<"Hit light" <<std::endl;
            continue;
        }
        std::pair<bool, glm::vec3> result = (it->first)->intersect(shadow_origin, shadow_direction);
        if (result.first) {
            vec3 diff_vec = glm::vec3(result.second.x - shadow_origin.x,
                                      result.second.y - shadow_origin.y,
                                      result.second.z - shadow_origin.z);
            float diff_dist = glm::dot(diff_vec, diff_vec);
            if (diff_dist < dist_to_light) {
                return true;
            }
        }
    }
    return false;
}
} // namespace

void Raytrace::raytrace (const vec3& eye, const vec3& center, const vec3& up, float fovx, float fovy, int width, int height, FIBITMAP* bitmap, int recurse) {
    int count = 0;
    //#pragma omp parallel for
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            glm::vec3 ray_direction = calculateRay(eye, center, up, fovx, fovy, width, height, static_cast<float>(i)+.5, static_cast<float>(j)+.5);
            
            std::pair<Object*, vec3> i_result = calculateIntersection(eye, ray_direction);
            //std::cout << "Intersection: " << i_result.second.x << " " << i_result.second.y << " " << i_result.second.z << std::endl;
            Object* i_obj = i_result.first;
            glm::vec3 intersection = i_result.second;
            if (i_obj != NULL) {
                vec4 phongColor = vec4(0,0,0,0);
                for (int ray_i = 0; ray_i < ray_per_pixel; ++ray_i) {
                    phongColor += (1 / static_cast<float>(ray_per_pixel)) * calculateColor(i_obj, intersection, eye, recurse);
                }
                
                RGBQUAD color;
                color.rgbBlue = 255 * phongColor.x;
                color.rgbGreen = 255 * phongColor.y;
                color.rgbRed = 255 * phongColor.z;
                //std::cout << "Color: " << 255 * phongColor.z << " " << 255 * phongColor.y << " " << 255 * phongColor.x << std::endl;
                FreeImage_SetPixelColor(bitmap, j, height - i - 1, &color);
            }
        }
        std::cout << ++count << " rows out of " << height << " rows calculated.\n";
    }
}


// returns the direction of the ray
glm::vec3 Raytrace::calculateRay(const vec3& eye, const vec3& center, const vec3& up, float fovx, float fovy, int width, int height, float i, float j) {
    vec3 w = glm::normalize(eye - center);
    vec3 u = glm::normalize(glm::cross(up, w));
    vec3 v = glm::cross(w, u);
    float a = glm::tan(glm::radians(fovx/2)) * ((j-(static_cast<float>(width)/2))/(static_cast<float>(width)/2));
    float b = glm::tan(glm::radians(fovy/2)) * (((static_cast<float>(height)/2)-i)/(static_cast<float>(height)/2));
    return glm::normalize(a*u + b*v - w);
}

glm::vec4 phongIllumination(const vec3& normal, const vec3& direction, const vec3& halfAngle, const vec4& lightcolor, float distance, const vec4& obj_diffuse, const vec4& obj_specular, GLfloat obj_shininess) {
    float nDotL = glm::dot(normal, direction);
    if (nDotL < 0) {
        nDotL = 0;
    }
    vec4 diffuseTerm = glm::vec4(obj_diffuse.x * nDotL, obj_diffuse.y * nDotL, obj_diffuse.z * nDotL, obj_diffuse.w * nDotL);
    float nDotH = glm::dot(normal, halfAngle);
    if (nDotH < 0) {
        nDotH = 0;
    }
    float shine = glm::pow(nDotH, obj_shininess);
    vec4 specularTerm = glm::vec4(obj_specular.x * shine, obj_specular.y * shine, obj_specular.z * shine, obj_specular.w * shine);
    
    return (lightcolor / (attenuation.x + attenuation.y * distance + attenuation.z * distance * distance)) * (diffuseTerm + specularTerm);

}

namespace {
vec3 sampleFromHemiSphereUniform(vec3 hemisphere_normal) {
    float theta, azm;
    vec3 point;
    do {
        theta = (rand() / static_cast<float>(RAND_MAX)) * M_PI;
        azm = (rand() / static_cast<float>(RAND_MAX)) * 2 * M_PI;
        point = vec3(sin(theta)*cos(azm), sin(theta)*sin(azm), cos(theta));
    } while (glm::dot(point, hemisphere_normal) <= 0);
    return point;
}
} // namespace

glm::vec4 Raytrace::calculateColor(Object * obj, const vec3& intersection, const vec3& eye, int recurse) {
    if (obj->isLight) {
        return obj->_emission;
    }
    vec4 finalcolor = vec4(0, 0, 0, 0);
    vec3 eyedir = glm::normalize(eye-intersection);
    vec3 normal = obj->getNormal(intersection);
    vec3 shadow_direction, halfAngle;
    float distance;
    float increment = .001;
    //std::cout<<"Num of Lights: " << lights.size()<<std::endl;
    for (int i = 0; i < lights.size(); i++) {
        // shadow
        vec4 lightpos = lights[i]->getLightPosn();
        //std::cout << "Light Posn: " << lightpos.x << " " <<  lightpos.y << " " << lightpos.z << std::endl;
        // area lighting
        vec3 dir_vec = glm::vec3(lightpos[0] - intersection.x,
                                 lightpos[1] - intersection.y,
                                 lightpos[2] - intersection.z);
        shadow_direction = glm::normalize(dir_vec);
        vec3 shadow_origin = intersection + increment * shadow_direction;
        float dist_to_light = glm::dot(dir_vec,dir_vec);
        //std::cout << "Shadow Origin: " << shadow_origin.x << " " << shadow_origin.y << " " << shadow_origin.z << std::endl;
        //std::cout << "Distance to Light " << dist_to_light << std::endl;
        if (checkInShadow(shadow_origin, shadow_direction, dist_to_light)) {            
            continue;
        }
        
        halfAngle = glm::normalize(shadow_direction + eyedir);
        distance = sqrt(dist_to_light);
        
        vec4 color = lights[i]->_emission;
        
        finalcolor += .5 * phongIllumination(normal, shadow_direction, halfAngle, color, distance, obj->_diffuse, obj->_specular, obj->_shininess);

        //std::cout<< finalcolor.x << " " << finalcolor.y << " " <<finalcolor.z << std::endl;
        
    }
    
    finalcolor += obj->_ambient + obj->_emission;

    //std::cout<< finalcolor.x << " " << finalcolor.y << " " <<finalcolor.z << std::endl;

    if (recurse > 0) {  // This is for reflection
        float times = 2 * glm::dot(eyedir, normal);
        vec3 reflection_direction = glm::normalize(-eyedir+(times * normal));
        vec3 temp_start = intersection + increment * reflection_direction;
        std::pair<Object*, vec3> i_result = calculateIntersection(temp_start, reflection_direction);
        Object* i_obj = i_result.first;
        glm::vec3 intersec = i_result.second;
        if (i_obj != NULL) {
            finalcolor += obj->_specular * calculateColor(i_obj, intersec, intersection, recurse - 1);
        }
    }
    return glm::vec4(std::min(finalcolor[0],static_cast<float>(1)), std::min(finalcolor[1],static_cast<float>(1)),
                     std::min(finalcolor[2],static_cast<float>(1)), std::min(finalcolor[3],static_cast<float>(1)));
}

