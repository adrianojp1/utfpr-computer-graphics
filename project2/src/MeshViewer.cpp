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

// Control keys
#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_RIGHT 2
#define KEY_LEFT 3
#define KEY_A 4
#define KEY_D 5

// Axis vectors
#define AXIS_X \
    vec3 { 1.0f, 0.0f, 0.0f }
#define AXIS_Y \
    vec3 { 0.0f, 1.0f, 0.0f }
#define AXIS_Z \
    vec3 { 0.0f, 0.0f, 1.0f }

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
    if (argc < 3) {
        cerr << "Usage: ./mesh2 object.obj texture.ext normal_map.ext2" << endl;
        exit(-1);
    }
    string mesh_filename = argv[1];
    string texture_filename = argv[2];
    // const string normal_map_filename = argv[3];
    string normal_map_filename = "";

    // Init MeshViewer attributes
    initAttributes();

    // Init window
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow(argv[0]);
    glewInit();

    // Load resource files
    loadResources(mesh_filename, texture_filename, normal_map_filename);

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

void MeshViewer::initAttributes() {
    /** Window size */
    win_width = 800;
    win_height = 800;

    background_color = vec4{ 1.0f, 0.0f, 1.0f, 0.0f };

    /** Time control */
    old_time = 0;
    delta_time = 0;
    init_time = glutGet(GLUT_ELAPSED_TIME);

    /** Modes */
    transform_mode = TRANSLATION_MODE;
    polygon_mode = FACES_MODE;
    color_mode = LIGHTNING_MODE;

    /** Shaders */
    shaders.push_back(new Shader("./shaders/light_vtx.glsl", "./shaders/light_frag.glsl"));
    shaders.push_back(new Shader("./shaders/text_vtx.glsl", "./shaders/text_frag.glsl"));
    shaders.push_back(new Shader("./shaders/text_vtx.glsl", "./shaders/text_frag.glsl"));

    /** Camera */
    camera_position = vec3{ 0.0f, 0.0f, 0.0f };
    camera_target = vec3{ 0.0f, 0.0f, 0.0f };
    up_vec = vec3{ 0.0f, 1.0f, 0.0f };

    /** Light */
    light_color = vec3{ 1.0f, 1.0f, 1.0f };
    light_position = vec3{ 0.0f, 0.0f, 0.0f };

    /** Projection */
    projection_fovy = 45.0f;
    projection_near = 0.1f;

    model = mat4{ 1.0f };
    view = mat4{ 1.0f };
    projection = mat4{ 1.0f };
}

void MeshViewer::loadResources(string mesh_file, string texture_file, string normal_map_file) {
    // Load mesh
    scene_mesh.load(mesh_file);

    fitViewProjection();

    // Load shaders
    for (Shader* s : shaders) {
        s->load();
    }
    changeColorMode(color_mode);

    bool is_flat = texture_file.find("flat") != string::npos;
    texture = new CubemapTexture(texture_file);
    texture->load(is_flat);
    texture->use();
}

void MeshViewer::fitViewProjection() {
    vec3 scene_box_size = scene_mesh.getBoundBoxMax() - scene_mesh.getBoundBoxMin();
    float scene_front_size = std::max(scene_box_size.x, scene_box_size.y);
    float scene_depth = scene_box_size.z;

    // Set view to be 3 times the scene front size
    float half_view = scene_front_size * 3 / 2;
    float camera_distance = (half_view / tan(projection_fovy / 2)) + scene_depth / 2;

    camera_target = scene_mesh.getCenter();
    camera_position = scene_mesh.getCenter();
    camera_position.z = camera_distance;

    float projection_far = scene_depth * 100 + camera_distance;
    view = lookAt(camera_position, camera_target, up_vec);
    projection = perspective(radians(projection_fovy), (GLfloat)win_width / win_height, projection_near, projection_far);

    light_position = camera_position;

    // Init translation proportion
    translation_proportion = 0.05 * std::max(std::max(scene_box_size.x, scene_box_size.y), scene_box_size.z);
}

void MeshViewer::_display() {
    glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int shader_id = shaders[color_mode]->getId();

    switch (color_mode) {
        case LIGHTNING_MODE:
            bindLightMode(shader_id);
            break;
        case TEXTURE_MODE:
            bindTextMode(shader_id);
            break;
        case TEXTURE_NORMAL_MODE:
            bindTextNormalMode(shader_id);
            break;
    }

    model = scene_mesh.getTransformation();

    unsigned int light_color_loc = glGetUniformLocation(shader_id, "light_color");
    unsigned int light_position_loc = glGetUniformLocation(shader_id, "light_position");
    unsigned int camera_position_loc = glGetUniformLocation(shader_id, "camera_position");

    unsigned int model_loc = glGetUniformLocation(shader_id, "model");
    unsigned int view_loc = glGetUniformLocation(shader_id, "view");
    unsigned int projection_loc = glGetUniformLocation(shader_id, "projection");

    glUniform3f(light_color_loc, light_color.r, light_color.g, light_color.b);
    glUniform3f(light_position_loc, light_position.x, light_position.y, light_position.z);
    glUniform3f(camera_position_loc, camera_position.x, camera_position.y, camera_position.z);

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, value_ptr(model));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection));

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

