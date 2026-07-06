#include "../struct/stbuffer.h"
#include <GLES3/gl31.h>
#include <cstdint>
#include <cstring>

extern "C" {

int32_t vkCreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo, const void* pAllocator, VkImage* pImage) {
    if (!device || !pCreateInfo || !pImage) return -3;

    VkImage img = new VkImage_T();
    img->texture = 0;
    img->imageType = pCreateInfo->imageType;
    img->format = pCreateInfo->format;
    img->width = pCreateInfo->extent.width;
    img->height = pCreateInfo->extent.height;
    img->depth = pCreateInfo->extent.depth;
    img->mipLevels = pCreateInfo->mipLevels;
    img->arrayLayers = pCreateInfo->arrayLayers;

    GLuint gl_tex = 0;
    glGenTextures(1, &gl_tex);
    img->texture = static_cast<int32_t>(gl_tex);

    if (img->texture == 0) {
        delete img;
        return -3;
    }

    GLenum target = GL_TEXTURE_2D;
    if (img->imageType == 2) {
        target = GL_TEXTURE_3D;
    } else if (img->imageType == 1 && img->arrayLayers > 1) {
        target = GL_TEXTURE_2D_ARRAY;
    }

    glBindTexture(target, static_cast<GLuint>(img->texture));

    GLenum internalFormat = GL_RGBA8; 
    switch (pCreateInfo->format) {
        case 9:   internalFormat = GL_R8; break;
        case 16:  internalFormat = GL_RG8; break;
        case 23:  internalFormat = GL_RGB8; break;
        case 37:  internalFormat = GL_RGBA8; break;
        case 43:  internalFormat = GL_SRGB8_ALPHA8; break;
        case 44:  internalFormat = GL_RGBA8; break;
        case 50:  internalFormat = GL_SRGB8_ALPHA8; break;
        case 124: internalFormat = GL_DEPTH_COMPONENT16; break;
        case 126: internalFormat = GL_DEPTH_COMPONENT32F; break;
        case 129: internalFormat = GL_DEPTH24_STENCIL8; break;
        default:  internalFormat = GL_RGBA8; break;
    }

    if (target == GL_TEXTURE_2D) {
        glTexStorage2D(target, img->mipLevels, internalFormat, img->width, img->height);
    } else if (target == GL_TEXTURE_3D) {
        glTexStorage3D(target, img->mipLevels, internalFormat, img->width, img->height, img->depth);
    } else if (target == GL_TEXTURE_2D_ARRAY) {
        glTexStorage3D(target, img->mipLevels, internalFormat, img->width, img->height, img->arrayLayers);
    }

    glBindTexture(target, 0);

    *pImage = img;
    return 0;
}

void vkDestroyImage(VkDevice device, VkImage image, const void* pAllocator) {
    if (image) {
        if (image->texture != 0) {
            GLuint gl_tex = static_cast<GLuint>(image->texture);
            glDeleteTextures(1, &gl_tex);
        }
        delete image;
    }
}

int32_t vkCreateImageView(VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const void* pAllocator, VkImageView* pView) {
    if (!device || !pCreateInfo || !pView) return -3;

    VkImageView view = new VkImageView_T();
    view->image = pCreateInfo->image;
    view->viewType = pCreateInfo->viewType;
    view->format = pCreateInfo->format;
    view->baseMipLevel = pCreateInfo->subresourceRange.baseMipLevel;
    view->levelCount = pCreateInfo->subresourceRange.levelCount;
    view->baseArrayLayer = pCreateInfo->subresourceRange.baseArrayLayer;
    view->layerCount = pCreateInfo->subresourceRange.layerCount;

    *pView = view;
    return 0;
}

void vkDestroyImageView(VkDevice device, VkImageView imageView, const void* pAllocator) {
    if (imageView) {
        delete imageView;
    }
}

