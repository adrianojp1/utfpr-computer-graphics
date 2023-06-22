#include "SceneMesh.hpp"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <limits>
#include <assimp/postprocess.h>
#include <utils.hpp>

using namespace std;
using namespace glm;

#define ASSIMP_PROCESSING_FLAGS aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_GenSmoothNormals

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

            // Update scene bounding box and center
            aiAABB* mesh_bound_box = &mesh->mAABB;
            vec3 mesh_max = toVec3(mesh_bound_box->mMax);
            vec3 mesh_min = toVec3(mesh_bound_box->mMin);

            bound_box_max = glm::max(bound_box_max, mesh_max);
            bound_box_min = glm::min(bound_box_min, mesh_min);

            mesh_list[i].center = (mesh_max + mesh_min) / 2.0f;
            center += mesh_list[i].center;

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

                mesh_list[i].vert_tangents.push_back(vec3(0.0f, 0.0f, 0.0f));
            }

            // (3) Loop through all mesh [i]'s Indices
            // --------------------------------------------------
            // for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3) {
            //     for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4) {
            //         mesh_list[i].vert_indices.push_back(mesh->mFaces[i3].mIndices[i4]);
            //     }
            // }

            calcTangentSpace(i);
            setBufferData(i);   // Set up: VAO, VBO and EBO.
        }

        center /= (float)num_meshes;
    }
}

void SceneMesh::setBufferData(unsigned int index) {
    glGenVertexArrays(1, &mesh_list[index].VAO);
    glGenBuffers(1, &mesh_list[index].VBO1);
    glGenBuffers(1, &mesh_list[index].VBO2);
    glGenBuffers(1, &mesh_list[index].VBO3);
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

    // Vertex Tangents
    // --------------------
    glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * mesh_list[index].vert_tangents.size(), &mesh_list[index].vert_tangents[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Indices for: glDrawElements()
    // ---------------------------------------
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_list[index].EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh_list[index].vert_indices.size(), &mesh_list[index].vert_indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);   // Unbind VAO
}

void SceneMesh::calcTangentSpace(unsigned int index) {
    int i = 0;
    int i1, i2, i3;
    float f;
    vec3 pos1, pos2, pos3, edge1, edge2, tan;
    vec2 uv1, uv2, uv3, deltaUV1, deltaUV2;

    while (i < mesh_list[index].vert_positions.size()) {
        i1 = i++;
        i2 = i++;
        i3 = i++;

        pos1 = mesh_list[index].vert_positions[i1];
        pos2 = mesh_list[index].vert_positions[i2];
        pos3 = mesh_list[index].vert_positions[i3];

        vec3 mesh_center = mesh_list[index].center;
        uv1 = toCubeUV(pos1 - mesh_center, mesh_list[index].vert_normals[i1]);
        uv2 = toCubeUV(pos2 - mesh_center, mesh_list[index].vert_normals[i2]);
        uv3 = toCubeUV(pos3 - mesh_center, mesh_list[index].vert_normals[i3]);

        edge1 = pos2 - pos1;
        edge2 = pos3 - pos1;
        deltaUV1 = uv2 - uv1;
        deltaUV2 = uv3 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tan.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tan.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tan.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        mesh_list[index].vert_tangents[i1] += tan;
        mesh_list[index].vert_tangents[i2] += tan;
        mesh_list[index].vert_tangents[i3] += tan;
    }
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
