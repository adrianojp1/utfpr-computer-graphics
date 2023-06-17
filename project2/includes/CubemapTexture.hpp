#pragma once

class CubemapTexture {
   public:
    CubemapTexture(const char* filename);

    void load();

    void use();

    // Getters
    int getId() const;

    int width;
    int height;
    int face_side;
    int n_channels;

    unsigned char** loadFaces();
   private:
    const char* filename;

    unsigned int id;

    void copyToBuffer(unsigned char* data, unsigned char* buf, int y_px, int x_px);
};
