CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
CFLAGS = -g -DGL_GLEXT_PROTOTYPES -DOSX -fopenmp
INCFLAGS = -I./glm-0.9.2.7 -I/usr/X11/include -I./include/
LDFLAGS = -framework GLUT -framework OpenGL -L./osxlib/ \
		-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
		-lGL -lGLU -lm -lstdc++ -lfreeimage -fopenmp
else
CFLAGS = -g -DGL_GLEXT_PROTOTYPES -fopenmp
INCFLAGS = -I./glm-0.9.2.7 -I./include/ -I/usr/X11R6/include -I/sw/include \
		-I/usr/sww/include -I/usr/sww/pkg/Mesa/include
LDFLAGS = -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib \
		-L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lglut -lGLU -lGL -lX11 -lfreeimage
endif

RM = /bin/rm -f 
all: transform
transform: main.o Transform.o readfile.o Raytrace.o variables.h readfile.h shaders.h Transform.h Raytrace.h Object.h Light.h Light.o Triangle.h Triangle.o Sphere.h Sphere.o BoundingBox.h BoundingBox.o
	$(CC) $(CFLAGS) -o transforms main.o Transform.o readfile.o Raytrace.o Triangle.o Light.o Sphere.o BoundingBox.o $(INCFLAGS) $(LDFLAGS) 
main.o: main.cpp Transform.h variables.h Raytrace.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c main.cpp
readfile.o: readfile.cpp readfile.h variables.h 
	$(CC) $(CFLAGS) $(INCFLAGS) -c readfile.cpp
Transform.o: Transform.cpp Transform.h 
	$(CC) $(CFLAGS) $(INCFLAGS) -c Transform.cpp  
Raytrace.o: Raytrace.cpp Raytrace.h variables.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c Raytrace.cpp
Triangle.o: Triangle.cpp Triangle.h Object.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c Triangle.cpp
Light.o: Light.cpp Light.h Triangle.h Object.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c Light.cpp
Sphere.o: Sphere.cpp Sphere.h Object.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c Sphere.cpp
BoundingBox.o: BoundingBox.cpp BoundingBox.h Object.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c BoundingBox.cpp
clean: 
	$(RM) *.o transforms *.png


 
