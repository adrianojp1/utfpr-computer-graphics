CC = g++

GLLIBS = -lglut -lGLEW -lGL
LIBS = ../lib/*.cpp

all: question1

question1: question1.cpp
	$(CC) question1.cpp $(LIBS) -o question1.o $(GLLIBS)

clean:
	rm -f question1.o
