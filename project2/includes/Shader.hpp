#pragma once

class Shader {
   public:
    Shader(const char* vtx_filename, const char* frag_filename);

    void load();

    void use();

    // Getters
    int getId() const;

   private:
    int id;

    const char* vtx_filename;
    const char* frag_filename;

    const char* readFile(const char* filename);
    int createShaderProgram(const char*, const char*);
};
