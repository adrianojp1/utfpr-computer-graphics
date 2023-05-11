CC = g++

GLLIBS = -lglut -lGLEW -lGL
LIBS = ../lib/*.cpp

all: question1 question2

question1: question1.cpp
	$(CC) question1.cpp $(LIBS) -o question1.o $(GLLIBS)

question2: question2.cpp
	$(CC) question2.cpp $(LIBS) -o question2.o $(GLLIBS)

clean:
	rm -f question1.o question2.o
