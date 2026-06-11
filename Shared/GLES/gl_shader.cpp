#include <GLES3/gl31.h>

GLuint CompileShader(
    GLenum type,
    const char* source)
{
    GLuint shader = glCreateShader(type);

    glShaderSource(
        shader,
        1,
        &source,
        nullptr);

    glCompileShader(shader);

    GLint success = 0;

    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &success);

    if (!success)
    {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint CreateProgram(
    const char* vertexSource,
    const char* fragmentSource)
{
    GLuint vs =
        CompileShader(
            GL_VERTEX_SHADER,
            vertexSource);

    GLuint fs =
        CompileShader(
            GL_FRAGMENT_SHADER,
            fragmentSource);

    if (!vs || !fs)
        return 0;

    GLuint program =
        glCreateProgram();

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint success = 0;

    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &success);

    if (!success)
    {
        glDeleteProgram(program);
        return 0;
    }

    return program;
}
