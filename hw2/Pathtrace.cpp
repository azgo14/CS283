#include <limits>
#include <iostream>

#include <utility>
#include <FreeImage.h>
#include "variables.h"
#include "Pathtrace.h"
#include <omp.h>

const float Pathtrace::INCREMENT = .0001;

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

float getRandomProb() {
    return rand() / static_cast<float>(RAND_MAX);
}

void output_info() {
    std::cout << "Starting Pathtracing" << std::endl
              << "Direct Lighting: " << ((direct) ? "true" : "false") << std::endl
              << "Indirect Lighting: " << ((indirect) ? "true" : "false") << std::endl
              << "Uniform: " << ((uniform) ? "true" : "false") << std::endl
              << "Antialiasing: " << ((antialiasing) ? "true" : "false") << std::endl;
}
} // namespace

void Pathtrace::pathtrace (const vec3& eye, const vec3& center, const vec3& up, float fovx, float fovy, int width, int height, FIBITMAP* bitmap, int recurse) {
    int count = 0;
    output_info();
    #pragma omp parallel for
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            vec4 phongColor = vec4(0,0,0,0);
            for (int ray_i = 0; ray_i < ray_per_pixel; ++ray_i) {
                float i_offset = getRandomProb() - .5; // for antialiasing
                float j_offset = getRandomProb() - .5;
                glm::vec3 ray_direction;
                if (antialiasing) {
                    ray_direction = calculateRay(eye, center, up, fovx, fovy, width, height, static_cast<float>(i)+i_offset, static_cast<float>(j)+j_offset);
                } else {
                    ray_direction = calculateRay(eye, center, up, fovx, fovy, width, height, static_cast<float>(i)+.5, static_cast<float>(j)+.5);
                }
                std::pair<Object*, vec3> i_result = calculateIntersection(eye, ray_direction);
                //std::cout << "Intersection: " << i_result.second.x << " " << i_result.second.y << " " << i_result.second.z << std::endl;
                if (i_result.first == NULL) {
                    std::cout << "No objects intersected" << std::endl;
                    continue;
                }
                Object* i_obj = i_result.first;
                glm::vec3 intersection = i_result.second;
                if (i_obj != NULL) {
                    vec4 tempColor = vec4(0,0,0,0);
                    calculateColor(i_obj, intersection, eye, recurse, 1, &tempColor);
                    tempColor = glm::vec4(std::min((tempColor)[0],static_cast<float>(1)), std::min((tempColor)[1],static_cast<float>(1)),
                                          std::min((tempColor)[2],static_cast<float>(1)), std::min((tempColor)[3],static_cast<float>(1)));
                    phongColor += (1 / static_cast<float>(ray_per_pixel)) * tempColor;
                }
            }
            RGBQUAD color;
            color.rgbBlue = 255 * phongColor.x;
            color.rgbGreen = 255 * phongColor.y;
            color.rgbRed = 255 * phongColor.z;
            //std::cout << "Color: " << 255 * phongColor.z << " " << 255 * phongColor.y << " " << 255 * phongColor.x << std::endl;
            FreeImage_SetPixelColor(bitmap, j, height - i - 1, &color);
        }
        std::cout << ++count << " rows out of " << height << " rows calculated." << std::endl;
    }
}


// returns the direction of the ray
glm::vec3 Pathtrace::calculateRay(const vec3& eye, const vec3& center, const vec3& up,
                                  float fovx, float fovy, int width, int height,
                                  float i, float j) {
    vec3 w = glm::normalize(eye - center);
    vec3 u = glm::normalize(glm::cross(up, w));
    vec3 v = glm::cross(w, u);
    float a = glm::tan(glm::radians(fovx/2)) * ((j-(static_cast<float>(width)/2))/(static_cast<float>(width)/2));
    float b = glm::tan(glm::radians(fovy/2)) * (((static_cast<float>(height)/2)-i)/(static_cast<float>(height)/2));
    return glm::normalize(a*u + b*v - w);
}

