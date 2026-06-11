#pragma once

#include <GLES3/gl31.h>
#include <cstdint>

struct Material
{
    GLuint program = 0;

    GLuint texture = 0;

    GLint uTexLoc = -1;

    bool hasTexture = false;
};
