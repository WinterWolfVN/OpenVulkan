#pragma once

#include <GLES3/gl31.h>
#include <cstdint>

namespace vkgles
{

struct Image
{
    GLuint id = 0;

    uint32_t width = 0;
    uint32_t height = 0;

    GLenum format = GL_RGBA;
};

static inline bool CreateImage2D(
    Image& img,
    uint32_t width,
    uint32_t height,
    const void* data)
{
    img.width = width;
    img.height = height;

    glGenTextures(1, &img.id);

    if (!img.id)
        return false;

    glBindTexture(GL_TEXTURE_2D, img.id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

static inline void DestroyImage(Image& img)
{
    if (img.id)
        glDeleteTextures(1, &img.id);

    img.id = 0;
    img.width = 0;
    img.height = 0;
}

}
