# version 120


// Mine is an old machine.  For version 130 or higher, do
// out vec4 color ;
// out vec4 mynormal ;
// out vec4 myvertex ;
// That is certainly more modern

uniform mat4 depthmatrix;

void main() {
      gl_Position = depthmatrix * gl_Vertex;
}