#pragma once

#include <iostream>

class CubemapTexture {
   public:
    CubemapTexture(const std::string text_file, const std::string normal_map_file, bool is_flat);

    void load();
    void use();
    bool hasNormalMap();

   private:
    class Texture {
       public:
        int face_width;
        int face_height;
        int n_channels;

        unsigned int color_format;
        unsigned int id;

        std::string filename;
    };

    Texture* diffuse_map;
    Texture* normal_map;

    bool is_flat;

    static bool loadFlat(Texture* texture);
    static unsigned char* loadFace(Texture* texture);

    static bool loadCube(Texture* texture);
    static unsigned char** loadFaces(Texture* texture);
    static void copyFaceToBuffer(unsigned char* data, int im_w, int f_h, int f_w, int n_c, int f_y, int f_x, unsigned char* buf);

    static void setTexParameters();
    static void updateColorFormat(Texture* texture);
};
