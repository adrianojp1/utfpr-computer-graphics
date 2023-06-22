#pragma once

#include <glm/glm.hpp>

#include "SceneMesh.hpp"
#include "Shader.hpp"
#include "CubemapTexture.hpp"

class MeshViewer {
   private:
    static MeshViewer* _instance;

    /** Window size */
    int win_width;
    int win_height;

    /** Time control */
    int old_time;
    int delta_time;
    int init_time;

    /** Modes */
    short transform_mode;
    short polygon_mode;
    short color_mode;

    /** Shaders */
    std::vector<Shader*> shaders;

    /** Scene mesh */
    SceneMesh scene_mesh;
    float translation_proportion;

    /** Camera */
    glm::vec3 camera_position;
    glm::vec3 camera_target;
    glm::vec3 up_vec;

    /** Light */
    glm::vec3 light_color;
    glm::vec3 light_position;

    /** Color */
    glm::vec3 background_color;
    glm::vec3 default_object_color;

    /** Texture */
    CubemapTexture* texture;

    /** Projection */
    float projection_fovy;
    float projection_near;

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

    void initAttributes();

    void loadResources(const std::string mesh_file, const std::string texture_file, const std::string normal_map_file);

    void fitViewProjection();

    void bindLightMode(Shader* shader);
    void bindTextMode(Shader* shader);
    void bindTextNormalMode(Shader* shader);

    // Control methods
    void changeColorMode(unsigned short mode);
    void switchPolygonMode();
    void transformMesh(unsigned short key);
    void translateMesh(unsigned short key);
    void rotateMesh(unsigned short key);
    void scaleMesh(unsigned short key);
};
