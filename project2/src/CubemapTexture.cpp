#include "CubemapTexture.hpp"
#include <GL/glew.h>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

#define RIGHT_FACE 0
#define LEFT_FACE 1
#define TOP_FACE 2
#define BOTTOM_FACE 3
#define FRONT_FACE 4
#define BACK_FACE 5

CubemapTexture::CubemapTexture(const string text_file, const string normal_map_file, bool is_flat) {
    this->is_flat = is_flat;
    this->diffuse_map = new Texture();
    this->diffuse_map->filename = text_file;

    ifstream f(normal_map_file.c_str());
    if (f.good()) {
        this->normal_map = new Texture();
        this->normal_map->filename = normal_map_file;
    } else {
        this->normal_map = NULL;
    }
}

void CubemapTexture::load() {
    if (is_flat) {
        if (loadFlat(this->diffuse_map)) {
            cout << "Texture " << this->diffuse_map->id << " - Diffuse flat loaded: " << this->diffuse_map->filename << endl;
        } else {
            cerr << "Texture " << this->diffuse_map->id << " - Failed to load diffuse flat: " << this->diffuse_map->filename << endl;
            exit(-1);
        }
        if (hasNormalMap()) {
            if (loadFlat(this->normal_map)) {
                cout << "Texture " << this->normal_map->id << " - Normal map flat loaded: " << this->normal_map->filename << endl;
            } else {
                cerr << "Texture " << this->normal_map->id << " - Failed to load normal map flat: " << this->diffuse_map->filename << endl;
                exit(-1);
            }
        }
    } else {
        if (loadCube(this->diffuse_map)) {
            cout << "Texture " << this->diffuse_map->id << " - Diffuse cube loaded: " << this->diffuse_map->filename << endl;
        } else {
            cerr << "Texture " << this->diffuse_map->id << " - Failed to load diffuse cube: " << this->diffuse_map->filename << endl;
            exit(-1);
        }
        if (hasNormalMap()) {
            if (loadCube(this->normal_map)) {
                cout << "Texture " << this->normal_map->id << " - Normal map cube loaded: " << this->diffuse_map->filename << endl;
            } else {
                cerr << "Texture " << this->normal_map->id << " - Failed to load normal map cube: " << this->diffuse_map->filename << endl;
                exit(-1);
            }
        }
    }
}

void CubemapTexture::use() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->diffuse_map->id);

    if (hasNormalMap()) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->normal_map->id);
    }
}

bool CubemapTexture::hasNormalMap() {
    return this->normal_map != NULL;
}

bool CubemapTexture::loadFlat(Texture* texture) {
    glGenTextures(1, &texture->id);

    unsigned char* im_data = loadFace(texture);
    if (im_data) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id);

        for (int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, texture->face_width, texture->face_height, 0, texture->color_format, GL_UNSIGNED_BYTE, im_data);
        }
        setTexParameters();

        free(im_data);
        return true;

    } else {
        return false;
    }
}

unsigned char* CubemapTexture::loadFace(Texture* texture) {
    unsigned char* im_data = stbi_load(texture->filename.c_str(), &texture->face_width, &texture->face_height, &texture->n_channels, 0);

    if (!im_data) {
        return NULL;
    }
    updateColorFormat(texture);

    return im_data;
}

bool CubemapTexture::loadCube(Texture* texture) {
    glGenTextures(1, &texture->id);

    unsigned char** textures_faces = loadFaces(texture);
    if (textures_faces) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id);

        for (int i = 0; i < 6; i++) {
            unsigned char* face_data = textures_faces[i];
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, texture->face_width, texture->face_height, 0, texture->color_format, GL_UNSIGNED_BYTE, face_data);
        }
        setTexParameters();

        for (int i = 0; i < 6; i++) {
            free(textures_faces[i]);
        }
        free(textures_faces);
        return true;

    } else {
        return false;
    }
}

unsigned char** CubemapTexture::loadFaces(Texture* texture) {
    int im_width, im_height, n_channels;
    unsigned char* im_data = stbi_load(texture->filename.c_str(), &im_width, &im_height, &n_channels, 0);

    if (!im_data) {
        return NULL;
    }
    texture->n_channels = n_channels;
    updateColorFormat(texture);

    if (im_height % 3 != 0 || im_width % 4 != 0) {
        cerr << "Texture file " << texture->filename << " size does not match with cubemap" << endl;
        return NULL;
    }

    int face_height = im_height / 3;
    int face_width = im_width / 4;
    texture->face_height = face_height;
    texture->face_width = face_width;

    unsigned char** faces_buffer = (unsigned char**)malloc(6 * sizeof(unsigned char*));
    for (int i = 0; i < 6; i++) {
        faces_buffer[i] = (unsigned char*)malloc(face_height * face_width * n_channels * sizeof(unsigned char));
    }

    // Right face
    int face_y = face_height;
    int face_x = 2 * face_width;
    copyFaceToBuffer(im_data, im_width, face_height, face_width, n_channels, face_y, face_x, faces_buffer[RIGHT_FACE]);

    // Left face
    face_y = face_height;
    face_x = 0;
    copyFaceToBuffer(im_data, im_width, face_height, face_width, n_channels, face_y, face_x, faces_buffer[LEFT_FACE]);

    // Top face
    face_y = 0;
    face_x = face_width;
    copyFaceToBuffer(im_data, im_width, face_height, face_width, n_channels, face_y, face_x, faces_buffer[TOP_FACE]);

    // Bottom face
    face_y = 2 * face_height;
    face_x = face_width;
    copyFaceToBuffer(im_data, im_width, face_height, face_width, n_channels, face_y, face_x, faces_buffer[BOTTOM_FACE]);

    // Front face
    face_y = face_height;
    face_x = face_width;
    copyFaceToBuffer(im_data, im_width, face_height, face_width, n_channels, face_y, face_x, faces_buffer[FRONT_FACE]);

    // Back face
    face_y = face_height;
    face_x = 3 * face_width;
    copyFaceToBuffer(im_data, im_width, face_height, face_width, n_channels, face_y, face_x, faces_buffer[BACK_FACE]);

    stbi_image_free(im_data);
    return faces_buffer;
}

void CubemapTexture::copyFaceToBuffer(unsigned char* data, int im_w, int f_h, int f_w, int n_c, int f_y, int f_x, unsigned char* buf) {
    int buff_i = 0;
    for (int y = f_y; y < f_y + f_h; y++) {
        int y_offset = y * im_w * n_c;

        for (int x = f_x; x < f_x + f_w; x++) {
            int x_offset = x * n_c;

            for (int c = 0; c < n_c; c++) {
                buf[buff_i++] = data[y_offset + x_offset + c];
            }
        }
    }
}

void CubemapTexture::setTexParameters() {
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubemapTexture::updateColorFormat(Texture* texture) {
    switch (texture->n_channels) {
        case 1:
            texture->color_format = GL_RED;
            break;
        case 2:
            texture->color_format = GL_RG;
            break;
        case 3:
            texture->color_format = GL_RGB;
            break;
        case 4:
            texture->color_format = GL_RGBA;
            break;
    }
}
