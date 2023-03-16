CC = g++

GLLIBS = -lglut -lGLEW -lGL
UTILS = ../lib/utils.cpp

all: triangle primitives rectangle rectangle2

triangle: triangle.cpp
	$(CC) triangle.cpp $(UTILS) -o triangle.o $(GLLIBS)

primitives: primitives.cpp
	$(CC) primitives.cpp $(UTILS) -o primitives.o $(GLLIBS)

rectangle: rectangle.cpp
	$(CC) rectangle.cpp $(UTILS) -o rectangle.o $(GLLIBS)

rectangle2: rectangle2.cpp
	$(CC) rectangle2.cpp $(UTILS) -o rectangle2.o $(GLLIBS)

clean:
	rm -f triangle.o primitives.o rectangle.o rectangle2.o
