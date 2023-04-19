#pragma once

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include <vector>

#define ASSIMP_PROCESSING_FLAGS aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenBoundingBoxes


/** Single mesh data class */
struct Mesh {
    unsigned int VAO, VBO1, VBO2, EBO;

    std::vector<glm::vec3> vert_positions;
    std::vector<glm::vec3> vert_normals;
    std::vector<unsigned int> vert_indices;
};

class SceneMesh {
   private:

    // Assimp attributes
    Assimp::Importer importer;   // https://assimp-docs.readthedocs.io/en/v5.1.0/ ... (An older Assimp website: http://assimp.sourceforge.net/lib_html/index.html)
    const aiScene* scene;
    aiNode* root_node;   // Only being used in the: load_model_cout_console() function.

    // Mesh attributes
    unsigned int num_meshes;
    std::vector<Mesh> mesh_list;
    glm::vec3 center;
    glm::vec3 bound_box_max;
    glm::vec3 bound_box_min;

    // Transformation parameters
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 _scale;

    // Transformation matrices
    glm::mat4 translation_mat;
    glm::mat4 rotation_x_mat;
    glm::mat4 rotation_y_mat;
    glm::mat4 rotation_z_mat;
    glm::mat4 scale_mat;

    glm::mat4 transformation;

   public:
    SceneMesh();

    void load(const char* mesh_path);

    // Transformation
    void translate(glm::vec3 translation);
    void rotate_x(float angles);
    void rotate_y(float angles);
    void rotate_z(float angles);
    void scale(glm::vec3 scale_increment);

    // Getters
    unsigned int getNumMeshes() const;
    std::vector<Mesh> getMeshList() const;
    glm::vec3 getCenter() const;
    glm::vec3 getBoundBoxMax() const;
    glm::vec3 getBoundBoxMin() const;
    glm::mat4 getTransformation() const;

   private:
    // Load methods
    void load_model();
    void load_model_cout_console();
    void update_scene_bound_box(aiAABB bound_box);
    void set_buffer_data(unsigned int index);

    void update_transformation();
};