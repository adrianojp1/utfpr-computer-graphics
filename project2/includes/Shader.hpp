#pragma once

#include <glm/glm.hpp>
#include <iostream>

class Shader {
   public:
    Shader(const char* vtx_filename, const char* frag_filename);

    void load();

    void use();

    // Getters
    int getId() const;

    void setInt(const std::string& name, int value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setMat4(const std::string& name, const glm::mat4& mat);

   private:
    int id;

    const char* vtx_filename;
    const char* frag_filename;

    const char* readFile(const char* filename);
    int createShaderProgram(const char*, const char*);
};
