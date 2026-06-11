#include "mesh.h"

static inline void MeshCreate(
    Mesh& m,
    const void* data,
    int size,
    int vertexCount)
{
    m.vertexCount = vertexCount;

    glGenVertexArrays(1, &m.vao);
    glGenBuffers(1, &m.vbo);

    glBindVertexArray(m.vao);

    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * 5,
        (void*)0);

    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * 5,
        (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
