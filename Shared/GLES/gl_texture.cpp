#pragma once

#include <GLES3/gl31.h>
#include <cstdint>

namespace vkgles
{

struct Texture
{
    GLuint id = 0;
    uint32_t width = 0;
    uint32_t height = 0;
};

static inline bool CreateTexture2D(
    Texture& tex,
    uint32_t width,
    uint32_t height,
    const void* data)
{
    tex.width = width;
    tex.height = height;

    glGenTextures(1, &tex.id);

    if (!tex.id)
        return false;

    glBindTexture(GL_TEXTURE_2D, tex.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

static inline void BindTexture(
    const Texture& tex,
    uint32_t slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, tex.id);
}

static inline void DestroyTexture(Texture& tex)
{
    if (tex.id)
        glDeleteTextures(1, &tex.id);

    tex.id = 0;
    tex.width = 0;
    tex.height = 0;
}

}
