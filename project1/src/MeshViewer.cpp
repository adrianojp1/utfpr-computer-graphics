#include "MeshViewer.hpp"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace glm;

/** Callbacks pure functions */
void display() { MeshViewer::instance()->_display(); }

void reshape(int width, int height) {
    MeshViewer::instance()->_reshape(width, height);
}

void keyboard(unsigned char key, int x, int y) {
    MeshViewer::instance()->_keyboard(key, x, y);
}

void specialKeys(int key, int x, int y) {
    MeshViewer::instance()->_specialKeys(key, x, y);
}

void idle() { MeshViewer::instance()->_idle(); }

/** MeshViewer members */
MeshViewer* MeshViewer::_instance = nullptr;

MeshViewer* MeshViewer::instance() {
    if (_instance == nullptr) {
        _instance = new MeshViewer();
    }
    return _instance;
}

void MeshViewer::init(int argc, char** argv) {
    // Check .obj file argument
    if (argc < 2) {
        cerr << "Mesh file (.obj) path must be given!" << endl;
        exit(-1);
    }
    const char* mesh_filename = argv[1];

    // Init MeshViewer attributes
    init_attributes();

    // Init window
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow(argv[0]);
    glewInit();

    // Load resource files
    loadResources(mesh_filename, vtx_filename, frag_filename);

    // Register callbacks
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutIdleFunc(idle);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    glutMainLoop();
}

void MeshViewer::init_attributes() {
    /** Window width. */
    win_width = 800;
    /** Window height. */
    win_height = 800;

    /** Time control */
    old_time = 0;
    delta_time = 0;
    init_time = glutGet(GLUT_ELAPSED_TIME);

    /** Modes */
    transform_mode = TRANSLATION_MODE;
    visual_mode = FACES_MODE;

    /** Shaders. */
    vtx_filename = "./shaders/vtx.glsl";
    frag_filename = "./shaders/frag.glsl";

    /** Camera. */
    init_camera_position = vec3{ 0.0f, 0.0f, 0.5f };
    camera_target = vec3{ 0.0f, 0.0f, 0.0f };
    up_vec = vec3{ 0.0f, 1.0f, 0.0f };
    camera_rotation = 0.0f;
    camera_rotation_speed = 0.05f;

    init_model = glm::mat4(1.0f);
    init_view = glm::lookAt(init_camera_position, camera_target, up_vec);
    init_projection = glm::perspective(glm::radians(45.0f), (GLfloat)win_width / win_height, 0.1f, 20.0f);
}

void MeshViewer::loadResources(const char* mesh_file, const char* vtx_file, const char* frag_file) {
    // Load mesh
    scene_mesh.load(mesh_file);

    // Create shaders
    shader.loadAndCreateShader(vtx_file, frag_file);
    shader.use();
}

void MeshViewer::_display() {
    glClearColor(0.1, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    unsigned int model_loc = glGetUniformLocation(shader.getId(), "model");
    unsigned int view_matrix_loc = glGetUniformLocation(shader.getId(), "view");
    unsigned int projection_matrix_loc = glGetUniformLocation(shader.getId(), "projection");

    // unsigned int camera_position_loc = glGetUniformLocation(shader.id,
    // "camera_position"); glUniform3f(camera_position_loc, camera_position.x,
    // camera_position.y, camera_position.z);

    mat4 model = scene_mesh.getTransformation();
    // mat4 cam_rotation = rotate(mat4(1.0f), radians(camera_rotation), vec3(0.0f, 1.0f, 0.0f));
    // vec3 camera_position = cam_rotation * vec4(init_camera_position, 1.0f);
    mat4 view = lookAt(init_camera_position, scene_mesh.getCenter(), up_vec);
    mat4 projection = init_projection;

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, value_ptr(model));        // Pass rotation matrix to vertex shader.
    glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, value_ptr(view));   // Uniform: Transfer view matrix to vertex shader.
    glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, value_ptr(projection));

    for (Mesh mesh : scene_mesh.getMeshList()) {
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh.vert_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    glutSwapBuffers();

    int cur_time = glutGet(GLUT_ELAPSED_TIME);
    delta_time = cur_time - old_time;
    old_time = cur_time;
}

void MeshViewer::_reshape(int width, int height) {
    win_width = width;
    win_height = height;
    glViewport(0, 0, width, height);
    glutPostRedisplay();
}

void MeshViewer::_keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:   // Esc
        case 'q':
            glutLeaveMainLoop();
            break;
        case 't':
            transform_mode = TRANSLATION_MODE;
            break;
        case 'r':
            transform_mode = ROTATION_MODE;
            break;
        case 's':
            transform_mode = SCALE_MODE;
            break;
        case 'v':
            if (visual_mode == FACES_MODE) {
                visual_mode = WIREFRAME_MODE;
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                visual_mode = FACES_MODE;
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            break;
        case 'a':
            switch (transform_mode) {
                case TRANSLATION_MODE:
                    scene_mesh.translate(vec3{ 0.0f, 0.0f, -0.01f });
                    break;
                case ROTATION_MODE:
                    scene_mesh.rotate_z(10.0f);
                    break;
                case SCALE_MODE:
                    scene_mesh.scale(vec3{ 0.0f, 0.0f, 0.1f });
                    break;
            }
            break;
        case 'd':
            switch (transform_mode) {
                case TRANSLATION_MODE:
                    scene_mesh.translate(vec3{ 0.0f, 0.0f, 0.01f });
                    break;
                case ROTATION_MODE:
                    scene_mesh.rotate_z(-10.0f);
                    break;
                case SCALE_MODE:
                    scene_mesh.scale(vec3{ 0.0f, 0.0f, -0.1f });
                    break;
            }
            break;
    }

    glutPostRedisplay();
}

void MeshViewer::_specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            switch (transform_mode) {
                case TRANSLATION_MODE:
                    scene_mesh.translate(vec3{ 0.0f, 0.01f, 0.0f });
                    break;
                case ROTATION_MODE:
                    scene_mesh.rotate_x(10.0f);
                    break;
                case SCALE_MODE:
                    scene_mesh.scale(vec3{ 0.0f, 0.1f, 0.0f });
                    break;
            }
            break;
        case GLUT_KEY_DOWN:
            switch (transform_mode) {
                case TRANSLATION_MODE:
                    scene_mesh.translate(vec3{ 0.0f, -0.01f, 0.0f });
                    break;
                case ROTATION_MODE:
                    scene_mesh.rotate_x(-10.0f);
                    break;
                case SCALE_MODE:
                    scene_mesh.scale(vec3{ 0.0f, -0.1f, 0.0f });
                    break;
            }
            break;
        case GLUT_KEY_RIGHT:
            switch (transform_mode) {
                case TRANSLATION_MODE:
                    scene_mesh.translate(vec3{ 0.01f, 0.0f, 0.0f });
                    break;
                case ROTATION_MODE:
                    scene_mesh.rotate_y(10.0f);
                    break;
                case SCALE_MODE:
                    scene_mesh.scale(vec3{ 0.1f, 0.0f, 0.0f });
                    break;
            }
            break;
        case GLUT_KEY_LEFT:
            switch (transform_mode) {
                case TRANSLATION_MODE:
                    scene_mesh.translate(vec3{ -0.01f, 0.0f, 0.0f });
                    break;
                case ROTATION_MODE:
                    scene_mesh.rotate_y(-10.0f);
                    break;
                case SCALE_MODE:
                    scene_mesh.scale(vec3{ -0.1f, 0.0f, 0.0f });
                    break;
            }
            break;
    }
}

void MeshViewer::_idle() {
    glutPostRedisplay();
}