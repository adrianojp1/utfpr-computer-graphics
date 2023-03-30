CC = g++

GLLIBS = -lglut -lGLEW -lGL
LIBS = ../../lib/*.cpp

all: question2

question2: question2.cpp
	$(CC) question2.cpp $(LIBS) -o question2.o $(GLLIBS)

clean:
	rm -f question2.o
