#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <SceneMesh.hpp>
#include "Shader.h"

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

/** Shaders. */
const char* vtx_filename("./shaders/vtx.glsl");
const char* frag_filename("./shaders/frag.glsl");
Shader shader;

/** Object mesh. */
const char* obj_filename("./resources/tetrahedron.obj");
SceneMesh sceneMesh;

/** Camera. */
vec3 init_camera_position(0.0f, 3.0f, 10.0f);
vec3 camera_target(0.0f, 0.0f, 0.0f);
vec3 up_vec(0.0f, 1.0f, 0.0f);
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
void loadResources(void);

void display()
{
    glClearColor(0.1, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    unsigned int model_loc = glGetUniformLocation(shader.id, "model");
    unsigned int view_matrix_loc = glGetUniformLocation(shader.id, "view");
    unsigned int projection_matrix_loc = glGetUniformLocation(shader.id, "projection");

    // unsigned int camera_position_loc = glGetUniformLocation(shader.id, "camera_position");
    // glUniform3f(camera_position_loc, camera_position.x, camera_position.y, camera_position.z);

    mat4 model = init_model;
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, value_ptr(model)); // Pass rotation matrix to vertex shader.

    mat4 cam_rotation = rotate(mat4(1.0f), radians(camera_rotation), vec3(0.0f, 1.0f, 0.0f));
    vec3 camera_position = cam_rotation * vec4(init_camera_position, 1.0f);
    mat4 view = lookAt(camera_position, sceneMesh.center, up_vec);
    glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, value_ptr(view)); // Uniform: Transfer view matrix to vertex shader.

    mat4 projection = init_projection;
    glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, value_ptr(projection));

    for (unsigned int i = 0; i < sceneMesh.num_meshes; ++i) {
        glBindVertexArray(sceneMesh.mesh_list[i].VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)sceneMesh.mesh_list[i].vert_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

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
 * Load mesh and shaders resource files.
 */
void loadResources()
{
    // Load mesh
    sceneMesh.load(obj_filename);

    // Create shaders
    shader.loadAndCreateShader(vtx_filename, frag_filename);
    shader.use();
}

int main(int argc, char** argv)
{
    // Init window
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow(argv[0]);
    glewInit();

    // Load resource files
    loadResources();

    // Callbacks
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    // Depth test
    glEnable(GL_DEPTH_TEST);

    // Init time
    init_time = glutGet(GLUT_ELAPSED_TIME);

    glutMainLoop();
}
