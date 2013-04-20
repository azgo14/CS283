# version 120 

/* This is the fragment shader for reading in a scene description, including 
   lighting.  Uniform lights are specified from the main program, and used in 
   the shader.  As well as the material parameters of the object.  */

// Mine is an old machine.  For version 130 or higher, do 
// in vec4 color ;  
// in vec4 mynormal ; 
// in vec4 myvertex ;
// That is certainly more modern

varying vec4 color ;
varying vec3 mynormal ; 
varying vec4 myvertex ; 

const int numLights = 10 ; 
uniform bool enablelighting ; // are we lighting at all (global).
uniform vec4 lightposn[numLights] ; // positions of lights 
uniform vec4 lightcolor[numLights] ; // colors of lights
uniform int numused ;               // number of lights used

// Now, set the material parameters.  These could be varying and/or bound to 
// a buffer.  But for now, I'll just make them uniform.  
// I use ambient, diffuse, specular, shininess as in OpenGL.  
// But, the ambient is just additive and doesn't multiply the lights.  

uniform float shadow_buff_size;
uniform bool use_shadow;
uniform bool use_pcf;

uniform vec4 ambient ; 
uniform vec4 diffuse ; 
uniform vec4 specular ; 
uniform vec4 emission ; 
uniform float shininess ; 

varying vec4 shadowcoord;
uniform sampler2D shadowmap;
uniform samplerCube cubemap;
uniform mat4 modelmatrixinversetranspose;
uniform mat4 modelmatrix;
uniform vec3 eye_world;
uniform bool is_reflect;
uniform bool is_reflect_diffuse;
uniform bool is_skybox;
varying mat3 mytbn;
uniform bool is_displace;
uniform bool is_bump;
uniform bool is_tex;

uniform sampler2D bumpmap;
uniform sampler2D texmap;



vec4 ComputeLight (const in vec3 direction, const in vec4 lightcolor, const in vec3 normal, const in vec3 halfvec, const in vec4 mydiffuse, const in vec4 myspecular, const in float myshininess) {

    float nDotL = dot(normal, direction)  ;         
    vec4 lambert = mydiffuse * lightcolor * max (nDotL, 0.0) ;  

    float nDotH = dot(normal, halfvec) ; 
    vec4 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess) ; 

    vec4 retval = lambert + phong ; 
    return retval ;            
}


float computeShadowBias(const in vec3 normal, const in vec3 light_dir) {
    float angle = dot(normal, normalize(light_dir));
    if (angle < .1)  // care only about angles within 90 degrees. don't want tan(acos(0)).
        angle = .1; 
    if (angle > 1)
        angle = 1;
    float shadow_bias = .0025 * tan(acos(angle));
    if (shadow_bias > .005)
        shadow_bias = .005;
    return shadow_bias; 
}

float computeShadowPCF(const in float shadow_bias) {
    float offset = 1.0 / shadow_buff_size;
    int shadow_count = 0;
    for (int i = -2; i < 3; ++i) {
        for (int j = -2; j < 3; ++j) {
            if ( texture2D( shadowmap, shadowcoord.xy + vec2(i*offset,j*offset) ).z  >  shadowcoord.z - shadow_bias){
                shadow_count = shadow_count + 1;
            } 
        }
    }
    return shadow_count / 25.0;   
}

float computeShadow(const in vec3 normal, const in vec3 light_dir) {
    float shadow_bias = computeShadowBias(normal, light_dir);
    float shadow = 1.0;

    if (use_pcf) {
        shadow = .3 + .7 * computeShadowPCF(shadow_bias);
    } else if ( texture2D( shadowmap, shadowcoord.xy ).z  <  shadowcoord.z - shadow_bias){
        shadow = 0.3;
    }   
    return shadow;
}

void main (void) 
{       
    if (enablelighting) {       
        
        if (gl_TexCoord[0].z != 0 && is_skybox) { // for skybox only
          gl_FragColor = textureCube(cubemap, normalize(gl_TexCoord[0].xyz));
          return;
        }
        vec4 finalcolor ; 

        finalcolor = ambient + emission ;
        
        const vec3 eyepos = vec3(0,0,0) ; 
        vec4 _mypos = gl_ModelViewMatrix * myvertex ; 
        vec3 mypos = _mypos.xyz / _mypos.w ; // Dehomogenize current location 
        vec3 eyedirn = normalize(eyepos - mypos) ; 

        vec3 _normal = (gl_ModelViewMatrixInverseTranspose*vec4(mynormal,0.0)).xyz ; 
        vec3 normal = normalize(_normal) ; 
        
        vec4 world_position = modelmatrix * myvertex;
        vec3 world_eye_dirn = normalize(eye_world - (world_position.xyz / world_position.w));
        vec3 world_normal = normalize((modelmatrixinversetranspose * vec4(mynormal,0.0)).xyz);
              
        float times = 2 * dot(world_eye_dirn, world_normal);
        vec3 world_reflect_dir = normalize(-world_eye_dirn + (times * world_normal));
        
        if (is_bump && gl_TexCoord[0].x >= 0) {
          mypos = mytbn * mypos;
          eyedirn = normalize(mytbn * eyedirn);
          normal = normalize(vec3(texture2D(bumpmap, gl_TexCoord[0].xy).rgb * 2.0 - 1.0));
        }
        vec3 light_dir = vec3(-1,-1,-1); 
        for(int i = 0; i < numused; ++i) {
          vec3 direction = vec3(0,0,0) ;
          if (lightposn[i].w == 0) {   // directional
            direction = normalize (lightposn[i].xyz) ; // assume this is lightdirn
            if (is_bump && gl_TexCoord[0].x >= 0) {
              direction = normalize(mytbn * direction);
            }
            if (light_dir.x == -1) {
              light_dir = direction;
            }
          }
          else { // point
            vec3 position = lightposn[i].xyz / lightposn[i].w ;
            if (is_bump && gl_TexCoord[0].x >= 0) {
              position = mytbn * position;
            }
            direction = normalize (position - mypos) ;
          }
          vec3 halfvec = normalize (direction + eyedirn) ;  
          
          if (is_reflect) {
            vec4 col = textureCube(cubemap, normalize(world_reflect_dir.xyz));
            if (is_reflect_diffuse) {
              float nDotL = dot(normal, direction);
              col = col * max (nDotL, 0.0) ; 
            }
            finalcolor += col ;
          } else {
            vec4 col = ComputeLight(direction, lightcolor[i], normal, halfvec, diffuse, specular, shininess) ;
            finalcolor += col ;
          }
        }
        float shadow = 1.0;
        if (use_shadow) {
            shadow = computeShadow(normal, normalize(light_dir)); 
        }
        if (is_tex && gl_TexCoord[0].x >= 0) {
          gl_FragColor = shadow * texture2D(texmap, gl_TexCoord[0].xy) * finalcolor ; 
        } else {  
          gl_FragColor = shadow * finalcolor ;
        }
    }
    else gl_FragColor = color ; 
}
