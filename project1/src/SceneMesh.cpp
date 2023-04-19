#include "SceneMesh.hpp"

#include <GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <limits>

using namespace std;
using namespace glm;

const float min_float = numeric_limits<float>::min();
const float max_float = numeric_limits<float>::max();

SceneMesh::SceneMesh() {
    center = vec3{ 0.0f, 0.0f, 0.0f };
    bound_box_max = vec3{ min_float, min_float, min_float };
    bound_box_min = vec3{ max_float, max_float, max_float };

    transformation = mat4{ 1.0f };

    scene = nullptr;
    root_node = nullptr;

    position = vec3{ 0.0f, 0.0f, 0.0f };
    rotation = vec3{ 0.0f, 0.0f, 0.0f };
    _scale = vec3{ 1.0f, 1.0f, 1.0f };

    translation_mat = mat4{ 1.0f };
    rotation_x_mat = mat4{ 1.0f };
    rotation_y_mat = mat4{ 1.0f };
    rotation_z_mat = mat4{ 1.0f };
    scale_mat = mat4{ 1.0f };
}

void SceneMesh::load(const char* mesh_path) {
    cout << "Reading mesh from file: " << mesh_path << endl;
    // http://assimp.sourceforge.net/lib_html/postprocess_8h.html (See: aiPostProcessSteps) (Flag options)
    scene = importer.ReadFile(mesh_path, ASSIMP_PROCESSING_FLAGS);

    load_model();
    // load_model_cout_console();
}

void SceneMesh::load_model() {
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

            update_scene_bound_box(mesh->mAABB);

            // (2) Loop through all mesh [i]'s vertices
            // ---------------------------------------------------
            for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2) {
                vec3 position(mesh->mVertices[i2].x, mesh->mVertices[i2].y, mesh->mVertices[i2].z);
                mesh_list[i].vert_positions.push_back(position);

                if (mesh->HasNormals()) {
                    vec3 normal(mesh->mNormals[i2].x, mesh->mNormals[i2].y, mesh->mNormals[i2].z);
                    mesh_list[i].vert_normals.push_back(normal);
                } else
                    mesh_list[i].vert_normals.push_back(vec3(0.0f, 0.0f, 0.0f));
            }

            // (3) Loop through all mesh [i]'s Indices
            // --------------------------------------------------
            for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3)
                for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4)
                    mesh_list[i].vert_indices.push_back(mesh->mFaces[i3].mIndices[i4]);

            set_buffer_data(i);   // Set up: VAO, VBO and EBO.
        }

        center = (bound_box_max + bound_box_min) / 2.0f;
    }
}

void SceneMesh::load_model_cout_console() {
    // Briefly looking at the node structure
    // ------------------------------------------------
    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        cout << "Assimp importer.ReadFile (Error) -- " << importer.GetErrorString() << "\n";
    else {
        num_meshes = scene->mNumMeshes;
        mesh_list.resize(num_meshes);

        cout << "\n\n   Start of Assimp Loading Meshes & Analysis";
        cout << "\n   -----------------------------------------";

        root_node = scene->mRootNode;

        cout << "\n   node->mNumMeshes: " << root_node->mNumMeshes;
        cout << "\n   node->mName.C_Str(): " << root_node->mName.C_Str();
        cout << "\n\n   node->mNumChildren: " << root_node->mNumChildren;
        // ------------------------------------------------------------------------------------------
        for (unsigned int i = 0; i < root_node->mNumChildren; ++i) {
            cout << "\n   node->mChildren[i]->mName.C_Str(): " << root_node->mChildren[i]->mName.C_Str();
            cout << "\n   node->mChildren[i]->mNumMeshes: " << root_node->mChildren[i]->mNumMeshes;
        }

        aiMesh* mesh{};

        int total_num_indices = 0;

        // (1) Loop through all the model's meshes
        // -----------------------------------------------------
        cout << "\n   scene->mNumMeshes: " << num_meshes;
        cout << "\n   ********************\n";
        // ---------------------------------------------------------
        for (unsigned int i = 0; i < num_meshes; ++i)   // In this case... scene->mNumMeshes = node->mChildren[i]->mNumMeshes
        {
            mesh = scene->mMeshes[i];   // http://assimp.sourceforge.net/lib_html/structai_mesh.html

            cout << "\n   mesh->mName.C_Str(): " << mesh->mName.C_Str();
            cout << "\n   Mesh index: " << i << " (mesh->mNumVertices: " << mesh->mNumVertices << ")";
            cout << "\n   ------------------------------------- ";

            // (2) Loop through all mesh [i]'s vertices
            // ---------------------------------------------------
            for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2) {
                vec3 position(mesh->mVertices[i2].x, mesh->mVertices[i2].y, mesh->mVertices[i2].z);
                mesh_list[i].vert_positions.push_back(position);

                cout << "\n   Count: " << i2;
                cout << "\n   mesh->mVertices[" << i2 << "].x: " << position.x;
                cout << "\n   mesh->mVertices[" << i2 << "].y: " << position.y;
                cout << "\n   mesh->mVertices[" << i2 << "].z: " << position.z;

                if (mesh->HasNormals()) {
                    vec3 normal(mesh->mNormals[i2].x, mesh->mNormals[i2].y, mesh->mNormals[i2].z);
                    mesh_list[i].vert_normals.push_back(normal);
                    cout << "\n   mesh->mNormals[" << i2 << "] X: " << normal.x << " Y: " << normal.y << " Z: " << normal.z;
                } else
                    mesh_list[i].vert_normals.push_back(vec3(0.0f, 0.0f, 0.0f));
            }
            cout << "\n\n   mesh->mNumFaces: " << mesh->mNumFaces << "\n";
            cout << "   ------------------ ";

            // (3) Loop through all mesh [i]'s Indices
            // --------------------------------------------------
            for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3) {
                cout << "\n";
                for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4) {
                    cout << "   mesh->mFaces[" << i3 << "].mIndices[" << i4 << "]: " << mesh->mFaces[i3].mIndices[i4] << "\n";
                    mesh_list[i].vert_indices.push_back(mesh->mFaces[i3].mIndices[i4]);
                    ++total_num_indices;
                }
            }
            cout << "\n   Total number of indices: " << total_num_indices;
            cout << "\n   **************************";
            total_num_indices = 0;
            cout << "\n   *****************************************************\n\n";

            set_buffer_data(i);   // Set up: VAO, VBO and EBO.
        }
    }
}

