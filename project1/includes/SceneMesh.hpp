#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <iostream>

using namespace std;
using namespace glm;

class SceneMesh {

private:
    Assimp::Importer importer; // https://assimp-docs.readthedocs.io/en/v5.1.0/ ... (An older Assimp website: http://assimp.sourceforge.net/lib_html/index.html)
    const aiScene* scene = nullptr;
    aiNode* root_node = nullptr; // Only being used in the: load_model_cout_console() function.

    struct Mesh
    {
        unsigned int VAO, VBO1, VBO2, EBO; // Buffer handles (Typically type: GLuint is used)

        vec3 center{ 0.0f, 0.0f, 0.0f };
        vector<vec3> vert_positions;
        vector<vec3> vert_normals;
        vector<unsigned int> vert_indices;
    };

public:
    unsigned int num_meshes;
    vector<Mesh> mesh_list;
    vec3 center{ 0.0f, 0.0f, 0.0f };

    SceneMesh() {}

    void load(const char* model_path) // Constructor
    {
        // http://assimp.sourceforge.net/lib_html/postprocess_8h.html (See: aiPostProcessSteps) (Flag options)

            // scene = importer.ReadFile(model_path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        scene = importer.ReadFile(model_path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate);
        // scene = importer.ReadFile(model_path, aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);
        // scene = importer.ReadFile(model_path, aiProcess_Triangulate | aiProcess_FlipUVs);
        // scene = importer.ReadFile(model_path, NULL);

        load_model(); // Uncomment only one of these two load model functions.
        // load_model_cout_console();
    }

private:
    void load_model()
    {
        if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
            cout << "Assimp importer.ReadFile (Error) -- " << importer.GetErrorString() << "\n";
        else
        {
            num_meshes = scene->mNumMeshes;
            mesh_list.resize(num_meshes);
            vec3 scene_center_accum(0.0f, 0.0f, 0.0f);

            aiMesh* mesh{};

            // (1) Loop through all the model's meshes
            // -----------------------------------------------------
            for (unsigned int i = 0; i < num_meshes; ++i)
            {
                mesh = scene->mMeshes[i]; // http://assimp.sourceforge.net/lib_html/structai_mesh.html
                vec3 mesh_center_accum(0.0f, 0.0f, 0.0f);
                // (3) Loop through all mesh [i]'s vertices
                // ---------------------------------------------------
                for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2)
                {
                    vec3 position(mesh->mVertices[i2].x, mesh->mVertices[i2].y, mesh->mVertices[i2].z);
                    mesh_list[i].vert_positions.push_back(position);
                    mesh_center_accum += position;

                    if (mesh->HasNormals())
                    {
                        vec3 normal(mesh->mNormals[i2].x, mesh->mNormals[i2].y, mesh->mNormals[i2].z);
                        mesh_list[i].vert_normals.push_back(normal);
                    }
                    else
                        mesh_list[i].vert_normals.push_back(vec3(0.0f, 0.0f, 0.0f));
                }
                mesh_list[i].center = mesh_center_accum / (float)mesh->mNumVertices;

                // (4) Loop through all mesh [i]'s Indices
                // --------------------------------------------------
                for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3)
                    for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4)
                        mesh_list[i].vert_indices.push_back(mesh->mFaces[i3].mIndices[i4]);

