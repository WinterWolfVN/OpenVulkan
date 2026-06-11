#pragma once

#include <GLES3/gl31.h>

namespace vkgles
{

struct Sampler
{
    GLuint id = 0;
};

static inline bool CreateSampler(Sampler& smp)
{
    glGenSamplers(1, &smp.id);

    if (!smp.id)
        return false;

    glSamplerParameteri(smp.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(smp.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glSamplerParameteri(smp.id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(smp.id, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return true;
}

static inline void BindSampler(const Sampler& smp, uint32_t slot)
{
    glBindSampler(slot, smp.id);
}

static inline void DestroySampler(Sampler& smp)
{
    if (smp.id)
        glDeleteSamplers(1, &smp.id);

    smp.id = 0;
}

}
