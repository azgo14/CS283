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

uniform vec4 ambient ; 
uniform vec4 diffuse ; 
uniform vec4 specular ; 
uniform vec4 emission ; 
uniform float shininess ; 

varying vec4 shadowcoord;
uniform sampler2D shadowmap;

vec4 ComputeLight (const in vec3 direction, const in vec4 lightcolor, const in vec3 normal, const in vec3 halfvec, const in vec4 mydiffuse, const in vec4 myspecular, const in float myshininess) {

        float nDotL = dot(normal, direction)  ;         
        vec4 lambert = mydiffuse * lightcolor * max (nDotL, 0.0) ;  

        float nDotH = dot(normal, halfvec) ; 
        vec4 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess) ; 

        vec4 retval = lambert + phong ; 
        return retval ;            
}

void main (void) 
{       
    if (enablelighting) {       
        
        
        vec4 finalcolor ; 

        finalcolor = ambient + emission ;
        
        const vec3 eyepos = vec3(0,0,0) ; 
        vec4 _mypos = gl_ModelViewMatrix * myvertex ; 
        vec3 mypos = _mypos.xyz / _mypos.w ; // Dehomogenize current location 
        vec3 eyedirn = normalize(eyepos - mypos) ; 

        vec3 _normal = (gl_ModelViewMatrixInverseTranspose*vec4(mynormal,0.0)).xyz ; 
        vec3 normal = normalize(_normal) ; 
        
        vec3 light_dir = vec3(-1,-1,-1); 
        for(int i = 0; i < numused; ++i) {
          vec3 direction = vec3(0,0,0) ;
          if (lightposn[i].w == 0) {   // directional
            direction = normalize (lightposn[i].xyz) ; // assume this is lightdirn
            if (light_dir.x == -1) {
              light_dir = direction;
            }
          }
          else { // point
            vec3 position = lightposn[i].xyz / lightposn[i].w ;
            direction = normalize (position - mypos) ;
          }
          vec3 halfvec = normalize (direction + eyedirn) ;  
          vec4 col = ComputeLight(direction, lightcolor[i], normal, halfvec, diffuse, specular, shininess) ;
          finalcolor += col ;
        }
        float angle = dot(normal, normalize(light_dir));
        if (angle < .1)  // care only about angles within 90 degrees. don't want tan(acos(0)).
          angle = .1;
        if (angle > 1)
          angle = 1;
        float shadow_bias = .0007 * tan(acos(angle));
        if (shadow_bias > .005)
          shadow_bias = .005;
        float shadow = 1.0;
        
        if ( texture2D( shadowmap, shadowcoord.xy ).z  <  shadowcoord.z - shadow_bias){
          shadow = 0.5;
        }
        
        gl_FragColor = shadow * finalcolor ; 
    }
    else gl_FragColor = color ; 
}
