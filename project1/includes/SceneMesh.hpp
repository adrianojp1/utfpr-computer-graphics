#pragma once

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include <vector>

#define ASSIMP_PROCESSING_FLAGS aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenBoundingBoxes

class SceneMesh {
   public:
    struct Mesh {
        unsigned int VAO, VBO1, VBO2, EBO;   // Buffer handles (Typically type: GLuint is used)

        std::vector<glm::vec3> vert_positions;
        std::vector<glm::vec3> vert_normals;
        std::vector<unsigned int> vert_indices;
    };

    unsigned int num_meshes;
    std::vector<Mesh> mesh_list;

    glm::vec3 center{ 0.0f, 0.0f, 0.0f };
    glm::vec3 bound_box_max{ 0.0f, 0.0f, 0.0f };
    glm::vec3 bound_box_min{ 0.0f, 0.0f, 0.0f };

    glm::mat4 transformation{ 1.0f };

   private:
    Assimp::Importer importer;   // https://assimp-docs.readthedocs.io/en/v5.1.0/ ... (An older Assimp website: http://assimp.sourceforge.net/lib_html/index.html)
    const aiScene* scene = nullptr;
    aiNode* root_node = nullptr;   // Only being used in the: load_model_cout_console() function.

    // Transformation
    glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
    glm::vec3 _scale{ 1.0f, 1.0f, 1.0f };
    glm::mat4 translation_mat{ 1.0f };
    glm::mat4 rotation_x_mat{ 1.0f };
    glm::mat4 rotation_y_mat{ 1.0f };
    glm::mat4 rotation_z_mat{ 1.0f };
    glm::mat4 scale_mat{ 1.0f };

   public:
    SceneMesh();

    void load(const char* mesh_path);

    void translate(glm::vec3 translation);
    void rotate_x(float angles);
    void rotate_y(float angles);
    void rotate_z(float angles);
    void scale(glm::vec3 scale);

   private:
    void load_model();

    void load_model_cout_console();

    void update_scene_bound_box(aiAABB bound_box);

    void set_buffer_data(unsigned int index);

    void update_transformation();
};