namespace {
float inv_pi = 1 / M_PI;
glm::vec4 phongDiffuse(const vec4& color, const vec4& obj_diffuse) {
    vec4 diffuseTerm = glm::vec4(obj_diffuse.x, obj_diffuse.y,
                                 obj_diffuse.z, obj_diffuse.w);
    
    return color * inv_pi * diffuseTerm ;

}
glm::vec4 phongSpecular(const vec3& normal, const vec3& halfAngle,
                        const vec4& color, const vec4& obj_specular, float obj_shininess) {
    
    float nDotH = glm::dot(normal, halfAngle);
    if (nDotH < 0) {
        nDotH = 0;
    }
    float shine = glm::pow(nDotH, obj_shininess);

    vec4 specularTerm = glm::vec4(obj_specular.x * shine, obj_specular.y * shine,
                                  obj_specular.z * shine, obj_specular.w * shine);
    
    return color * specularTerm;

}
} // namespace
void Pathtrace::getDirectLight(Object * obj, const vec3& intersection,
                               const vec3& eye, const vec3& eyedir,
                               const vec3& normal, float reflect_weight, vec4 * finalcolor) {
    vec3 shadow_direction, halfAngle;
    float distance;
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
        vec3 shadow_origin = intersection + INCREMENT * shadow_direction;
        float dist_to_light = glm::dot(dir_vec,dir_vec);
        //std::cout << "Shadow Origin: " << shadow_origin.x << " " << shadow_origin.y << " " << shadow_origin.z << std::endl;
        //std::cout << "Distance to Light " << dist_to_light << std::endl;
        if (checkInShadow(shadow_origin, shadow_direction, dist_to_light)) {            
            continue;
        }
        
        halfAngle = glm::normalize(shadow_direction + eyedir);
        distance = sqrt(dist_to_light);
        
        vec4 color = lights[i]->getColor(normal, shadow_direction, distance); 
        
        (*finalcolor) += .5 * reflect_weight * phongDiffuse(color, obj->_diffuse);
        (*finalcolor) += .5 * reflect_weight * phongSpecular(normal, halfAngle,
                                                             color, obj->_specular,
                                                             obj->_shininess);
        // ^ half the contribution of each part of the area light (since it's just one light in the end)

        //std::cout<< finalcolor.x << " " << finalcolor.y << " " <<finalcolor.z << std::endl;
        
    }
}
namespace {
vec3 sampleFromHemiSphereUniform(vec3 hemisphere_normal) {
    float theta, azm;
    vec3 point;
    do {
        theta = getRandomProb() * M_PI;
        azm = getRandomProb() * 2 * M_PI;
        point = vec3(sin(theta)*cos(azm), sin(theta)*sin(azm), cos(theta));
    } while (glm::dot(point, hemisphere_normal) <= 0);
    return glm::normalize(point);
}

vec3 sampleFromHemiSphereCosLambertianBRDF(vec3 hemisphere_normal) {
    float theta, azm, z;
    vec3 point;
    azm = getRandomProb() * 2 * M_PI;
    z = getRandomProb();
    theta = acos(sqrt(z));
    point = glm::normalize(vec3(sin(theta)*cos(azm), sin(theta)*sin(azm), z));
    
    vec3 w = hemisphere_normal;
    vec3 u = glm::normalize(glm::cross(point, w));
    vec3 v = glm::normalize(glm::cross(w, u));
    glm::mat3 rot = glm::mat3(u[0], u[1], u[2],
                              v[0], v[1], v[2],
                              w[0], w[1], w[2]);
    return glm::normalize(rot * point);
}

vec3 sampleFromHemiSpherePhongBRDF(vec3 reflect_dir, int shininess) {
    float theta, azm, z;
    vec3 point;
    z = getRandomProb();
    theta = acos(pow(z, 1.0 / (shininess + 1)));
    azm = getRandomProb() * 2 * M_PI;

    point = glm::normalize(vec3(sin(theta)*cos(azm), sin(theta)*sin(azm), z));
    
    vec3 w = reflect_dir;
    vec3 u = glm::normalize(glm::cross(point, w));
    vec3 v = glm::normalize(glm::cross(w, u));
    glm::mat3 rot = glm::mat3(u[0], u[1], u[2],
                              v[0], v[1], v[2],
                              w[0], w[1], w[2]);
    return glm::normalize(rot * point);
}
} // namespace