void SceneMesh::update_scene_bound_box(aiAABB bound_box) {
    bound_box_max.x = std::max(bound_box_max.x, (float)bound_box.mMax.x);
    bound_box_max.y = std::max(bound_box_max.y, (float)bound_box.mMax.y);
    bound_box_max.z = std::max(bound_box_max.z, (float)bound_box.mMax.z);

    bound_box_min.x = std::min(bound_box_min.x, (float)bound_box.mMin.x);
    bound_box_min.y = std::min(bound_box_min.y, (float)bound_box.mMin.y);
    bound_box_min.z = std::min(bound_box_min.z, (float)bound_box.mMin.z);
}

void SceneMesh::set_buffer_data(unsigned int index) {
    glGenVertexArrays(1, &mesh_list[index].VAO);
    glGenBuffers(1, &mesh_list[index].VBO1);   // Alternative to using 3 separate VBOs, instead use only 1 VBO and set glVertexAttribPointer's offset...
    glGenBuffers(1, &mesh_list[index].VBO2);   // like was done in tutorial 3... Orbiting spinning cubes.
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
    position += translation;
    translation_mat = glm::translate(mat4(1.0f), position);
    update_transformation();
}

void SceneMesh::rotate_x(float angles) {
    rotation.x += angles;
    mat4 mat = glm::translate(mat4(1.0f), center);
    mat = rotate(mat, radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
    rotation_x_mat = glm::translate(mat, -center);
    update_transformation();
}

void SceneMesh::rotate_y(float angles) {
    rotation.y += angles;
    mat4 mat = glm::translate(mat4(1.0f), center);
    mat = rotate(mat, radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
    rotation_y_mat = glm::translate(mat, -center);
    update_transformation();
}

void SceneMesh::rotate_z(float angles) {
    rotation.z += angles;
    mat4 mat = glm::translate(mat4(1.0f), center);
    mat = rotate(mat, radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
    rotation_z_mat = glm::translate(mat, -center);
    update_transformation();
}

void SceneMesh::scale(glm::vec3 scale_increment) {
    _scale += scale_increment;
    mat4 mat = glm::translate(mat4(1.0f), center);
    mat = glm::scale(mat, _scale);
    scale_mat = glm::translate(mat, -center);
    update_transformation();
}

void SceneMesh::update_transformation() {
    transformation = translation_mat * rotation_z_mat * rotation_y_mat * rotation_x_mat * scale_mat;
}

unsigned int SceneMesh::getNumMeshes() const { return num_meshes; }
std::vector<Mesh> SceneMesh::getMeshList() const { return mesh_list; }
glm::vec3 SceneMesh::getCenter() const { return center; }
glm::vec3 SceneMesh::getBoundBoxMax() const { return bound_box_max; }
glm::vec3 SceneMesh::getBoundBoxMin() const { return bound_box_min; }
glm::mat4 SceneMesh::getTransformation() const { return transformation; }
