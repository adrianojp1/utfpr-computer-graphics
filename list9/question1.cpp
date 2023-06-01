#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "../lib/utils.h"
#include "../lib/euclidian.h"

using namespace std;
using namespace glm;

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

/** Arrays properties */
#define VERTEX_SIZE 3
#define MAX_POLYGON_VERTICES 100
#define CLIP_RECT_VERTICES 4 * VERTEX_SIZE

#define POLYGON_VERTICES MAX_POLYGON_VERTICES * VERTEX_SIZE
#define CLIP_POLYGON_VERTICES POLYGON_VERTICES + CLIP_RECT_VERTICES
#define MAX_VERTICES CLIP_RECT_VERTICES + POLYGON_VERTICES + CLIP_POLYGON_VERTICES

/** Clipping rectangle vertices. */
float clip_rect[2 * VERTEX_SIZE] = {};
int clip_rect_size = 0;
float clip_rect_color[] = { 1.0f, 1.0f, 1.0f };

/** Polygon. */
float polygon_vertices[POLYGON_VERTICES] = {};
int polygon_vertices_size = 0;
float polygon_color[] = { 0.0f, 1.0f, 1.0f };

/** Clipped polygon vertices. */
float clip_polygon_vertices[CLIP_POLYGON_VERTICES] = {};
int clip_polygon_vertices_size = 0;
float clip_polygon_color[] = { 1.0f, 0.0f, 1.0f };

/** Vertices arrays. */
float vertices[MAX_VERTICES];
float colors[MAX_VERTICES];
int vertices_size = 0;

/** Program states */
#define WAITING_FOR_CLIP 0
#define WAITING_FOR_POLYGON 1
#define SHOWING_POLYGON 2
int program_state = WAITING_FOR_CLIP;

/** Draw mode */
GLenum draw_mode = GL_TRIANGLE_FAN;

/* Functions. */
void display(void);
void reshape(int, int);
void keyboard(unsigned char, int, int);
void mouse(int button, int state, int x, int y);

void toGlCoordsPrint(float win_x, float win_y, float* gl_x, float* gl_y);
void addToClipRect(float x, float y);
void addToPolygon(float x, float y);
void finishClipRect();
void finishPolygon();
void clipPolygon();

void initData(void);
void initShaders(void);

/**
 * Drawing function.
 *
 * Draws a triangle.
 */
void display() {
    glClearColor(0.1, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glBindVertexArray(VAO);

    // Update buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_size, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_size, colors, GL_STATIC_DRAW);

    switch (program_state) {
        case WAITING_FOR_POLYGON:
            cout << "vertices_size: " << vertices_size << endl;
            glDrawArrays(draw_mode, 0, 4);
            break;

        case SHOWING_POLYGON:
            glDrawArrays(draw_mode, 4, polygon_vertices_size);
            glDrawArrays(draw_mode, 0, 4);
            glDrawArrays(draw_mode, 4 + polygon_vertices_size, clip_polygon_vertices_size);
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
        case 't':
            draw_mode = GL_TRIANGLE_FAN;
            break;
        case 'l':
            draw_mode = GL_LINE_LOOP;
            break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    switch (program_state) {
        case WAITING_FOR_CLIP:
            if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
                float scene_x, scene_y;
                toGlCoordsPrint(x, y, &scene_x, &scene_y);
                addToClipRect(scene_x, scene_y);
            }
            break;

        case WAITING_FOR_POLYGON:
            if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
                float scene_x, scene_y;
                toGlCoordsPrint(x, y, &scene_x, &scene_y);
                addToPolygon(scene_x, scene_y);
            } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
                finishPolygon();
            }
            break;
    }
    glutPostRedisplay();
}

void toGlCoordsPrint(float win_x, float win_y, float* gl_x, float* gl_y) {
    cout << "win_x: " << win_x << ", win_y: " << win_y << endl;
    toGlCoords(win_x, win_y, gl_x, gl_y, win_width, win_height);
    cout << "gl_x: " << *gl_x << ", gl_y: " << *gl_y << endl;
}

