#include "../fpa/struct_core.h"
#include <GLES3/gl31.h>
#include <cstdint>
#include <vector>

extern "C" {

void vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, int32_t srcStageMask, int32_t dstStageMask, int32_t dependencyFlags, int32_t memoryBarrierCount, const void* pMemoryBarriers, int32_t bufferMemoryBarrierCount, const void* pBufferMemoryBarriers, int32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) {
    if (!commandBuffer) return;

    commandBuffer->commands.push_back([]() {
        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_FRAMEBUFFER_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
    });
}

void vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, int32_t pipelineBindPoint, VkPipelineLayout layout, int32_t firstSet, int32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, int32_t dynamicOffsetCount, const int32_t* pDynamicOffsets) {
    if (!commandBuffer || !pDescriptorSets) return;    
    std::vector<VkDescriptorSet_T> sets(descriptorSetCount);
    for (int32_t i = 0; i < descriptorSetCount; ++i) {
        if (pDescriptorSets[i]) sets[i] = *(pDescriptorSets[i]);
    }
    
    commandBuffer->commands.push_back([sets]() {
        for (const auto& set : sets) {
            if (set.descriptorType == 0) {
                glBindBufferRange(GL_UNIFORM_BUFFER,
                                  static_cast<GLuint>(set.binding),
                                  static_cast<GLuint>(set.uniformBuffer),
                                  static_cast<GLintptr>(set.offset),
                                  static_cast<GLsizeiptr>(set.size));
            } else if (set.descriptorType == 1) {
                glActiveTexture(GL_TEXTURE0 + static_cast<GLuint>(set.binding));
                if (set.image) {
                    GLenum target = (set.image->imageType == 2) ? GL_TEXTURE_3D : GL_TEXTURE_2D;
                    glBindTexture(target, static_cast<GLuint>(set.image->texture));
                }
                if (set.sampler) {
                    glBindSampler(static_cast<GLuint>(set.binding), static_cast<GLuint>(set.sampler->sampler));
                }
            }
        }
    });
}

void vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, int32_t dstImageLayout, int32_t regionCount, const VkBufferImageCopy* pRegions) {
    if (!commandBuffer || !srcBuffer || !dstImage || !pRegions) return;    
    std::vector<VkBufferImageCopy> regions(pRegions, pRegions + regionCount);
    int32_t buf = srcBuffer->buffer;
    int32_t tex = dstImage->texture;
    int32_t type = dstImage->imageType;
    
    commandBuffer->commands.push_back([buf, tex, type, regions]() {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, static_cast<GLuint>(buf));
        GLenum target = (type == 2) ? GL_TEXTURE_3D : GL_TEXTURE_2D;
        glBindTexture(target, static_cast<GLuint>(tex));
        
        for (const auto& r : regions) {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, r.bufferRowLength);
            glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, r.bufferImageHeight);
            
            if (target == GL_TEXTURE_2D) {
                glTexSubImage2D(target, r.imageSubresource.mipLevel, 
                                r.imageOffset.x, r.imageOffset.y, 
                                r.imageExtent.width, r.imageExtent.height, 
                                GL_RGBA, GL_UNSIGNED_BYTE, 
                                reinterpret_cast<void*>(static_cast<uintptr_t>(r.bufferOffset)));
            }
        }
        
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
        glBindTexture(target, 0);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    });
}

void vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, int32_t srcImageLayout, VkBuffer dstBuffer, int32_t regionCount, const VkBufferImageCopy* pRegions) {
    if (!commandBuffer || !srcImage || !dstBuffer || !pRegions) return;
    std::vector<VkBufferImageCopy> regions(pRegions, pRegions + regionCount);
    int32_t buf = dstBuffer->buffer;
    int32_t tex = srcImage->texture;
    
    commandBuffer->commands.push_back([buf, tex, regions]() {
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        
        glBindBuffer(GL_PIXEL_PACK_BUFFER, static_cast<GLuint>(buf));
        
        for (const auto& r : regions) {
            glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, static_cast<GLuint>(tex), r.imageSubresource.mipLevel);
            
            glPixelStorei(GL_PACK_ROW_LENGTH, r.bufferRowLength);
            
            glReadPixels(r.imageOffset.x, r.imageOffset.y, 
                         r.imageExtent.width, r.imageExtent.height, 
                         GL_RGBA, GL_UNSIGNED_BYTE, 
                         reinterpret_cast<void*>(static_cast<uintptr_t>(r.bufferOffset)));
        }
        
        glPixelStorei(GL_PACK_ROW_LENGTH, 0);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
    });
}

void vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, int32_t srcImageLayout, VkImage dstImage, int32_t dstImageLayout, int32_t regionCount, const VkImageBlit* pRegions, int32_t filter) {
    if (!commandBuffer || !srcImage || !dstImage || !pRegions) return;
    std::vector<VkImageBlit> regions(pRegions, pRegions + regionCount);
    int32_t sTex = srcImage->texture;
    int32_t dTex = dstImage->texture;
    
    commandBuffer->commands.push_back([sTex, dTex, regions, filter]() {
        GLuint fbos[2];
        glGenFramebuffers(2, fbos);
        
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbos[0]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbos[1]);
        
        GLenum glFilter = (filter == 0) ? GL_NEAREST : GL_LINEAR;
        
        for (const auto& r : regions) {
            glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, static_cast<GLuint>(sTex), r.srcSubresource.mipLevel);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, static_cast<GLuint>(dTex), r.dstSubresource.mipLevel);
            
            glBlitFramebuffer(r.srcOffsets[0].x, r.srcOffsets[0].y, r.srcOffsets[1].x, r.srcOffsets[1].y,
                              r.dstOffsets[0].x, r.dstOffsets[0].y, r.dstOffsets[1].x, r.dstOffsets[1].y,
                              GL_COLOR_BUFFER_BIT, glFilter);
        }
        
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDeleteFramebuffers(2, fbos);
    });
}

}
