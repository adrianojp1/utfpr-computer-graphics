CC = g++

GLLIBS = -lglut -lGLEW -lGL
LIBS = ../lib/*.cpp

all: triangle

triangle: triangle.cpp
	$(CC) triangle.cpp $(LIBS) -o triangle.o $(GLLIBS)

clean:
	rm -f triangle.o
