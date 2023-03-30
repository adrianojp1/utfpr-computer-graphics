#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "../lib/utils.h"
#include "../lib/euclidian.h"

using namespace std;

/* Globals */
/** Window width. */
int win_width = 800;
/** Window height. */
int win_height = 600;

/** Program variable. */
int program;
/** Vertex array object. */
unsigned int VAO;
/** Vertex buffer object. */
unsigned int VBO;

/** Color buffer. */
unsigned int colorbuffer;

/** Vertices array. */
float vertices[9] = {};
float vertices_set_idx = 0;

/** colors array. */
float colors[] = {
    1.0f, 0.0f, 0.00f,
    0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f
};

bool displayVectors = false;

/* Functions. */
void display(void);
void reshape(int, int);
void keyboard(unsigned char, int, int);
void mouse(int button, int state, int x, int y);
void refreshPoints(float coord_x, float coord_y);
void initData(void);
void initShaders(void);

/**
 * Drawing function.
 *
 * Draws a triangle.
 */
void display()
{
    glClearColor(0.1, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glBindVertexArray(VAO);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Draws the points.
    if (displayVectors) {
        glDrawArrays(GL_LINE_STRIP, 0, 3);
    } else {
        glDrawArrays(GL_POINTS, 0, 3);
    }

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
    }
}


void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        cout << "\nx: " << x << ", y: " << y << "\n";
        float coord_x = ((float)x / win_width - 0.5f) * 2;
        float coord_y = ((float)y / win_height - 0.5f) * -2;
        cout << "coord_x: " << coord_x << ", coord_y: " << coord_y << "\n";
        refreshPoints(coord_x, coord_y);
    }
}

void printVector(vector<float> v) {
    cout << "{ ";
    for (float f: v) {
      cout << f << ", ";
    }
    cout << "}\n";
}

void printArray(float *arr, int size) {
    cout << "{ ";
    for (int i = 0; i < size; i++) {
        cout << vertices[i] << ", ";
    }
    cout << "}\n";
}

void refreshPoints(float coord_x, float coord_y) {
    int first_idx = vertices_set_idx * 3;
    vertices[first_idx] = coord_x;
    vertices[first_idx + 1] = coord_y;
    vertices[first_idx + 2] = 0.0f;

    if (vertices_set_idx < 2) {
        vertices_set_idx++;
        displayVectors = false;
    }
    else {
        vertices_set_idx = 0;
        displayVectors = true;
    }
    cout << "Vertices: ";
    printArray(vertices, 9);

    if (displayVectors) {
        vector<float> point_p {vertices[0], vertices[1], vertices[2]};
        vector<float> point_o {vertices[3], vertices[4], vertices[5]};
        vector<float> point_q {vertices[6], vertices[7], vertices[8]};

        vector<float> vector_u{point_p[0] - point_o[0], point_p[1] - point_o[1], point_p[2] - point_o[2]};
        vector<float> vector_v{point_q[0] - point_o[0], point_q[1] - point_o[1], point_o[2] - point_o[2]};

        cout << "\n\n" << "Vector u: ";
        printVector(vector_u);
        cout << "Vector v: ";
        printVector(vector_v);
        cout << "\n\n";

        float angle = rad2dgr(innerAngle(vector_u, vector_v));
        float inner_prod = innerProduct(vector_u, vector_v);
        vector<float> cross_prod = crossProduct(vector_u, vector_v);
        float dist_p_2_v = distPoint2Line(point_p, point_o, vector_v);

        cout << "Angle between u and v: " << angle << "\n";
        cout << "Inner product between u and v: " << inner_prod << "\n";
        cout << "Vetorial product between u and v: ";
        printVector(cross_prod);
        cout << "Distance between p and v: " << dist_p_2_v << "\n";
    }
    
    glutPostRedisplay();
}

/**
 * Init vertex data.
 *
 * Defines the coordinates for vertices, creates the arrays for OpenGL.
 */
void initData()
{
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

    // Color buffer
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    // Set attributes.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
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
    // Request a program and shader slots from GPU
    program = glCreateProgram();
    int vertex = glCreateShader(GL_VERTEX_SHADER);
    int fragment = glCreateShader(GL_FRAGMENT_SHADER);

    // Read and set shaders source
    const char* vertex_code = readFile("question6_vtx.glsl");
    const char* fragment_code = readFile("question6_frag.glsl");
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

int main(int argc, char** argv)
{
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