int32_t vkCreateSampler(VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const void* pAllocator, VkSampler* pSampler) {
    if (!device || !pCreateInfo || !pSampler) return -3;

    VkSampler sampler = new VkSampler_T();
    sampler->sampler = 0;

    GLuint gl_samp = 0;
    glGenSamplers(1, &gl_samp);
    sampler->sampler = static_cast<int32_t>(gl_samp);

    if (sampler->sampler == 0) {
        delete sampler;
        return -3;
    }

    GLenum magFilter = (pCreateInfo->magFilter == 0) ? GL_NEAREST : GL_LINEAR;
    GLenum minFilter = (pCreateInfo->minFilter == 0) ? GL_NEAREST : GL_LINEAR;
    
    if (pCreateInfo->mipmapMode == 0) {
        minFilter = (pCreateInfo->minFilter == 0) ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_NEAREST;
    } else if (pCreateInfo->mipmapMode == 1) {
        minFilter = (pCreateInfo->minFilter == 0) ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR;
    }

    GLenum wrapU = (pCreateInfo->addressModeU == 0) ? GL_REPEAT : ((pCreateInfo->addressModeU == 1) ? GL_MIRRORED_REPEAT : GL_CLAMP_TO_EDGE);
    GLenum wrapV = (pCreateInfo->addressModeV == 0) ? GL_REPEAT : ((pCreateInfo->addressModeV == 1) ? GL_MIRRORED_REPEAT : GL_CLAMP_TO_EDGE);
    GLenum wrapW = (pCreateInfo->addressModeW == 0) ? GL_REPEAT : ((pCreateInfo->addressModeW == 1) ? GL_MIRRORED_REPEAT : GL_CLAMP_TO_EDGE);

    glSamplerParameteri(static_cast<GLuint>(sampler->sampler), GL_TEXTURE_MAG_FILTER, static_cast<GLint>(magFilter));
    glSamplerParameteri(static_cast<GLuint>(sampler->sampler), GL_TEXTURE_MIN_FILTER, static_cast<GLint>(minFilter));
    glSamplerParameteri(static_cast<GLuint>(sampler->sampler), GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapU));
    glSamplerParameteri(static_cast<GLuint>(sampler->sampler), GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapV));
    glSamplerParameteri(static_cast<GLuint>(sampler->sampler), GL_TEXTURE_WRAP_R, static_cast<GLint>(wrapW));

    glSamplerParameterf(static_cast<GLuint>(sampler->sampler), GL_TEXTURE_MIN_LOD, pCreateInfo->minLod);
    glSamplerParameterf(static_cast<GLuint>(sampler->sampler), GL_TEXTURE_MAX_LOD, pCreateInfo->maxLod);

    if (pCreateInfo->anisotropyEnable != 0) {
        const char* ext = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
        if (ext && std::strstr(ext, "GL_EXT_texture_filter_anisotropic")) {
            #ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
            #define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
            #endif
            glSamplerParameterf(static_cast<GLuint>(sampler->sampler), GL_TEXTURE_MAX_ANISOTROPY_EXT, pCreateInfo->maxAnisotropy);
        }
    }

    if (pCreateInfo->compareEnable != 0) {
        glSamplerParameteri(static_cast<GLuint>(sampler->sampler), GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        
        GLenum cmpOp = GL_LESS;
        switch (pCreateInfo->compareOp) {
            case 0: cmpOp = GL_NEVER; break;
            case 1: cmpOp = GL_LESS; break;
            case 2: cmpOp = GL_EQUAL; break;
            case 3: cmpOp = GL_LEQUAL; break;
            case 4: cmpOp = GL_GREATER; break;
            case 5: cmpOp = GL_NOTEQUAL; break;
            case 6: cmpOp = GL_GEQUAL; break;
            case 7: cmpOp = GL_ALWAYS; break;
            default: cmpOp = GL_LESS; break;
        }
        glSamplerParameteri(static_cast<GLuint>(sampler->sampler), GL_TEXTURE_COMPARE_FUNC, static_cast<GLint>(cmpOp));
    }

    *pSampler = sampler;
    return 0;
}

void vkDestroySampler(VkDevice device, VkSampler sampler, const void* pAllocator) {
    if (sampler) {
        if (sampler->sampler != 0) {
            GLuint gl_samp = static_cast<GLuint>(sampler->sampler);
            glDeleteSamplers(1, &gl_samp);
        }
        delete sampler;
    }
}

}
