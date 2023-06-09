#pragma once

class Shader {
   public:

    Shader(){};

    void loadAndCreateShader(const char* vtx_filename, const char* frag_filename);

    void use();

    // Getters
    int getId() const;

   private:
    int id;
    
    /**
     * Create program.
     *
     * Creates a program from given shader codes.
     *
     * @param vertex_code String with code for vertex shader.
     * @param fragment_code String with code for fragment shader.
     * @return Compiled program.
     */
    int createShaderProgram(const char*, const char*);

    /**
     * Read file.
     *
     * Read whole content of a file.
     *
     * @param filename String with file name to be read.
     * @return File content.
     */
    const char* readFile(const char* filename);
};
