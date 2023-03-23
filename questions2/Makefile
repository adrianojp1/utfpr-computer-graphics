CC = g++

GLLIBS = -lglut -lGLEW -lGL
LIBS = ../lib/*.cpp

all: questions2 question6

questions2: questions2.cpp
	$(CC) questions2.cpp $(LIBS) -o questions2.o $(GLLIBS)

question6: question6.cpp
	$(CC) question6.cpp $(LIBS) -o question6.o $(GLLIBS)

clean:
	rm -f questions2.o question6.o