void Pathtrace::getImportanceIndirectLight(Object * obj, const vec3& intersection, const vec3& eyedir, const vec3& normal,
                                           float alive_weight, float weight, int recurse, vec4 * finalcolor) {
    
    float diffuse_prob, diffuse_weight, specular_weight;
    diffuse_prob = (obj->_diffuse.x + obj->_diffuse.y + obj->_diffuse.z) /
        (obj->_diffuse.x + obj->_diffuse.y + obj->_diffuse.z + obj->_specular.x + obj->_specular.y + obj->_specular.z); 
    if (diffuse_prob == 0) {
        diffuse_weight = 1;
        specular_weight = 1;
    } else {    
        diffuse_weight = 1 / diffuse_prob;
        specular_weight = 1/ (1 - diffuse_prob);
    }
    bool chooseDiffuse = getRandomProb() < diffuse_prob;
    vec3 new_dir; // TODO: check if always normalized
    vec3 temp_start;
    std::pair<Object*, vec3> i_result;
    do {
        if (chooseDiffuse) {
            new_dir = sampleFromHemiSphereCosLambertianBRDF(normal);
        } else {
            float times = 2 * glm::dot(eyedir, normal);
            vec3 reflection_direction = glm::normalize(-eyedir+(times * normal));
            new_dir = sampleFromHemiSpherePhongBRDF(reflection_direction, obj->_shininess);
        }
        temp_start = intersection + INCREMENT * new_dir;
        i_result = calculateIntersection(temp_start, new_dir);
        if (i_result.first == NULL) {
            //std::cout << "No objects intersected" << std::endl;
            return;
        }
    } while (i_result.first->isLight);

    weight = weight * std::max(glm::dot(normal, new_dir), static_cast<float>(0));  // this applies for both specular and diffuse    
    if (chooseDiffuse) {
        //diffuse
        weight = inv_pi * weight;
        vec4 color = vec4(0,0,0,0);
        calculateColor(i_result.first, i_result.second, temp_start, recurse - 1, weight, &color);
        (*finalcolor) += alive_weight * diffuse_weight * phongDiffuse(color, obj->_diffuse);                                                                        
 
    } else {
        //specular
        vec3 halfAngle = glm::normalize(new_dir + eyedir);
        //std::cout << "Half Angle " << halfAngle.x << " " << halfAngle.y << " " << halfAngle.z << std::endl;


        float nDotH = glm::dot(normal, halfAngle);
        if (nDotH < 0) {
            nDotH = 0;
        }
        float shine = glm::pow(nDotH, obj->_shininess);
        weight = shine * weight;
        //std::cout << weight << std::endl;
        vec4 color = vec4(0,0,0,0);
        calculateColor(i_result.first, i_result.second, temp_start, recurse - 1, weight, &color);

        (*finalcolor) += alive_weight * specular_weight * phongSpecular(normal, halfAngle, color,
                                                                        obj->_specular, obj->_shininess);
        
        //std::cout << "Indirect Reflect Weight: " << reflect_weight << std::endl;
        //std::cout << "Indirect Specular Weight: " << specular_weight << std::endl;                                                              
        //std::cout << "Indirect Specular Color: " << finalcolor->x << " " << finalcolor->y << " " << finalcolor->z << std::endl;
    }
}

void Pathtrace::getReflectedLight(Object * obj, const vec3& intersection, const vec3& eyedir, const vec3& normal,
                                  float alive_weight, float weight, int recurse, vec4 * finalcolor) {
    float times = 2 * glm::dot(eyedir, normal);
    vec3 reflection_direction = glm::normalize(-eyedir+(times * normal));
    vec3 temp_start = intersection + INCREMENT * reflection_direction;
    std::pair<Object*, vec3> i_result = calculateIntersection(temp_start, reflection_direction);
    if (i_result.first == NULL) {
        return;
    }
    vec4 color = vec4(0,0,0,0);
    calculateColor(i_result.first, i_result.second, temp_start, recurse - 1, weight, &color);
    (*finalcolor) += alive_weight * obj->_reflectance * color;
}

