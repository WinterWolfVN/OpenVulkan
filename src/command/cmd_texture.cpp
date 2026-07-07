#include "../struct/stcmd.h"
#include <GLES3/gl31.h>
#include <cstdint>
#include <vector>

extern "C" {

static void GetGLFormatAndType(int32_t vkFmt, GLenum& fmt, GLenum& typ) {
    switch (vkFmt) {
        case 9: fmt=GL_RED; typ=GL_UNSIGNED_BYTE; break;
        case 16: fmt=GL_RG; typ=GL_UNSIGNED_BYTE; break;
        case 23: fmt=GL_RGB; typ=GL_UNSIGNED_BYTE; break;
        case 37: case 43: case 44: case 50: fmt=GL_RGBA; typ=GL_UNSIGNED_BYTE; break;
        case 124: fmt=GL_DEPTH_COMPONENT; typ=GL_UNSIGNED_SHORT; break;
        case 126: fmt=GL_DEPTH_COMPONENT; typ=GL_FLOAT; break;
        case 129: fmt=GL_DEPTH_STENCIL; typ=GL_UNSIGNED_INT_24_8; break;
        default: fmt=GL_RGBA; typ=GL_UNSIGNED_BYTE; break;
    }
}

static int32_t GetBPP(int32_t f) {
    switch(f){ case 9:return 1; case 16:return 2; case 23:return 3; case 124:return 2; default:return 4; }
}

static GLbitfield GetGLBlitMask(int32_t f) {
    if (f==124||f==126) return GL_DEPTH_BUFFER_BIT;
    if (f==129) return GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT;
    return GL_COLOR_BUFFER_BIT;
}

void vkCmdPipelineBarrier(VkCommandBuffer cmd, int32_t sm, int32_t dm, int32_t flags, int32_t mc, const void* pmb, int32_t bmc, const VkBufferMemoryBarrier* pbmb, int32_t imc, const VkImageMemoryBarrier* pimb) {
    if (!cmd) return;
    uint32_t b = 0;
    
    if (imc > 0 && pimb) {
        for (int32_t i=0; i<imc; ++i) {
            int32_t a = pimb[i].dstAccessMask;
            if (a & 0x00020) b |= GL_TEXTURE_FETCH_BARRIER_BIT;
            if (a & 0x00040) b |= GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;
            if (a & 0x03000) b |= GL_PIXEL_BUFFER_BARRIER_BIT;
            if (a & 0x00300) b |= GL_FRAMEBUFFER_BARRIER_BIT;
            if (pimb[i].image) pimb[i].image->layout = pimb[i].newLayout;
        }
    }

    if (bmc > 0 && pbmb) {
        for (int32_t i=0; i<bmc; ++i) {
            int32_t a = pbmb[i].dstAccessMask;
            if (a & 0x00001) b |= GL_COMMAND_BARRIER_BIT;
            if (a & 0x00002) b |= GL_ELEMENT_ARRAY_BARRIER_BIT;
            if (a & 0x00004) b |= GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT;
            if (a & 0x00008) b |= GL_UNIFORM_BARRIER_BIT;
            if (a & 0x00060) b |= GL_SHADER_STORAGE_BARRIER_BIT;
            if (a & 0x03000) b |= GL_BUFFER_UPDATE_BARRIER_BIT;
        }
    }
    
    if (!b && (imc>0 || bmc>0)) b = GL_ALL_BARRIER_BITS; 
    if (b) cmd->commands.push_back([b](){ glMemoryBarrier(b); });
}

void vkCmdCopyBufferToImage(VkCommandBuffer cmd, VkBuffer sBuf, VkImage dImg, int32_t dLayout, int32_t count, const VkBufferImageCopy* pReg) {
    if (!cmd || !sBuf || !dImg || !pReg) return;
    dImg->layout = dLayout;
    std::vector<VkBufferImageCopy> reg(pReg, pReg+count);
    int32_t buf = sBuf->buffer, tex = dImg->texture, type = dImg->imageType, fmt = dImg->format;
    
    cmd->commands.push_back([buf, tex, type, fmt, reg]() {
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

void vkCmdCopyImageToBuffer(VkCommandBuffer cmd, VkImage sImg, int32_t sLayout, VkBuffer dBuf, int32_t count, const VkBufferImageCopy* pReg) {
    if (!cmd || !sImg || !dBuf || !pReg) return;
    sImg->layout = sLayout;
    std::vector<VkBufferImageCopy> reg(pReg, pReg+count);
    int32_t buf = dBuf->buffer, tex = sImg->texture, type = sImg->imageType, fmt = sImg->format;
    
    cmd->commands.push_back([buf, tex, type, fmt, reg]() {
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

void vkCmdBlitImage(VkCommandBuffer cmd, VkImage sImg, int32_t sLayout, VkImage dImg, int32_t dLayout, int32_t count, const VkImageBlit* pReg, int32_t filter) {
    if (!cmd || !sImg || !dImg || !pReg) return;
    sImg->layout = sLayout; dImg->layout = dLayout;
    std::vector<VkImageBlit> reg(pReg, pReg+count);
    int32_t sTex=sImg->texture, sTyp=sImg->imageType, dTex=dImg->texture, dTyp=dImg->imageType, fmt=sImg->format;
    
    cmd->commands.push_back([sTex, sTyp, dTex, dTyp, fmt, reg, filter]() {
        GLuint fbo[2]; glGenFramebuffers(2, fbo);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo[0]); glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo[1]);
        GLbitfield msk = GetGLBlitMask(fmt);
        GLenum fT = (filter==0)?GL_NEAREST:GL_LINEAR;
        GLenum att = (msk&GL_COLOR_BUFFER_BIT)?GL_COLOR_ATTACHMENT0:((msk&GL_STENCIL_BUFFER_BIT)?GL_DEPTH_STENCIL_ATTACHMENT:GL_DEPTH_ATTACHMENT);

        for (const auto& r : reg) {
            int32_t lc = (sTyp==2)?(r.srcOffsets[1].z-r.srcOffsets[0].z):r.srcSubresource.layerCount; if(lc<=0) lc=1;
            int32_t sbl = (sTyp==2)?r.srcOffsets[0].z:r.srcSubresource.baseArrayLayer;
            int32_t dbl = (dTyp==2)?r.dstOffsets[0].z:r.dstSubresource.baseArrayLayer;

            for (int32_t i=0; i<lc; ++i) {
                if (sTyp==2||sTyp==1) glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, att, sTex, r.srcSubresource.mipLevel, sbl+i);
                else glFramebufferTexture2D(GL_READ_FRAMEBUFFER, att, GL_TEXTURE_2D, sTex, r.srcSubresource.mipLevel);
                if (dTyp==2||dTyp==1) glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, att, dTex, r.dstSubresource.mipLevel, dbl+i);
                else glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, att, GL_TEXTURE_2D, dTex, r.dstSubresource.mipLevel);
                
                glBlitFramebuffer(r.srcOffsets[0].x, r.srcOffsets[0].y, r.srcOffsets[1].x, r.srcOffsets[1].y,
                                  r.dstOffsets[0].x, r.dstOffsets[0].y, r.dstOffsets[1].x, r.dstOffsets[1].y, msk, fT);
            }
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDeleteFramebuffers(2, fbo);
    });
}

}   
