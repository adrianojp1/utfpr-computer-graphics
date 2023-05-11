#include <iostream>
#include <vector>
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
vec3 camera_position = vec3(0.0f, 0.0f, 5.0f);
vec3 camera_target = vec3(0.0f, 0.0f, 0.0f);
vec3 up_vec = vec3(0.0f, 1.0f, 0.0f);

/** MVP Matrices */
mat4 view = lookAt(camera_position, camera_target, up_vec);
mat4 projection = perspective(radians(45.0f), (GLfloat)win_width / win_height, 0.1f, 20.0f);

/** Model */
vector<vec3> vertex{
    vec3{0.0f,  0.0f, 0.0f},
    vec3{1.0f,  0.0f, 0.0f},
    vec3{0.5f,  1.0f, 0.0f},
    vec3{0.0f,  0.0f, 3.0f}
};
static const GLfloat colors[] = {
    0.5f, 0.0f, 0.5f,
    0.5f, 0.0f, 0.5f,
    0.2f, 0.0f, 0.2f,
    1.0f, 1.0f, 1.0f
};
vec4 normal = vec4{0.0f, 0.0f, 1.0f, 0.0f};
float triangle_rotation = 0.0f;
float triangle_rotation_speed = 0.05f;

/** Line / ray */
vec3 line_point{0.0f, 0.0f, 0.0f};
vec3 line_vec{0.0f, 0.0f, -1.0f};

/** Controls */
bool stop = false;

/* Functions. */
void display(void);
void reshape(int, int);
void keyboard(unsigned char, int, int);
void idle();
void initData(void);
void initShaders(void);

// See: https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/point_in_triangle.html
bool point_in_triangle(vec3 p1, vec3 p2, vec3 p3, vec3 p) {
    vec3 a = p1 - p;
    vec3 b = p2 - p;
    vec3 c = p3 - p;

    vec3 u = cross(b, c);
    vec3 v = cross(c, a);
    vec3 w = cross(a, b);

    if (dot(u, v) < 0.0f) {
        return false;
    }

    if (dot(u, w) < 0.0f) {
        return false;
    }

    return true;
}

void print_intersection(mat4 model, vec4 normal, vec3 l_point, vec3 l_vec) {
    vec3 p1 = model * vec4{vertex[0], 1.0f};
    vec3 p2 = model * vec4{vertex[1], 1.0f};
    vec3 p3 = model * vec4{vertex[2], 1.0f};

    vec3 transformed_normal = model * normal;

    cout << endl << "normal: " << to_string(transformed_normal) << endl;
    cout << "l_point: " << to_string(l_point) << endl;
    cout << "l_vec: " << to_string(l_vec) << endl;

    float normal_vec_dot = dot(transformed_normal, l_vec);
    cout << "normal_vec_dot: " << normal_vec_dot << endl;

    if (abs(normal_vec_dot) > 0.0001f) {
        float t = dot((p1 - l_point), transformed_normal) / normal_vec_dot;
        cout << "t: " << t << endl;
        vec3 plane_intersection = l_point + normalize(l_vec) * t;
        if (point_in_triangle(p1, p2, p3, plane_intersection)) {
            cout << "intersection: " << to_string(plane_intersection) << endl;
        } else {
            cout << "No triangle intersection" << endl;
        }
    } else {
        cout << "No plane intersection" << endl;
    }
}

void display()
{
    glClearColor(0.1, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(VAO);

    mat4 T = translate(mat4{1.0f}, vec3{-0.5f, -0.5f, 1.5f});
    mat4 R = rotate(mat4{1.0f}, radians(triangle_rotation), vec3{0.0f, 1.0f, 0.0f});
    mat4 model = R * T;

    mat4 MVP = projection * view * model;

    print_intersection(model, normal, line_point, line_vec);

    // Retrieve location of tranform variable in shader.
    unsigned int loc = glGetUniformLocation(program, "MVP");
    // Send matrix to shader.
    glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(MVP));

    glDrawArrays(GL_TRIANGLES, 0, 3);

    MVP = projection * view * mat4{1.0f};
    glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(MVP));
    glDrawArrays(GL_POINTS, 3, 1);

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
    case 's':
        if (stop) {
            stop = false;
        } else {
            stop = true;
        }
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
    if (!stop) {
        triangle_rotation += triangle_rotation_speed * delta_time;
        if (triangle_rotation >= 360.0f) {
            triangle_rotation -= 360.0f;
        }
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
    // Vertex array.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);



    // Vertex buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertex.size(), &vertex[0], GL_STATIC_DRAW);
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
    const char* vertex_code = readFile("question2_vtx.glsl");
    const char* fragment_code = readFile("question2_frag.glsl");

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
