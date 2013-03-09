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

vec4 phongIllumination(vec3 normal, vec3 direction, vec3 halfAngle, vec4 lightcolor) {
    float nDotL = dot(normal, direction);
    vec4 diffuseTerm = diffuse * lightcolor * max(nDotL, 0.0);
    float nDotH = dot(normal, halfAngle);
    vec4 specularTerm = specular * lightcolor * pow(max(nDotH, 0.0), shininess);
    return diffuseTerm + specularTerm;
}

void main (void) 
{       
    if (enablelighting) {       
        
        vec4 finalcolor = vec4(0, 0, 0, 0); 

        // YOUR CODE FOR HW 2 HERE
        // A key part is implementation of the fragment shader
	vec4 _mypos = gl_ModelViewMatrix * myvertex;
        vec3 mypos = _mypos.xyz / _mypos.w;
	// not sure about this
	vec3 eyepos = vec3(0, 0, 0);
        vec3 eyedir = normalize(eyepos - mypos);
        vec3 normal = normalize(gl_NormalMatrix * mynormal);
	vec3 direction, halfAngle;

	for (int i = 0; i < numused; i++) {
	  vec4 lightcolor = lightcolor[i];
	  if (lightposn[i].w == 0) {
	    direction = normalize(lightposn[i].xyz);
	    halfAngle = normalize(direction + eyedir);
	  } else {
	    vec3 position = lightposn[i].xyz / lightposn[i].w;
	    direction = normalize(position - mypos);
	    halfAngle = normalize(direction + eyedir);
	  }  
	  finalcolor += phongIllumination(normal, direction, halfAngle, lightcolor);
	}
	finalcolor += (color * ambient) + emission;
        gl_FragColor = finalcolor;
    }
    else gl_FragColor = color ; 
}