void addToClipRect(float x, float y) {
    if (clip_rect_size >= 2) {
        cerr << "Invalid state! Can not add more vertices to clipping rectangle." << endl;
        exit(1);
    }

    int idx = clip_rect_size * VERTEX_SIZE;
    clip_rect[idx++] = x;
    clip_rect[idx++] = y;
    clip_rect[idx] = 0.0f;
    clip_rect_size++;

    cout << "clip_rect: ";
    printArray(clip_rect, clip_rect_size * VERTEX_SIZE);

    if (clip_rect_size == 2) {
        finishClipRect();
    }
}

void addToPolygon(float x, float y) {
    if (polygon_vertices_size >= MAX_POLYGON_VERTICES) {
        cerr << "Invalid state! Can not add more vertices to polygon." << endl;
        exit(1);
    }

    int idx = polygon_vertices_size * VERTEX_SIZE;
    polygon_vertices[idx++] = x;
    polygon_vertices[idx++] = y;
    polygon_vertices[idx] = 0.0f;
    polygon_vertices_size++;

    cout << "polygon_vertices: ";
    printArray(polygon_vertices, polygon_vertices_size * VERTEX_SIZE);

    if (polygon_vertices_size == MAX_POLYGON_VERTICES) {
        finishPolygon();
    }
}

void finishClipRect() {
    cout << "Finishing clipping rectangle" << endl;
    float max_x = std::max(clip_rect[0], clip_rect[VERTEX_SIZE]);
    float min_x = std::min(clip_rect[0], clip_rect[VERTEX_SIZE]);
    float max_y = std::max(clip_rect[1], clip_rect[VERTEX_SIZE + 1]);
    float min_y = std::min(clip_rect[1], clip_rect[VERTEX_SIZE + 1]);
    float z = 0.0f;

    int idx = 0;
    vertices[idx++] = min_x;
    vertices[idx++] = min_y;
    vertices[idx++] = z;
    vertices[idx++] = max_x;
    vertices[idx++] = min_y;
    vertices[idx++] = z;
    vertices[idx++] = max_x;
    vertices[idx++] = max_y;
    vertices[idx++] = z;
    vertices[idx++] = min_x;
    vertices[idx++] = max_y;
    vertices[idx++] = z;

    vertices_size += idx;
    copyRepeatedTo(clip_rect_color, VERTEX_SIZE, colors, 0, 4);

    cout << "vertices: ";
    printArray(vertices, vertices_size);
    cout << "colors: ";
    printArray(colors, vertices_size);

    program_state = WAITING_FOR_POLYGON;
}

void finishPolygon() {
    cout << "Finishing polygon" << endl;
    for (int i = 0; i < polygon_vertices_size * VERTEX_SIZE; i++) {
        vertices[vertices_size] = polygon_vertices[i];
        vertices_size++;
    }

    copyRepeatedTo(polygon_color, VERTEX_SIZE, colors, 4, polygon_vertices_size);

    cout << "vertices: ";
    printArray(vertices, vertices_size);
    cout << "colors: ";
    printArray(colors, vertices_size);

    clipPolygon();

    program_state = SHOWING_POLYGON;
}

void clipPolygon() {
    vector<vec2> clipper = to2DVector(vertices, 4);
    vector<vec2> poly = to2DVector(polygon_vertices, polygon_vertices_size);
    cout << "clipper: ";
    printVector(clipper);
    cout << "poly: ";
    printVector(poly);
    vector<vec2> clipped = suthHodgClip(poly, clipper);
    cout << "clipped: ";
    printVector(clipped);
    to3DPointer(clipped, clip_polygon_vertices, &clip_polygon_vertices_size, 0.0f);
    cout << "clip_polygon_vertices: ";
    printArray(clip_polygon_vertices, clip_polygon_vertices_size * VERTEX_SIZE);


    for (int i = 0; i < clip_polygon_vertices_size * VERTEX_SIZE; i++) {
        vertices[vertices_size] = clip_polygon_vertices[i];
        vertices_size++;
    }

    copyRepeatedTo(clip_polygon_color, VERTEX_SIZE, colors, 4 + polygon_vertices_size, clip_polygon_vertices_size);

    cout << "vertices: ";
    printArray(vertices, vertices_size);
    cout << "colors: ";
    printArray(colors, vertices_size);
}

/**
 * Init vertex data.
 *
 * Defines the coordinates for vertices, creates the arrays for OpenGL.
 */
void initData() {
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
