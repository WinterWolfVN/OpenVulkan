#pragma once
#include <cstdint>
#include <vector>
#include <functional>

struct VkViewport {
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
};

struct VkExtent2D {
    int32_t width;
    int32_t height;
};

struct VkOffset2D {
    int32_t x;
    int32_t y;
};

struct VkRect2D {
    VkOffset2D offset;
    VkExtent2D extent;
};

struct VkFramebuffer_T {
    GLuint fbo;
    int32_t width;
    int32_t height;
};
typedef struct VkFramebuffer_T* VkFramebuffer;

union VkClearColorValue {
    float float32[4];
    int32_t int32[4];
};

struct VkClearDepthStencilValue {
    float depth;
    int32_t stencil;
};

union VkClearValue {
    union VkClearColorValue color;
    struct VkClearDepthStencilValue depthStencil;
};

struct VkRenderPassBeginInfo {
    int32_t sType;
    const void* pNext;
    VkRenderPass renderPass;
    VkFramebuffer framebuffer;
    struct VkRect2D renderArea;
    int32_t clearValueCount;
    const union VkClearValue* pClearValues;
};

struct VkOffset3D {
    int32_t x;
    int32_t y;
    int32_t z;
};

struct VkImageSubresourceLayers {
    int32_t aspectMask;
    int32_t mipLevel;
    int32_t baseArrayLayer;
    int32_t layerCount;
};

struct VkBufferImageCopy {
    int64_t bufferOffset;
    int32_t bufferRowLength;
    int32_t bufferImageHeight;
    struct VkImageSubresourceLayers imageSubresource;
    struct VkOffset3D imageOffset;
    struct VkExtent3D imageExtent;
};

struct VkImageBlit {
    struct VkImageSubresourceLayers srcSubresource;
    struct VkOffset3D srcOffsets[2];
    struct VkImageSubresourceLayers dstSubresource;
    struct VkOffset3D dstOffsets[2];
};

struct VkRenderingAttachmentInfo {
    const void* pNext;
    VkImageView imageView;
    VkImageView resolveImageView;
    int32_t sType;
    int32_t imageLayout;
    int32_t resolveMode;
    int32_t resolveImageLayout;
    int32_t loadOp;
    int32_t storeOp;
    float clearValue[4];
};

struct VkRenderingInfo {
    const void* pNext;
    const VkRenderingAttachmentInfo* pColorAttachments;
    const VkRenderingAttachmentInfo* pDepthAttachment;
    const VkRenderingAttachmentInfo* pStencilAttachment;
    int32_t sType;
    int32_t flags;
    int32_t renderAreaX;
    int32_t renderAreaY;
    int32_t renderAreaWidth;
    int32_t renderAreaHeight;
    int32_t layerCount;
    int32_t viewMask;
    int32_t colorAttachmentCount;
};
