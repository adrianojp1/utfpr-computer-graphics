#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/gtx/string_cast.hpp>
#include "../lib/utils.h"
#include "../lib/euclidian.h"

using namespace std;
using namespace glm;

/* Globals */
/** Window width. */
int win_width = 600;
/** Window height. */
int win_height = 600;

/** Program variable. */
int program;
/** Vertex array object. */
unsigned int VAO;
/** Vertex buffer object. */
unsigned int VBO;
/** Texture buffer object. */
unsigned int TBO;

/** Texture */
unsigned int texture;
#define TEXT_WIDTH 200
#define TEXT_HEIGHT 200
unsigned char text_data[TEXT_HEIGHT][TEXT_WIDTH][3];
vec3 bg_color(0.1f, 0.2f, 0.1f);

/** Circle. */
ivec2 circle_center(5, -3);
int circle_radius = 50;
vec3 circle_color(1.0f, 0.0f, 0.0f);
vec3 center_color(0.0f, 1.0f, 1.0f);

/* Functions. */
void display(void);
void reshape(int, int);
void keyboard(unsigned char, int, int);
void mouse(int button, int state, int x, int y);

void initData(void);
void generateTexture();
bool inTexture(ivec2 p);
void initShaders(void);

/**
 * Drawing function.
 *
 * Draws a triangle.
 */
void display() {
    glClearColor(0.1, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture);

    glUseProgram(program);
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, 2 * 3);

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
void reshape(int width, int height) {
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
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
        case 'q':
        case 'Q':
            glutLeaveMainLoop();
            break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    glutPostRedisplay();
}

/**
 * Init vertex data.
 *
 * Defines the coordinates for vertices, creates the arrays for OpenGL.
 */
void initData() {
    GLfloat vertices[] = {
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,

        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    GLfloat text_coords[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f
    };

    // Vertex array object.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Set attributes.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture buffer
    glGenBuffers(1, &TBO);
    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(text_coords), text_coords, GL_STATIC_DRAW);
    // Set attributes
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // Load texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    generateTexture();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXT_WIDTH, TEXT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, &text_data[0][0][0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Unbind Vertex Array Object.
    glBindVertexArray(0);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
}

void generateTexture() {
    for (int y = 0; y < TEXT_HEIGHT; y++) {
        for (int x = 0; x < TEXT_WIDTH; x++) {
            for (int c = 0; c < 3; c++) {
                text_data[y][x][c] = bg_color[c] * 255;
            }
        }
    }

    vector<ivec2> circle_points = midPointCircleDraw(circle_center, circle_radius);

    ivec2 text_center(TEXT_WIDTH / 2, TEXT_HEIGHT / 2);

    ivec2 text_circle_center = text_center + circle_center;
    if (inTexture(text_circle_center)) {
        for (int c = 0; c < 3; c++) {
            text_data[text_circle_center.y][text_circle_center.x][c] = center_color[c] * 255;
        }
    }

    for (ivec2 p : circle_points) {
        cout << to_string(p) << endl;

        ivec2 text_p = text_center + p;
        if (inTexture(text_p)) {
            for (int c = 0; c < 3; c++) {
                text_data[text_p.y][text_p.x][c] = circle_color[c] * 255;
            }
        }
    }
}

bool inTexture(ivec2 p) {
    return p.x >= 0 && p.x < TEXT_WIDTH && p.y >= 0 && p.y < TEXT_HEIGHT;
}

/** Create program (shaders).
 *
 * Compile shaders and create the program.
 */
void initShaders() {
    // Request a program and shader slots from GPU
    program = glCreateProgram();
    int vertex = glCreateShader(GL_VERTEX_SHADER);
    int fragment = glCreateShader(GL_FRAGMENT_SHADER);

    // Read and set shaders source
    const char* vertex_code = readFile("question1_vtx.glsl");
    const char* fragment_code = readFile("question1_frag.glsl");
    glShaderSource(vertex, 1, &vertex_code, NULL);
    glShaderSource(fragment, 1, &fragment_code, NULL);

    // Compile shaders
    glCompileShader(vertex);
    glCompileShader(fragment);

    // Attach shader objects to the program
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    // Link the program
    glLinkProgram(program);

    // Get rid of shaders (not needed anymore)
    glDetachShader(program, vertex);
    glDetachShader(program, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // Set the program to be used.
    glUseProgram(program);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow(argv[0]);
    glewInit();

    // Init vertex data for the triangle.
    initData();

    // Create shaders.
    initShaders();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    glutMainLoop();

    return 0;
}
