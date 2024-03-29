#pragma once

#include <glm/glm.hpp>

#include "SceneMesh.hpp"
#include "Shader.hpp"

// Transformation modes
#define TRANSLATION_MODE 0
#define ROTATION_MODE 1
#define SCALE_MODE 2

// Visualization modes
#define FACES_MODE 0
#define WIREFRAME_MODE 1

class MeshViewer {
   private:
    static MeshViewer* _instance;

    /** Window size */
    int win_width;
    int win_height;

    glm::vec4 background_color;

    /** Time control */
    int old_time;
    int delta_time;
    int init_time;

    /** Modes */
    short transform_mode;
    short visual_mode;

    /** Shaders */
    const char* vtx_filename;
    const char* frag_filename;
    Shader shader;

    /** Scene mesh */
    SceneMesh scene_mesh;
    float translation_proportion;

    /** Camera */
    glm::vec3 camera_position;
    glm::vec3 camera_target;
    glm::vec3 up_vec;

    /** Projection */
    float projection_fovy;
    float projection_near;
    float projection_far;

    /** MVP Matrices */
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

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

    void fit_view_projection();

    // Control methods
    void switch_visual_mode();
    void transform_mesh(unsigned short key);
    void translate_mesh(unsigned short key);
    void rotate_mesh(unsigned short key);
    void scale_mesh(unsigned short key);
};
