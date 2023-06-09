CC = g++

OBJ_NAME = mesh2
INC_DIR = ./includes
SRC_DIR = ./src

GLLIBS = -lglut -lGLEW -lGL -lassimp
LIBS = $(GLLIBS)

all: main

main: $(SRC_DIR)/main.cpp
	$(CC) -I $(INC_DIR) $(SRC_DIR)/*.cpp $(LIBS) -o $(OBJ_NAME)

# run: main
# 	./$(OBJ_NAME)

clean:
	rm -f $(OBJ_NAME)
