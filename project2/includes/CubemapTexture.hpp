#pragma once

#include <iostream>

class CubemapTexture {
   public:
    CubemapTexture(const std::string filename);

    void load(bool flat);

    void use();

    // Getters
    int getId() const;

    int width;
    int height;
    int face_side;
    int n_channels;

   private:
    std::string filename;

    unsigned int id;

    unsigned int color_format;

    void loadFlat();
    unsigned char* loadFace();

    void loadCube();
    unsigned char** loadFaces();
    void copyToBuffer(unsigned char* data, unsigned char* buf, int y_px, int x_px);

    void updateColorFormat();
};
