#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "../lib/utils.h"

using namespace std;
using namespace glm;

/* Globals */
/** Window width. */
int win_width = 800;
/** Window height. */
int win_height = 800;

/** Time control */
int old_time = 0;
int delta_time = 0;
int init_time = 0;

/** Program variable. */
int program;
/** Vertex array object. */
unsigned int VAO;
/** Vertex buffer object. */
unsigned int VBO;
/** Color buffer object. */
unsigned int CBO;

/** Camera. */
vec3 init_camera_position = vec3(0.0f, 3.0f, 10.0f);
vec3 camera_target = vec3(0.0f, 0.0f, 0.0f);
vec3 up_vec = vec3(0.0f, 1.0f, 0.0f);
float camera_rotation = 0.0f;
float camera_rotation_speed = 0.05f;

/** MVP Matrices */
mat4 init_model = mat4(1.0f);
mat4 init_view = lookAt(init_camera_position, camera_target, up_vec);
mat4 init_projection = perspective(radians(45.0f), (GLfloat)win_width / win_height, 1.0f, 20.0f);

/* Functions. */
void display(void);
void reshape(int, int);
void keyboard(unsigned char, int, int);
void idle();
void initData(void);
void initShaders(void);

void display()
{
    glClearColor(0.1, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(VAO);

    mat4 cam_rotation = rotate(mat4(1.0f), radians(camera_rotation), vec3(0.0f, 1.0f, 0.0f));
    vec3 camera_position = cam_rotation * vec4(init_camera_position, 1.0f);

    mat4 model = init_model;
    mat4 view = lookAt(camera_position, camera_target, up_vec);
    mat4 projection = init_projection;

    mat4 MVP = projection * view * model;

    // Retrieve location of tranform variable in shader.
    unsigned int loc = glGetUniformLocation(program, "MVP");
    // Send matrix to shader.
    glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(MVP));

    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

    glutSwapBuffers();

    int cur_time = glutGet(GLUT_ELAPSED_TIME);
    delta_time = cur_time - old_time;
    old_time = cur_time;
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
    camera_rotation += camera_rotation_speed * delta_time;
    if (camera_rotation >= 360.0f) {
        camera_rotation -= 360.0f;
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
    // Set triangle vertices.
    static const GLfloat vertex[] = {
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };

    static const GLfloat colors[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };

    // Vertex array.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
    // Set attributes.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color buffer
    glGenBuffers(1, &CBO);
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    // Set attributes.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // Unbind Vertex Array Object.
    glBindVertexArray(0);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
}

/** Create program (shaders).
 *
 * Compile shaders and create the program.
 */
void initShaders()
{
    // Read shaders source
    const char* vertex_code = readFile("question1_vtx.glsl");
    const char* fragment_code = readFile("question1_frag.glsl");

    // Request a program and shader slots from GPU
    program = createShaderProgram(vertex_code, fragment_code);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow(argv[0]);
    glewInit();

    // Init vertex data for the triangle.
    initData();

    // Create shaders.
    initShaders();
    glUseProgram(program);

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    init_time = glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();
}
