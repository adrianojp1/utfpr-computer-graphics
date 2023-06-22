/**
 * Based on https://www.programmingcreatively.com/opengl-tutorial-5-qs.php
 */

#pragma once

#define AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE 60

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include <vector>

/** Single mesh data class */
class Mesh {
   public:
    unsigned int VAO, VBO1, VBO2, VBO3, EBO;

    std::vector<glm::vec3> vert_positions;
    std::vector<glm::vec3> vert_normals;
    std::vector<glm::vec3> vert_tangents;
    std::vector<unsigned int> vert_indices;

    glm::vec3 center;
};

class SceneMesh {
   private:
    // Assimp attributes
    Assimp::Importer importer;
    const aiScene* scene;

    // Mesh attributes
    unsigned int num_meshes;
    std::vector<Mesh> mesh_list;
    glm::vec3 center;
    glm::vec3 bound_box_max;
    glm::vec3 bound_box_min;

    // Transformation parameters
    glm::vec3 _translation;
    glm::vec3 _rotation;
    glm::vec3 _scale;

    // Transformation matrix
    glm::mat4 translation_mat;
    glm::mat4 rotation_mat;
    glm::mat4 scale_mat;
    glm::mat4 transformation_mat;

   public:
    SceneMesh();

    void load(const std::string mesh_path);

    // Transformation
    void translate(glm::vec3 translation);
    void rotate(float degrees, glm::vec3 axis);
    void scale(glm::vec3 scale);

    // Getters
    unsigned int getNumMeshes() const;
    std::vector<Mesh> getMeshList() const;
    glm::vec3 getCenter() const;
    glm::vec3 getBoundBoxMax() const;
    glm::vec3 getBoundBoxMin() const;
    glm::mat4 getTransformation() const;

   private:
    // Load methods
    void loadModel();
    void setBufferData(unsigned int index);
    void calcTangentSpace(unsigned int index);

    void updateTransformation();
};