//
// Created by fgsqme on 2022/10/3.
//

#ifndef NATIVESURFACE_IMAGETEXTURE_H
#define NATIVESURFACE_IMAGETEXTURE_H


#include <EGL/egl.h>
#include <dlfcn.h>
#include <GLES3/gl32.h>
#include <string>
#include <imgui.h>

using namespace std;

class ImageTexture {
private:
    GLuint my_opengl_texture;
public:
    ~ImageTexture();

    ImageTexture();

    ImageTexture(uint8_t *buffer, int width, int height);

    void *getOpenglTexture() const;

    void setBuffer(uint8_t *buffer, int width, int height);
};

#endif //NATIVESURFACE_IMAGETEXTURE_H
