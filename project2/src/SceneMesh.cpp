#include "SceneMesh.hpp"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <limits>
#include <assimp/postprocess.h>

using namespace std;
using namespace glm;

#define ASSIMP_PROCESSING_FLAGS aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_GenSmoothNormals

const float min_float = numeric_limits<float>::min();
const float max_float = numeric_limits<float>::max();

SceneMesh::SceneMesh() {
    center = vec3{ 0.0f, 0.0f, 0.0f };
    bound_box_max = vec3{ min_float, min_float, min_float };
    bound_box_min = vec3{ max_float, max_float, max_float };

    _translation = vec3{ 0.0f, 0.0f, 0.0f };
    _rotation = vec3{ 0.0f, 0.0f, 0.0f };
    _scale = vec3{ 1.0f, 1.0f, 1.0f };

    translation_mat = mat4{ 1.0f };
    rotation_mat = mat4{ 1.0f };
    scale_mat = mat4{ 1.0f };
    transformation_mat = mat4{ 1.0f };

    scene = nullptr;
}

void SceneMesh::load(const string mesh_path) {
    cout << "Reading mesh from file: " << mesh_path << endl;
    scene = importer.ReadFile(mesh_path, ASSIMP_PROCESSING_FLAGS);

    loadModel();
}

void SceneMesh::loadModel() {
    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        cout << "Assimp importer.ReadFile (Error) -- " << importer.GetErrorString() << "\n";
        exit(-1);
    } else {
        num_meshes = scene->mNumMeshes;
        mesh_list.resize(num_meshes);

        aiMesh* mesh{};

        // (1) Loop through all the model's meshes
        // -----------------------------------------------------
        for (unsigned int i = 0; i < num_meshes; ++i) {
            mesh = scene->mMeshes[i];

            updateSceneBoundBox(mesh->mAABB);

            // (2) Loop through all mesh [i]'s vertices
            // ---------------------------------------------------
            for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2) {
                vec3 position(mesh->mVertices[i2].x, mesh->mVertices[i2].y, mesh->mVertices[i2].z);
                mesh_list[i].vert_positions.push_back(position);

                if (mesh->HasNormals()) {
                    vec3 normal(mesh->mNormals[i2].x, mesh->mNormals[i2].y, mesh->mNormals[i2].z);
                    mesh_list[i].vert_normals.push_back(normal);
                } else {
                    mesh_list[i].vert_normals.push_back(vec3(0.0f, 0.0f, 0.0f));
                }
            }

            // (3) Loop through all mesh [i]'s Indices
            // --------------------------------------------------
            for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3) {
                for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4) {
                    mesh_list[i].vert_indices.push_back(mesh->mFaces[i3].mIndices[i4]);
                }
            }

            setBufferData(i);   // Set up: VAO, VBO and EBO.
        }

        center = (bound_box_max + bound_box_min) / 2.0f;
    }
}

void SceneMesh::updateSceneBoundBox(aiAABB bound_box) {
    bound_box_max.x = std::max(bound_box_max.x, (float)bound_box.mMax.x);
    bound_box_max.y = std::max(bound_box_max.y, (float)bound_box.mMax.y);
    bound_box_max.z = std::max(bound_box_max.z, (float)bound_box.mMax.z);

    bound_box_min.x = std::min(bound_box_min.x, (float)bound_box.mMin.x);
    bound_box_min.y = std::min(bound_box_min.y, (float)bound_box.mMin.y);
    bound_box_min.z = std::min(bound_box_min.z, (float)bound_box.mMin.z);
}

void SceneMesh::setBufferData(unsigned int index) {
    glGenVertexArrays(1, &mesh_list[index].VAO);
    glGenBuffers(1, &mesh_list[index].VBO1);
    glGenBuffers(1, &mesh_list[index].VBO2);
    glGenBuffers(1, &mesh_list[index].EBO);

    glBindVertexArray(mesh_list[index].VAO);

    // Vertex Positions
    // ---------------------
    glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * mesh_list[index].vert_positions.size(), &mesh_list[index].vert_positions[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);   // Void pointer below is for legacy reasons. Two possible meanings: "offset for buffer objects" & "address for client state arrays"
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Vertex Normals
    // --------------------
    glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * mesh_list[index].vert_normals.size(), &mesh_list[index].vert_normals[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Indices for: glDrawElements()
    // ---------------------------------------
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_list[index].EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh_list[index].vert_indices.size(), &mesh_list[index].vert_indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);   // Unbind VAO
}

void SceneMesh::translate(glm::vec3 translation) {
    _translation += translation;
    translation_mat = glm::translate(translation_mat, translation);
    updateTransformation();
}

void SceneMesh::rotate(float degrees, glm::vec3 axis) {
    _rotation += axis * degrees;
    mat4 from_center = glm::translate(mat4{ 1.0f }, -center);
    mat4 rot = glm::rotate(mat4{ 1.0f }, radians(degrees), axis);
    mat4 to_center = glm::translate(mat4{ 1.0f }, center);
    rotation_mat = to_center * rot * from_center * rotation_mat;
    updateTransformation();
}

void SceneMesh::scale(glm::vec3 scale) {
    _scale += scale;
    mat4 from_center = glm::translate(mat4{ 1.0f }, -center);
    mat4 _scale_mat = glm::scale(mat4{ 1.0f }, _scale);
    mat4 to_center = glm::translate(mat4{ 1.0f }, center);
    scale_mat = to_center * _scale_mat * from_center;
    updateTransformation();
}

void SceneMesh::updateTransformation() {
    transformation_mat = translation_mat * rotation_mat * scale_mat;
}

unsigned int SceneMesh::getNumMeshes() const { return num_meshes; }
std::vector<Mesh> SceneMesh::getMeshList() const { return mesh_list; }
glm::vec3 SceneMesh::getCenter() const { return center; }
glm::vec3 SceneMesh::getBoundBoxMax() const { return bound_box_max; }
glm::vec3 SceneMesh::getBoundBoxMin() const { return bound_box_min; }
glm::mat4 SceneMesh::getTransformation() const { return transformation_mat; }
