#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <assimp/scene.h>

using namespace std;
using namespace glm;

// Based on: https://en.wikipedia.org/wiki/Cube_mapping
glm::vec2 toCubeUV(vec3 pos, vec3 normal) {
    vec3 cube = normalize(pos);
    vec3 abs_c = abs(cube);
    vec3 abs_n = abs(normal);

    float max_axis, uc, vc;

    // X axis face
    if ((abs_c.x > abs_c.y && abs_c.x > abs_c.z) ||
        (abs_c.x > abs_c.y && abs_c.x == abs_c.z && abs_n.x >= abs_n.z) ||
        (abs_c.x == abs_c.y && abs_n.x >= abs_n.y && abs_c.x > abs_c.z)) {
        if (cube.x > 0) {
            // u (0 to 1) goes from +z to -z
            // v (0 to 1) goes from -y to +y
            max_axis = abs_c.x;
            uc = -cube.z;
            vc = cube.y;
        } else {
            // u (0 to 1) goes from -z to +z
            // v (0 to 1) goes from -y to +y
            max_axis = abs_c.x;
            uc = cube.z;
            vc = cube.y;
        }
    }
    // Y axis face
    else if ((abs_c.y > abs_c.x && abs_c.y > abs_c.z) ||
        (abs_c.y > abs_c.x && abs_c.y == abs_c.z && abs_n.y >= abs_n.z) ||
        (abs_c.y == abs_c.z && abs_n.y >= abs_n.z && abs_c.y > abs_c.z)) {
        if (cube.y > 0) {
            // u (0 to 1) goes from -x to +x
            // v (0 to 1) goes from +z to -z
            max_axis = abs_c.y;
            uc = cube.x;
            vc = -cube.z;
        } else {
            // u (0 to 1) goes from -x to +x
            // v (0 to 1) goes from -z to +z
            max_axis = abs_c.y;
            uc = cube.x;
            vc = cube.z;
        }
    }
    // Z axis face
    else if ((abs_c.z > abs_c.x && abs_c.z > abs_c.y) ||
        (abs_c.z > abs_c.x && abs_c.z == abs_c.y && abs_n.y >= abs_n.y) ||
        (abs_c.z == abs_c.x && abs_n.z >= abs_n.x && abs_c.z > abs_c.y)) {
        if (cube.z > 0) {
            // u (0 to 1) goes from -x to +x
            // v (0 to 1) goes from -y to +y
            max_axis = abs_c.z;
            uc = cube.x;
            vc = cube.y;
        } else {
            // u (0 to 1) goes from +x to -x
            // v (0 to 1) goes from -y to +y
            max_axis = abs_c.z;
            uc = -cube.x;
            vc = cube.y;
        }
    }

    // If 3 axis corner, resolve by normal
    if (abs_c.x == abs_c.y && abs_c.x == abs_c.z) {
        if (abs_n.x >= abs_n.y && abs_n.x >= abs_n.z) {
            if (normal.x > 0) {
                // u (0 to 1) goes from +z to -z
                // v (0 to 1) goes from -y to +y
                max_axis = abs_c.x;
                uc = -cube.z;
                vc = cube.y;
            } else {
                // u (0 to 1) goes from -z to +z
                // v (0 to 1) goes from -y to +y
                max_axis = abs_c.x;
                uc = cube.z;
                vc = cube.y;
            }
        } else if (abs_n.y >= abs_n.x && abs_n.y >= abs_n.z) {
            if (normal.y > 0) {
                // u (0 to 1) goes from -x to +x
                // v (0 to 1) goes from +z to -z
                max_axis = abs_c.y;
                uc = cube.x;
                vc = -cube.z;
            } else {
                // u (0 to 1) goes from -x to +x
                // v (0 to 1) goes from -z to +z
                max_axis = abs_c.y;
                uc = cube.x;
                vc = cube.z;
            }
        } else if (abs_n.z >= abs_n.x && abs_n.z >= abs_n.y) {
            if (normal.z > 0) {
                // u (0 to 1) goes from -x to +x
                // v (0 to 1) goes from -y to +y
                max_axis = abs_c.z;
                uc = cube.x;
                vc = cube.y;
            } else {
                // u (0 to 1) goes from +x to -x
                // v (0 to 1) goes from -y to +y
                max_axis = abs_c.z;
                uc = -cube.x;
                vc = cube.y;
            }
        }
    }

    // Convert range from -1 to 1 to 0 to 1
    float u = 0.5f * (uc / max_axis + 1.0f);
    float v = 0.5f * (vc / max_axis + 1.0f);
    return vec2{ u, v };
}

glm::vec3 toVec3(aiVector3D ai_vec3) {
    return vec3{ (float)ai_vec3.x, (float)ai_vec3.y, (float)ai_vec3.z };
}
