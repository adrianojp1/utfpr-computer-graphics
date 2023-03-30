CC = g++

GLLIBS = -lglut -lGLEW -lGL
LIBS = ../../lib/*.cpp

all: questions

questions: questions.cpp
	$(CC) questions.cpp $(LIBS) -o questions.o $(GLLIBS)

clean:
	rm -f questions.o