void Pathtrace::getUniformIndirectLight(Object * obj, const vec3& intersection, const vec3& eyedir, const vec3& normal,
                                        float alive_weight, float weight, int recurse, vec4 * finalcolor) {
    float diffuse_prob = .5;
    float diffuse_weight = 1 / diffuse_prob;
    float specular_weight = 1/ (1 - diffuse_prob);
    vec3 new_dir; // TODO: check if always normalized
    vec3 temp_start;
    std::pair<Object*, vec3> i_result;
    do {
        new_dir = sampleFromHemiSphereUniform(normal);
        temp_start = intersection + INCREMENT * new_dir;
        i_result = calculateIntersection(temp_start, new_dir);
        if (i_result.first == NULL) {
            //std::cout << "No objects intersected" << std::endl;
            return;
        }
    } while (i_result.first->isLight);

    weight = weight * std::max(glm::dot(normal, new_dir), static_cast<float>(0));  // this applies for both specular and diffuse    
    if (getRandomProb() < diffuse_prob) {
        //diffuse
        weight = inv_pi * weight;
        vec4 color = vec4(0,0,0,0);
        calculateColor(i_result.first, i_result.second, temp_start, recurse - 1, weight, &color);
        (*finalcolor) += alive_weight * diffuse_weight * phongDiffuse(color, obj->_diffuse);                                                                        
 
    } else {
        //specular
        vec3 halfAngle = glm::normalize(new_dir + eyedir);
        //std::cout << "Half Angle " << halfAngle.x << " " << halfAngle.y << " " << halfAngle.z << std::endl;


        float nDotH = glm::dot(normal, halfAngle);
        if (nDotH < 0) {
            nDotH = 0;
        }
        float shine = glm::pow(nDotH, obj->_shininess);
        weight = shine * weight;
        //std::cout << weight << std::endl;
        vec4 color = vec4(0,0,0,0);
        calculateColor(i_result.first, i_result.second, temp_start, recurse - 1, weight, &color);

        (*finalcolor) += alive_weight * specular_weight * phongSpecular(normal, halfAngle, color,
                                                                        obj->_specular, obj->_shininess);
        
        //std::cout << "Indirect Reflect Weight: " << reflect_weight << std::endl;
        //std::cout << "Indirect Specular Weight: " << specular_weight << std::endl;                                                              
        //std::cout << "Indirect Specular Color: " << finalcolor->x << " " << finalcolor->y << " " << finalcolor->z << std::endl;
    }
}

namespace {
bool winRussianRoulette(float weight) {
   return getRandomProb() < weight; 
}
} // namespace

void Pathtrace::calculateColor(Object * obj, const vec3& intersection, const vec3& eye, int recurse, float weight, vec4 *finalcolor) {
    //std::cout << recurse << std::endl;
    if (recurse < 0 || (weight < .05 && !winRussianRoulette(weight))) {
        return;
    }
    float alive_weight = 1;
    if (weight < .05) {
        alive_weight = 1 / (1 - weight);
    }

    vec3 eyedir = glm::normalize(eye-intersection);
    vec3 normal = obj->getNormal(intersection);

    if (obj->isLight) {
        //std::cout << "here"<<std::endl;
        (*finalcolor) += alive_weight * static_cast<Light*>(obj)->getIntensity(intersection, attenuation);
        return;
    } else if (obj->_reflectance.x != 0 || obj->_reflectance.y != 0 || obj->_reflectance.z != 0 ) {
        //std::cout << obj->_reflectance.x << " " << obj->_reflectance.y << " " << obj->_reflectance.z << std::endl;
        getReflectedLight(obj, intersection, eyedir, normal, alive_weight, weight, recurse, finalcolor); // does recursive call
        return;
    }
    
    if (recurse != depth || direct) {
        getDirectLight(obj, intersection, eye, eyedir, normal, alive_weight, finalcolor);
        (*finalcolor) += alive_weight * obj->_emission;
    }

    if (indirect) {            
        if (uniform) {                
            getUniformIndirectLight(obj, intersection, eyedir, normal, alive_weight, weight, recurse, finalcolor); // does recursive call
        } else {
            getImportanceIndirectLight(obj, intersection, eyedir, normal, alive_weight, weight, recurse, finalcolor); // does recursive call
        }
    }
    //std::cout<< finalcolor->x << " " << finalcolor->y << " " <<finalcolor->z << std::endl;
}

