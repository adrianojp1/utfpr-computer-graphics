/**
 * @file utils.cpp
 * Util functions.
 *
 * Implements frequently used functions.
 *
 * @author Ricardo Dutra da Silva
 */

#include "utils.h"
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

/**
 * Create program.
 *
 * Creates a program from given shader codes.
 *
 * @param vertex_code String with code for vertex shader.
 * @param fragment_code String with code for fragment shader.
 * @return Compiled program.
 */
int createShaderProgram(const char* vertex_code, const char* fragment_code) {
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

/**
 * Read file.
 *
 * Read whole content of a file.
 *
 * @param filename String with file name to be read.
 * @return File content.
 */
const char* readFile(const char* filename) {
    FILE* inputFile;
    if ((inputFile = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Error - Unable to open %s\n", filename);
        exit(-1);
    }

    fseek(inputFile, 0, SEEK_END);
    long length = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    char* buffer = (char*)malloc(length + 1);
    buffer[length] = '\0';
    fread(buffer, 1, length, inputFile);
    fclose(inputFile);

    printf("Read file %s\n", filename);

    return buffer;
}

void printVector(vector<vec2> v) {
    size_t size = v.size();
    cout << "{ ";
    for (int i = 0; i < size - 1; i++) {
        cout << to_string(v[i]) << ", ";
    }
    cout << to_string(v[size - 1]) << " }" << endl;
}

void printVector(vector<float> v) {
    size_t size = v.size();
    cout << "{ ";
    for (int i = 0; i < size - 1; i++) {
        cout << v[i] << ", ";
    }
    cout << v[size - 1] << " }" << endl;
}

void printArray(float* arr, int size) {
    cout << "{ ";
    for (int i = 0; i < size - 1; i++) {
        cout << arr[i] << ", ";
    }
    cout << arr[size - 1] << " }" << endl;
}

void toGlCoords(float win_x, float win_y, float* gl_x, float* gl_y, int win_width, int win_height) {
    *gl_x = ((float)win_x / win_width - 0.5f) * 2;
    *gl_y = ((float)win_y / win_height - 0.5f) * -2;
}

void copyRepeatedTo(float* elem, int stride, float* out, int first, int count) {
    for (int i = first; i < first + count; i++) {
        int offset = i * stride;
        for (int j = 0; j < stride; j++) {
            out[offset + j] = elem[j];
        }
    }
}
