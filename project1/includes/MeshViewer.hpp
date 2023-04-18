#pragma once

#include <glm/glm.hpp>

#include "SceneMesh.hpp"
#include "Shader.hpp"

#define TRANSLATION_MODE 0
#define ROTATION_MODE 1
#define SCALE_MODE 2

#define FACES_MODE 0
#define WIREFRAME_MODE 1

class MeshViewer {
   private:
    static MeshViewer* _instance;

    /** Window width. */
    int win_width;
    /** Window height. */
    int win_height;

    /** Time control */
    int old_time;
    int delta_time;
    int init_time;

    /** Modes */
    short transform_mode;
    short visual_mode;

    /** Shaders. */
    const char* vtx_filename;
    const char* frag_filename;
    Shader shader;

    /** Scene mesh. */
    SceneMesh scene_mesh;

    /** Camera. */
    glm::vec3 init_camera_position;
    glm::vec3 camera_target;
    glm::vec3 up_vec;
    float camera_rotation;
    float camera_rotation_speed;

    /** MVP Matrices */
    glm::mat4 init_model;
    glm::mat4 init_view;
    glm::mat4 init_projection;

   public:
    static MeshViewer* instance();

    void init(int argc, char** argv);

    /** Callbacks. */
    void _display();
    void _reshape(int width, int height);
    void _keyboard(unsigned char key, int x, int y);
    void _specialKeys(int key, int x, int y);
    void _idle();

   private:
    MeshViewer(){};

    void init_attributes();

    void loadResources(const char* mesh_file, const char* vtx_file, const char* frag_file);
};
