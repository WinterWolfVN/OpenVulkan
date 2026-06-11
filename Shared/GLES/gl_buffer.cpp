#include <GLES3/gl31.h>

struct GLVertexBuffer
{
    GLuint vbo = 0;
    GLuint vao = 0;
};

bool CreateVertexBuffer(
    GLVertexBuffer& buffer,
    const void* vertices,
    size_t size)
{
    glGenVertexArrays(
        1,
        &buffer.vao);

    glGenBuffers(
        1,
        &buffer.vbo);

    glBindVertexArray(
        buffer.vao);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        buffer.vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        size,
        vertices,
        GL_STATIC_DRAW);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * 3,
        nullptr);

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    return true;
}

void DestroyVertexBuffer(
    GLVertexBuffer& buffer)
{
    if (buffer.vbo)
        glDeleteBuffers(
            1,
            &buffer.vbo);

    if (buffer.vao)
        glDeleteVertexArrays(
            1,
            &buffer.vao);

    buffer.vbo = 0;
    buffer.vao = 0;
}
