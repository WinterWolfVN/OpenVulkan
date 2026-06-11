#pragma once

#include <GLES3/gl31.h>
#include <cstdint>

struct Mesh
{
    GLuint vao = 0;
    GLuint vbo = 0;

    int vertexCount = 0;
};
