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

// Transformation modes
#define TRANSLATION_MODE 0
#define ROTATION_MODE 1
#define SCALE_MODE 2

// Polygon modes
#define FACES_MODE 0
#define WIREFRAME_MODE 1

// Color modes
#define LIGHTNING_MODE 0
#define TEXTURE_MODE 1
#define TEXTURE_NORMAL_MODE 2

// Axis directions
const vec3 axis_x_dir = { 1.0f, 0.0f, 0.0f };
const vec3 axis_y_dir = { 0.0f, 1.0f, 0.0f };
const vec3 axis_z_dir = { 0.0f, 0.0f, 1.0f };

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
    if (argc < 4) {
        cerr << "Usage: ./mesh2 object.obj texture.ext normal_map.ext2" << endl;
        exit(-1);
    }
    string mesh_filename = argv[1];
    string texture_filename = argv[2];
    string normal_map_filename = argv[3];

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
    shaders.push_back(new Shader("./shaders/normal_vtx.glsl", "./shaders/normal_frag.glsl"));

    /** Camera */
    camera_position = vec3{ 0.0f, 0.0f, 0.0f };
    camera_target = vec3{ 0.0f, 0.0f, 0.0f };
    up_vec = vec3{ 0.0f, 1.0f, 0.0f };

    /** Light */
    light_color = vec3{ 1.0f, 1.0f, 1.0f };
    light_position = vec3{ 0.0f, 0.0f, 0.0f };

    /** Color */
    background_color = vec3{ 0.0f, 0.1f, 0.15f };
    default_object_color = vec3{ 0.5f, 0.5f, 0.5f };

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
    texture = new CubemapTexture(texture_file, normal_map_file, is_flat);
    texture->load();
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
    light_position.x += scene_front_size / 2.0f * 0.7f;
    light_position.y += scene_front_size / 2.0f * 0.5f;

    // Init translation proportion
    translation_proportion = 0.05 * std::max(std::max(scene_box_size.x, scene_box_size.y), scene_box_size.z);
}

void MeshViewer::_display() {
    glClearColor(background_color.r, background_color.g, background_color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model = scene_mesh.getTransformation();

    Shader* shader = shaders[color_mode];

    shader->setVec3("light_color", light_color);
    shader->setVec3("light_position", light_position);
    shader->setVec3("camera_position", camera_position);

    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    switch (color_mode) {
        case LIGHTNING_MODE:
            bindLightMode(shader);
            break;
        case TEXTURE_MODE:
            bindTextMode(shader);
            break;
        case TEXTURE_NORMAL_MODE:
            bindTextNormalMode(shader);
            break;
    }

    for (Mesh mesh : scene_mesh.getMeshList()) {
        glBindVertexArray(mesh.VAO);
        // glDrawElements(GL_TRIANGLES, (GLsizei)mesh.vert_normals.size(), GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)mesh.vert_positions.size());
        glBindVertexArray(0);
    }

    glutSwapBuffers();

    int cur_time = glutGet(GLUT_ELAPSED_TIME);
    delta_time = cur_time - old_time;
    old_time = cur_time;
}

void MeshViewer::bindLightMode(Shader* shader) {
    shader->setVec3("object_color", default_object_color);
}

void MeshViewer::bindTextMode(Shader* shader) {
    shader->setVec3("object_center", scene_mesh.getCenter());
    shader->setInt("diffuse_map", 0);
}

void MeshViewer::bindTextNormalMode(Shader* shader) {
    bindTextMode(shader);
    shader->setInt("normal_map", 1);
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
    if (mode == TEXTURE_NORMAL_MODE && !texture->hasNormalMap()) {
        cerr << "Texture does not have normal map!" << endl;
    } else {
        color_mode = mode;
        shaders[color_mode]->use();
        cout << "Color mode set to " << color_mode << ", shader " << shaders[color_mode]->getId() << endl;
    }
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
            scene_mesh.translate(axis_y_dir * translation_proportion);
            break;
        case KEY_DOWN:
            scene_mesh.translate(axis_y_dir * -translation_proportion);
            break;
        case KEY_RIGHT:
            scene_mesh.translate(axis_x_dir * translation_proportion);
            break;
        case KEY_LEFT:
            scene_mesh.translate(axis_x_dir * -translation_proportion);
            break;
        case KEY_A:
            scene_mesh.translate(axis_z_dir * translation_proportion);
            break;
        case KEY_D:
            scene_mesh.translate(axis_z_dir * -translation_proportion);
            break;
    }
}

void MeshViewer::rotateMesh(unsigned short key) {
    switch (key) {
        case KEY_UP:
            scene_mesh.rotate(10.0f, axis_x_dir);
            break;
        case KEY_DOWN:
            scene_mesh.rotate(-10.0f, axis_x_dir);
            break;
        case KEY_RIGHT:
            scene_mesh.rotate(10.0f, axis_y_dir);
            break;
        case KEY_LEFT:
            scene_mesh.rotate(-10.0f, axis_y_dir);
            break;
        case KEY_A:
            scene_mesh.rotate(10.0f, axis_z_dir);
            break;
        case KEY_D:
            scene_mesh.rotate(-10.0f, axis_z_dir);
            break;
    }
}

void MeshViewer::scaleMesh(unsigned short key) {
    switch (key) {
        case KEY_UP:
            scene_mesh.scale(axis_y_dir * 0.1f);
            break;
        case KEY_DOWN:
            scene_mesh.scale(axis_y_dir * -0.1f);
            break;
        case KEY_RIGHT:
            scene_mesh.scale(axis_x_dir * 0.1f);
            break;
        case KEY_LEFT:
            scene_mesh.scale(axis_x_dir * -0.1f);
            break;
        case KEY_A:
            scene_mesh.scale(axis_z_dir * 0.1f);
            break;
        case KEY_D:
            scene_mesh.scale(axis_z_dir * -0.1f);
            break;
    }
}
