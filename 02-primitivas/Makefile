CC = g++

GLLIBS = -lglut -lGLEW -lGL

all: triangle primitives rectangle rectangle2

triangle: triangle.cpp
	$(CC) triangle.cpp -o triangle.o $(GLLIBS)

primitives: primitives.cpp
	$(CC) primitives.cpp ../lib/utils.cpp -o primitives.o $(GLLIBS)

rectangle: rectangle.cpp
	$(CC) rectangle.cpp ../lib/utils.cpp -o rectangle.o $(GLLIBS)

rectangle2: rectangle2.cpp
	$(CC) rectangle2.cpp ../lib/utils.cpp -o rectangle2.o $(GLLIBS)

clean:
	rm -f triangle.o primitives.o rectangle.o rectangle2.o
