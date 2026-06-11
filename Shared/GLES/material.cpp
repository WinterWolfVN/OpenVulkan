#include "material.h"

static inline void MaterialInit(Material& m, GLuint program)
{
    m.program = program;
    m.texture = 0;
    m.hasTexture = false;

    m.uTexLoc = glGetUniformLocation(program, "uTex");
}

static inline void MaterialSetTexture(Material& m, GLuint tex)
{
    m.texture = tex;
    m.hasTexture = true;
}

static inline void MaterialUse(const Material& m)
{
    glUseProgram(m.program);

    if (m.hasTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m.texture);

        if (m.uTexLoc >= 0)
            glUniform1i(m.uTexLoc, 0);
    }
}
