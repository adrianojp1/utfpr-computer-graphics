#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "../../lib/utils.h"

using namespace glm;

/* Globals */
/** Window width. */
int win_width  = 800;
/** Window height. */
int win_height = 600;

/** Program variable. */
int program;
/** Vertex array object. */
unsigned int VAO;
/** Vertex buffer object. */
unsigned int VBO;

/** Scale. */
float scale_x = 1.0f;
float scale_y = 1.0f;
/** Scale increment. */
float scale_inc = 0.002f;
/** Scale mode. */
#define SCALE_MODE_KEEP 0.0f
#define SCALE_MODE_INCREASE 1.0f
#define SCALE_MODE_SHRINK -1.0f
int scale_mode_x = SCALE_MODE_KEEP;
int scale_mode_y = SCALE_MODE_KEEP;

/* Functions. */
void display(void);
void reshape(int, int);
void keyboard(unsigned char, int, int);
void idle(void);
void initData(void);
void initShaders(void);

void display()
{
    glClearColor(0.1, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glBindVertexArray(VAO);

    // Scale.
    mat4 S = scale(mat4(1.0f), vec3(scale_x, scale_y, 1.0));

    // Retrieve location of tranform variable in shader.
	unsigned int loc = glGetUniformLocation(program, "transform");
   	// Send matrix to shader.
	glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(S));

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glutSwapBuffers();
}


/**
 * Reshape function.
 *
 * Called when window is resized.
 *
 * @param width New window width.
 * @param height New window height.
 */
void reshape(int width, int height)
{
    win_width = width;
    win_height = height;
    glViewport(0, 0, width, height);
    glutPostRedisplay();
}


/** 
 * Keyboard function.
 *
 * Called to treat pressed keys.
 *
 * @param key Pressed key.
 * @param x Mouse x coordinate when key pressed.
 * @param y Mouse y coordinate when key pressed.
 */
void keyboard(unsigned char key, int x, int y)
{
        switch (key)
        {
                case 27:
                        glutLeaveMainLoop();
                case 'q':
                case 'Q':
                        glutLeaveMainLoop();
		case '1':
			scale_mode_x = SCALE_MODE_SHRINK;
			break;
		case '2':
			scale_mode_x = SCALE_MODE_KEEP;
			break;
		case '3':
			scale_mode_x = SCALE_MODE_INCREASE;
			break;
		case '4':
			scale_mode_y = SCALE_MODE_SHRINK;
			break;
		case '5':
			scale_mode_y = SCALE_MODE_KEEP;
			break;
		case '6':
			scale_mode_y = SCALE_MODE_INCREASE;
			break;
		case 'r':
			scale_x = 1.0f;
			scale_y = 1.0f;
			break;
        }
    
	glutPostRedisplay();
}


/**
 * Idle function.
 *
 * Called continuously.
 */
void idle()
{
    scale_x = scale_x + scale_inc * scale_mode_x;
    scale_y = scale_y + scale_inc * scale_mode_y;

    glutPostRedisplay();
}


/**
 * Init vertex data.
 *
 * Defines the coordinates for vertices, creates the arrays for OpenGL.
 */
void initData()
{
    // Set triangle vertices.
    float vertices[] = {
	// First triangle
        // coordinate     color
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        // Second triangle
        // coordinate     color
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f
    };
    
    // Vertex array.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Set attributes.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind Vertex Array Object.
    glBindVertexArray(0);
}

/** Create program (shaders).
 * 
 * Compile shaders and create the program.
 */
void initShaders()
{
    // Read shaders source
    const char* vertex_code = readFile("question3_vtx.glsl");
    const char* fragment_code = readFile("question3_frag.glsl");

    // Request a program and shader slots from GPU
    program = createShaderProgram(vertex_code, fragment_code);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(win_width,win_height);
	glutCreateWindow(argv[0]);
	glewInit();

    	// Init vertex data for the triangle.
    	initData();
    
    	// Create shaders.
    	initShaders();
	
    	glutReshapeFunc(reshape);
    	glutDisplayFunc(display);
    	glutKeyboardFunc(keyboard);
    	glutIdleFunc(idle);

	glutMainLoop();
}