                scene_center_accum += mesh_list[i].center;
                set_buffer_data(i); // Set up: VAO, VBO and EBO.
            }
            
            center = scene_center_accum / (float)num_meshes;
        }
    }

    void load_model_cout_console()
    {
        // Briefly looking at the node structure
        // ------------------------------------------------
        if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
            cout << "Assimp importer.ReadFile (Error) -- " << importer.GetErrorString() << "\n";
        else
        {
            num_meshes = scene->mNumMeshes;
            mesh_list.resize(num_meshes);

            cout << "\n\n   Start of Assimp Loading Meshes & Analysis";
            cout << "\n   -----------------------------------------";

            root_node = scene->mRootNode;

            cout << "\n   node->mNumMeshes: " << root_node->mNumMeshes;
            cout << "\n   node->mName.C_Str(): " << root_node->mName.C_Str();
            cout << "\n\n   node->mNumChildren: " << root_node->mNumChildren;
            // ------------------------------------------------------------------------------------------
            for (unsigned int i = 0; i < root_node->mNumChildren; ++i)
            {
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
            for (unsigned int i = 0; i < num_meshes; ++i) // In this case... scene->mNumMeshes = node->mChildren[i]->mNumMeshes
            {
                mesh = scene->mMeshes[i]; // http://assimp.sourceforge.net/lib_html/structai_mesh.html

                cout << "\n   mesh->mName.C_Str(): " << mesh->mName.C_Str();
                cout << "\n   Mesh index: " << i << " (mesh->mNumVertices: " << mesh->mNumVertices << ")";
                cout << "\n   ------------------------------------- ";

                // (3) Loop through all mesh [i]'s vertices
                // ---------------------------------------------------
                for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2)
                {
                    vec3 position(mesh->mVertices[i2].x, mesh->mVertices[i2].y, mesh->mVertices[i2].z);
                    mesh_list[i].vert_positions.push_back(position);

                    cout << "\n   Count: " << i2;
                    cout << "\n   mesh->mVertices[" << i2 << "].x: " << position.x;
                    cout << "\n   mesh->mVertices[" << i2 << "].y: " << position.y;
                    cout << "\n   mesh->mVertices[" << i2 << "].z: " << position.z;

                    if (mesh->HasNormals())
                    {
                        vec3 normal(mesh->mNormals[i2].x, mesh->mNormals[i2].y, mesh->mNormals[i2].z);
                        mesh_list[i].vert_normals.push_back(normal);
                        cout << "\n   mesh->mNormals[" << i2 << "] X: " << normal.x << " Y: " << normal.y << " Z: " << normal.z;
                    }
                    else
                        mesh_list[i].vert_normals.push_back(vec3(0.0f, 0.0f, 0.0f));
                }
                cout << "\n\n   mesh->mNumFaces: " << mesh->mNumFaces << "\n";
                cout << "   ------------------ ";

                // (4) Loop through all mesh [i]'s Indices
                // --------------------------------------------------
                for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3)
                {
                    cout << "\n";
                    for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4)
                    {
                        cout << "   mesh->mFaces[" << i3 << "].mIndices[" << i4 << "]: " << mesh->mFaces[i3].mIndices[i4] << "\n";
                        mesh_list[i].vert_indices.push_back(mesh->mFaces[i3].mIndices[i4]);
                        ++total_num_indices;
                    }
                }
                cout << "\n   Total number of indices: " << total_num_indices;
                cout << "\n   **************************";
                total_num_indices = 0;
                cout << "\n   *****************************************************\n\n";

                set_buffer_data(i); // Set up: VAO, VBO and EBO.
            }
        }
    }

    void set_buffer_data(unsigned int index)
    {
        glGenVertexArrays(1, &mesh_list[index].VAO);
        glGenBuffers(1, &mesh_list[index].VBO1); // Alternative to using 3 separate VBOs, instead use only 1 VBO and set glVertexAttribPointer's offset...
        glGenBuffers(1, &mesh_list[index].VBO2); // like was done in tutorial 3... Orbiting spinning cubes.
        glGenBuffers(1, &mesh_list[index].EBO);

        glBindVertexArray(mesh_list[index].VAO);

        // Vertex Positions
        // ---------------------
        glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * mesh_list[index].vert_positions.size(), &mesh_list[index].vert_positions[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); // Void pointer below is for legacy reasons. Two possible meanings: "offset for buffer objects" & "address for client state arrays"
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

        glBindVertexArray(0); // Unbind VAO
    }
};