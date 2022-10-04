//
// Created by fgsqme on 2022/10/3.
//

#include "ImageTexture.h"


ImageTexture::~ImageTexture() {
    glBindTexture(GL_TEXTURE_2D, 0);  // unbind texture
    glDeleteTextures(1, &my_opengl_texture);
}
ImageTexture::ImageTexture(uint8_t *buffer, int width, int height) {
    setBuffer(buffer, width, height);
};

void ImageTexture::setBuffer(uint8_t *buffer, int width, int height) {
    glGenTextures(1, &my_opengl_texture);
    glBindTexture(GL_TEXTURE_2D, my_opengl_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, buffer);
}

void *ImageTexture::getOpenglTexture() const {
    return (void *) (intptr_t) my_opengl_texture;
}

