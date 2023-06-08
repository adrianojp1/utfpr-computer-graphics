#pragma once

/**
 * @file utils.h
 * Util functions.
 *
 * Defines a set of util frequently used functions.
 *
 * @author Ricardo Dutra da Silva, Adriano Jose Paulichi
 */

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>


/** 
 * Create program.
 *
 * Creates a program from given shader codes.
 *
 * @param vertex_code String with code for vertex shader.
 * @param fragment_code String with code for fragment shader.
 * @return Compiled program.
 */
int createShaderProgram(const char *, const char *);

/** 
 * Read file.
 *
 * Read whole content of a file.
 *
 * @param filename String with file name to be read.
 * @return File content.
 */
const char *readFile(const char *);

void printVector(std::vector<float> v);
void printVector(std::vector<glm::vec2> v);
void printArray(float *arr, int size);

void toGlCoords(float win_x, float win_y, float* gl_x, float* gl_y, int win_width, int win_height);

void copyRepeatedTo(float* elem, int stride, float* out, int first, int count);
