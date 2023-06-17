#include "CubemapTexture.hpp"
#include <GL/glew.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

#define RIGHT_FACE 0
#define LEFT_FACE 1
#define TOP_FACE 2
#define BOTTOM_FACE 3
#define FRONT_FACE 4
#define BACK_FACE 5

CubemapTexture::CubemapTexture(const char* filename) {
    this->filename = filename;
}

// Based on https://learnopengl.com/Advanced-OpenGL/Cubemaps
void CubemapTexture::load() {
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    this->use();

    unsigned char** textures_faces = loadFaces();
    if (textures_faces) {
        for (int i = 0; i < 6; i++) {
            unsigned char* face_data = textures_faces[i];
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, face_side, face_side, 0, GL_RGBA, GL_UNSIGNED_BYTE, face_data);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        cout << "Cubemap texture loaded: " << filename << endl;

    } else {
        cerr << "Failed to load cubemap texture: " << filename << endl;
    }

    for (int i = 0; i < 6; i++) {
        free(textures_faces[i]);
    }
    free(textures_faces);
}

void CubemapTexture::use() {
    glBindTexture(GL_TEXTURE_2D, id);
}

int CubemapTexture::getId() const {
    return id;
}

unsigned char** CubemapTexture::loadFaces() {
    int desired_channels = 4;
    unsigned char* im_data = stbi_load(filename, &width, &height, &n_channels, desired_channels);

    if (!im_data) {
        return NULL;
    }

    if (n_channels != desired_channels) {
        cerr << "Texture file " << filename << " does not have RGBA channels" << endl;
        return NULL;
    }

    int face_width = width / 4;
    int face_height = height / 3;
    if (face_width == face_height && width % 4 == 0 && height % 3 == 0) {
        face_side = face_width;
    } else {
        cerr << "Texture file " << filename << " size does not match with cubemap" << endl;
        return NULL;
    }

    unsigned char** faces_buffer = (unsigned char**)malloc(6 * sizeof(unsigned char*));
    for (int i = 0; i < 6; i++) {
        faces_buffer[i] = (unsigned char*)malloc(face_side * face_side * n_channels * sizeof(unsigned char));
    }

    // Right face
    int y_pixel = face_side;
    int x_pixel = 2 * face_side;
    copyToBuffer(im_data, faces_buffer[RIGHT_FACE], y_pixel, x_pixel);

    // Left face
    y_pixel = face_side;
    x_pixel = 0;
    copyToBuffer(im_data, faces_buffer[LEFT_FACE], y_pixel, x_pixel);

    // Top face
    y_pixel = 0;
    x_pixel = face_side;
    copyToBuffer(im_data, faces_buffer[TOP_FACE], y_pixel, x_pixel);

    // Bottom face
    y_pixel = 2 * face_side;
    x_pixel = face_side;
    copyToBuffer(im_data, faces_buffer[BOTTOM_FACE], y_pixel, x_pixel);

    // Front face
    y_pixel = face_side;
    x_pixel = face_side;
    copyToBuffer(im_data, faces_buffer[FRONT_FACE], y_pixel, x_pixel);

    // Back face
    y_pixel = face_side;
    x_pixel = 3 * face_side;
    copyToBuffer(im_data, faces_buffer[BACK_FACE], y_pixel, x_pixel);

    stbi_image_free(im_data);
    return faces_buffer;
}

void CubemapTexture::copyToBuffer(unsigned char* data, unsigned char* buf, int y_px, int x_px) {
    int buff_i = 0;

    for (int y = y_px; y < y_px + face_side; y++) {
        int y_offset = y * width * n_channels;

        for (int x = x_px; x < x_px + face_side; x++) {
            int x_offset = x * n_channels;

            for (int c = 0; c < n_channels; c++) {
                buf[buff_i++] = data[y_offset + x_offset + c];
            }
        }
    }
}