void MeshViewer::bindLightMode(int shader_id) {
    unsigned int object_color_loc = glGetUniformLocation(shader_id, "object_color");
    glUniform3f(object_color_loc, 0.7f, 0.7f, 0.0f);
}

void MeshViewer::bindTextMode(int shader_id) {
    vec3 object_center = scene_mesh.getCenter();

    unsigned int object_center_loc = glGetUniformLocation(shader_id, "object_center");
    glUniform3f(object_center_loc, object_center.x, object_center.y, object_center.z);
}

void MeshViewer::bindTextNormalMode(int shader_id) {
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
        case '1':
            changeColorMode(LIGHTNING_MODE);
            break;
        case '2':
            changeColorMode(TEXTURE_MODE);
            break;
        case '3':
            changeColorMode(TEXTURE_NORMAL_MODE);
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
            switchPolygonMode();
            break;
        case 'a':
            transformMesh(KEY_A);
            break;
        case 'd':
            transformMesh(KEY_D);
            break;
    }

    glutPostRedisplay();
}

void MeshViewer::_specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            transformMesh(KEY_UP);
            break;
        case GLUT_KEY_DOWN:
            transformMesh(KEY_DOWN);
            break;
        case GLUT_KEY_RIGHT:
            transformMesh(KEY_RIGHT);
            break;
        case GLUT_KEY_LEFT:
            transformMesh(KEY_LEFT);
            break;
    }
}

void MeshViewer::_idle() {
    glutPostRedisplay();
}

void MeshViewer::changeColorMode(unsigned short mode) {
    color_mode = mode;
    shaders[color_mode]->use();
    cout << "Color mode set to " << color_mode << ", shader " << shaders[color_mode]->getId() << endl;
}

void MeshViewer::switchPolygonMode() {
    if (polygon_mode == FACES_MODE) {
        polygon_mode = WIREFRAME_MODE;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        cout << "Polygon mode set to wireframe" << endl;
    } else {
        polygon_mode = FACES_MODE;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        cout << "Polygon mode set to faces" << endl;
    }
}

void MeshViewer::transformMesh(unsigned short key) {
    switch (transform_mode) {
        case TRANSLATION_MODE:
            translateMesh(key);
            break;
        case ROTATION_MODE:
            rotateMesh(key);
            break;
        case SCALE_MODE:
            scaleMesh(key);
            break;
    }
}

void MeshViewer::translateMesh(unsigned short key) {
    switch (key) {
        case KEY_UP:
            scene_mesh.translate(AXIS_Y * translation_proportion);
            break;
        case KEY_DOWN:
            scene_mesh.translate(AXIS_Y * -translation_proportion);
            break;
        case KEY_RIGHT:
            scene_mesh.translate(AXIS_X * translation_proportion);
            break;
        case KEY_LEFT:
            scene_mesh.translate(AXIS_X * -translation_proportion);
            break;
        case KEY_A:
            scene_mesh.translate(AXIS_Z * translation_proportion);
            break;
        case KEY_D:
            scene_mesh.translate(AXIS_Z * -translation_proportion);
            break;
    }
}

void MeshViewer::rotateMesh(unsigned short key) {
    switch (key) {
        case KEY_UP:
            scene_mesh.rotate(10.0f, AXIS_X);
            break;
        case KEY_DOWN:
            scene_mesh.rotate(-10.0f, AXIS_X);
            break;
        case KEY_RIGHT:
            scene_mesh.rotate(10.0f, AXIS_Y);
            break;
        case KEY_LEFT:
            scene_mesh.rotate(-10.0f, AXIS_Y);
            break;
        case KEY_A:
            scene_mesh.rotate(10.0f, AXIS_Z);
            break;
        case KEY_D:
            scene_mesh.rotate(-10.0f, AXIS_Z);
            break;
    }
}

void MeshViewer::scaleMesh(unsigned short key) {
    switch (key) {
        case KEY_UP:
            scene_mesh.scale(AXIS_Y * 0.1f);
            break;
        case KEY_DOWN:
            scene_mesh.scale(AXIS_Y * -0.1f);
            break;
        case KEY_RIGHT:
            scene_mesh.scale(AXIS_X * 0.1f);
            break;
        case KEY_LEFT:
            scene_mesh.scale(AXIS_X * -0.1f);
            break;
        case KEY_A:
            scene_mesh.scale(AXIS_Z * 0.1f);
            break;
        case KEY_D:
            scene_mesh.scale(AXIS_Z * -0.1f);
            break;
    }
}
