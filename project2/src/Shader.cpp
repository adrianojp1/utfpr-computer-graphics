#include "Shader.hpp"

#include <GL/glew.h>

#include <iostream>

using namespace std;

Shader::Shader(const char* vtx_filename, const char* frag_filename) {
    this->vtx_filename = vtx_filename;
    this->frag_filename = frag_filename;
}

void Shader::load() {
    const char* vertex_code = readFile(vtx_filename);
    const char* fragment_code = readFile(frag_filename);

    // Request a program and shader slots from GPU
    id = createShaderProgram(vertex_code, fragment_code);
    cout << "Shader " << id << " loaded with files: " << vtx_filename << ", " << frag_filename << endl;
}

void Shader::use() {
    glUseProgram(id);
}

int Shader::getId() const {
    return id;
}

const char* Shader::readFile(const char* filename) {
    FILE* inputFile;
    if ((inputFile = fopen(filename, "r")) == NULL) {
        cerr << "Error - Unable to open " << filename << endl;
        exit(-1);
    }

    fseek(inputFile, 0, SEEK_END);
    long length = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    char* buffer = (char*)malloc(length + 1);
    buffer[length] = '\0';
    fread(buffer, 1, length, inputFile);
    fclose(inputFile);

    return buffer;
}

int Shader::createShaderProgram(const char* vertex_code, const char* fragment_code) {
    int success;
    char error[512];

    // Request a program and shader slots from GPU
    int program = glCreateProgram();
    int vertex = glCreateShader(GL_VERTEX_SHADER);
    int fragment = glCreateShader(GL_FRAGMENT_SHADER);

    // Set shaders source
    glShaderSource(vertex, 1, &vertex_code, NULL);
    glShaderSource(fragment, 1, &fragment_code, NULL);

    // Compile shaders
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, error);
        cout << "ERROR: Shader comilation error: " << error << endl;
    }

    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, error);
        cout << "ERROR: Shader comilation error: " << error << endl;
    }

    // Attach shader objects to the program
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    // Build program
    glLinkProgram(program);
    glGetShaderiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, error);
        cout << "ERROR: Program link error: " << error << endl;
    }

    // Get rid of shaders (not needed anymore)
    glDetachShader(program, vertex);
    glDetachShader(program, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}
