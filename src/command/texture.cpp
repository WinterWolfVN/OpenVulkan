#include "stdraw.h"
#include "../buffer/stbuffer.h"
#include <GLES3/gl31.h>
#include <cstdint>
#include <vector>
#include <utility>

extern "C" {

static void GetGLFormatAndType(int32_t vkFmt, GLenum& fmt, GLenum& typ) {
    switch (vkFmt) {
        case 9: fmt = GL_RED; typ = GL_UNSIGNED_BYTE; break;
        case 16: fmt = GL_RG; typ = GL_UNSIGNED_BYTE; break;
        case 23: fmt = GL_RGB; typ = GL_UNSIGNED_BYTE; break;
        case 37:
        case 43:
        case 44:
        case 50: fmt = GL_RGBA; typ = GL_UNSIGNED_BYTE; break;
        case 124: fmt = GL_DEPTH_COMPONENT; typ = GL_UNSIGNED_SHORT; break;
        case 126: fmt = GL_DEPTH_COMPONENT; typ = GL_FLOAT; break;
        case 129: fmt = GL_DEPTH_STENCIL; typ = GL_UNSIGNED_INT_24_8; break;
        default: fmt = GL_RGBA; typ = GL_UNSIGNED_BYTE; break;
    }
}

static int32_t GetBPP(int32_t vkFmt) {
    switch (vkFmt) {
        case 9: return 1;
        case 16: return 2;
        case 23: return 3;
        case 124: return 2;
        case 126: return 4;
        case 129: return 4;
        default: return 4;
    }
}

static GLbitfield GetGLBlitMask(int32_t vkFmt) {
    switch (vkFmt) {
        case 124:
        case 126: return GL_DEPTH_BUFFER_BIT;
        case 129: return GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
        default: return GL_COLOR_BUFFER_BIT;
    }
}

static GLenum GetGLBlitFilter(int32_t filter) {
    switch (filter) {
        case VK_FILTER_LINEAR: return GL_LINEAR;
        default: return GL_NEAREST;
    }
}

static void AttachBlitImage(GLenum target, GLenum attachment, VkImage image, int32_t mipLevel, int32_t layer) {
    if (image->imageType == VK_IMAGE_TYPE_2D) {
        glFramebufferTexture2D(target, attachment, GL_TEXTURE_2D, image->texture, mipLevel);
    } else {
        glFramebufferTextureLayer(target, attachment, image->texture, mipLevel, layer);
    }
}

void vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer sBuf, VkImage dImg, int32_t dLayout, int32_t count, const VkBufferImageCopy* pRegion) {
    if (!commandBuffer || !sBuf || !dImg || !pRegion) return;
    dImg->layout = dLayout;
    std::vector<VkBufferImageCopy> v(pRegion, pRegion+count);
    int32_t buf=sBuf->buffer, tex=dImg->texture, type=dImg->imageType, fmt=dImg->format;
    commandBuffer->commands.push_back([buf, tex, type, fmt, reg = std::move(v)]() {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf);
        GLenum tgt = (type==2)?GL_TEXTURE_3D:(type==1?GL_TEXTURE_2D_ARRAY:GL_TEXTURE_2D);
        glBindTexture(tgt, tex);
        GLenum glF, glT; GetGLFormatAndType(fmt, glF, glT);
        for (const auto& r : reg) {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, r.bufferRowLength);
            glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, r.bufferImageHeight);
            void* p = (void*)(uintptr_t)r.bufferOffset;
            if (tgt == GL_TEXTURE_2D) {
                glTexSubImage2D(tgt, r.imageSubresource.mipLevel, r.imageOffset.x, r.imageOffset.y, r.imageExtent.width, r.imageExtent.height, glF, glT, p);
            } else {
                int32_t zOff = (type==2)?r.imageOffset.z:r.imageSubresource.baseArrayLayer;
                int32_t d = (type==2)?r.imageExtent.depth:r.imageSubresource.layerCount;
                glTexSubImage3D(tgt, r.imageSubresource.mipLevel, r.imageOffset.x, r.imageOffset.y, zOff, r.imageExtent.width, r.imageExtent.height, d, glF, glT, p);
            }
        }
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0); glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
        glBindTexture(tgt, 0); glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    });
}

void vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage sImg, int32_t sLayout, VkBuffer dBuf, int32_t count, const VkBufferImageCopy* pRegion) {
    if (!commandBuffer || !sImg || !dBuf || !pRegion) return;
    sImg->layout = sLayout;
    std::vector<VkBufferImageCopy> v(pRegion, pRegion+count);
    int32_t buf=dBuf->buffer, tex=sImg->texture, type=sImg->imageType, fmt=sImg->format;
    commandBuffer->commands.push_back([buf, tex, type, fmt, reg = std::move(v)]() {
        GLuint fbo; glGenFramebuffers(1, &fbo); glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, buf);
        GLenum glF, glT; GetGLFormatAndType(fmt, glF, glT);
        int32_t bpp = GetBPP(fmt);
        for (const auto& r : reg) {
            int32_t lc = (type==2)?r.imageExtent.depth:r.imageSubresource.layerCount; if(lc<=0) lc=1;
            int32_t bl = (type==2)?r.imageOffset.z:r.imageSubresource.baseArrayLayer;
            int32_t rL = r.bufferRowLength>0?r.bufferRowLength:r.imageExtent.width;
            int32_t iH = r.bufferImageHeight>0?r.bufferImageHeight:r.imageExtent.height;
            glPixelStorei(GL_PACK_ROW_LENGTH, r.bufferRowLength);
            for (int32_t i=0; i<lc; ++i) {
                if (type==2||type==1) glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, r.imageSubresource.mipLevel, bl+i);
                else glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, r.imageSubresource.mipLevel);
                glReadPixels(r.imageOffset.x, r.imageOffset.y, r.imageExtent.width, r.imageExtent.height, glF, glT, (void*)(uintptr_t)(r.bufferOffset + i*(rL*iH*bpp)));
            }
        }
        glPixelStorei(GL_PACK_ROW_LENGTH, 0); glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); glDeleteFramebuffers(1, &fbo);
    });
}

void vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, int32_t srcImageLayout, VkImage dstImage, int32_t dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, int32_t filter) {
    if (!commandBuffer || !srcImage || !dstImage || !pRegions || !regionCount) return;
    std::vector<VkImageBlit> regions(pRegions, pRegions + regionCount);
    commandBuffer->commands.push_back([srcImage, dstImage, regions = std::move(regions), filter]() {
            GLuint fbos[2];
            glGenFramebuffers(2, fbos);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, fbos[0]);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbos[1]);
            GLenum glFilter = GetGLBlitFilter(filter);
            for (const VkImageBlit& r : regions) {
                GLbitfield mask = GetGLBlitMask(srcImage->format);
                GLenum attachment;
                if (mask & GL_COLOR_BUFFER_BIT) {
                    attachment = GL_COLOR_ATTACHMENT0;
                } else if (mask & GL_STENCIL_BUFFER_BIT) {
                    attachment = GL_DEPTH_STENCIL_ATTACHMENT;
                } else {
                    attachment = GL_DEPTH_ATTACHMENT;
                }
                int32_t srcLayers = r.srcSubresource.layerCount;
                int32_t dstLayers = r.dstSubresource.layerCount;
                if (srcLayers <= 0)
                    srcLayers = 1;
                if (dstLayers <= 0)
                    dstLayers = 1;
                bool src3D = srcImage->imageType == VK_IMAGE_TYPE_3D;
                bool dst3D = dstImage->imageType == VK_IMAGE_TYPE_3D;
                if (src3D || dst3D) {
                    int32_t srcZ0 = r.srcOffsets[0].z;
                    int32_t srcZ1 = r.srcOffsets[1].z;
                    int32_t dstZ0 = r.dstOffsets[0].z;
                    int32_t dstZ1 = r.dstOffsets[1].z;
                    int32_t srcDepth = srcZ1 - srcZ0;
                    int32_t dstDepth = dstZ1 - dstZ0;
                    if (srcDepth < 0) srcDepth = -srcDepth;
                    if (dstDepth < 0) dstDepth = -dstDepth;
                    int32_t count = srcDepth < dstDepth ? srcDepth : dstDepth;
                    for (int32_t i = 0; i < count; ++i) {
                        int32_t srcZ = srcZ0 < srcZ1 ? srcZ0 + i : srcZ0 - i;
                        int32_t dstZ = dstZ0 < dstZ1 ? dstZ0 + i : dstZ0 - i;
                        int32_t srcLayer = r.srcSubresource.baseArrayLayer + i;
                        int32_t dstLayer = r.dstSubresource.baseArrayLayer + i;
                        AttachBlitImage(GL_READ_FRAMEBUFFER, attachment, srcImage, r.srcSubresource.mipLevel, src3D ? srcZ : srcLayer);
                        AttachBlitImage(GL_DRAW_FRAMEBUFFER, attachment, dstImage, r.dstSubresource.mipLevel, dst3D ? dstZ : dstLayer);
                        glBlitFramebuffer(r.srcOffsets[0].x, r.srcOffsets[0].y, r.srcOffsets[1].x, r.srcOffsets[1].y, r.dstOffsets[0].x, r.dstOffsets[0].y, r.dstOffsets[1].x, r.dstOffsets[1].y, mask, glFilter);
                    }
                } else {
                    int32_t layers = srcLayers < dstLayers ? srcLayers : dstLayers;
                    for (int32_t i = 0; i < layers; ++i) {
                        AttachBlitImage(GL_READ_FRAMEBUFFER, attachment, srcImage, r.srcSubresource.mipLevel, r.srcSubresource.baseArrayLayer + i);
                        AttachBlitImage(GL_DRAW_FRAMEBUFFER, attachment, dstImage, r.dstSubresource.mipLevel, r.dstSubresource.baseArrayLayer + i);
                        glBlitFramebuffer(r.srcOffsets[0].x, r.srcOffsets[0].y, r.srcOffsets[1].x, r.srcOffsets[1].y, r.dstOffsets[0].x, r.dstOffsets[0].y, r.dstOffsets[1].x, r.dstOffsets[1].y, mask, glFilter);
                    }
                }
            }
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glDeleteFramebuffers(2, fbos);
        }
    );
}

}   
