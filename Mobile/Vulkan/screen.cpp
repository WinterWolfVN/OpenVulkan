#include <EGL/egl.h>
#include <GLES3/gl31.h>

typedef void* VkCommandBuffer;
typedef void* VkQueue;
typedef uint32_t VkResult;

#define VK_SUCCESS 0

extern EGLDisplay global_display;

struct ClearColor { float r, g, b, a; };
struct RenderPassBegin {
    void* renderPass;
    void* framebuffer;
    uint32_t renderArea[4];
    uint32_t clearValueCount;
    ClearColor* pClearValues;
};

extern "C" {

void vkCmdBeginRenderPass(VkCommandBuffer commandBuffer, const RenderPassBegin* pBegin, uint32_t contents) {
    if (pBegin && pBegin->clearValueCount > 0) {
        glClearColor(pBegin->pClearValues[0].r, pBegin->pClearValues[0].g, pBegin->pClearValues[0].b, pBegin->pClearValues[0].a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}

void vkCmdEndRenderPass(VkCommandBuffer commandBuffer) {}

void vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
}

VkResult vkQueuePresentKHR(VkQueue queue, const void* pPresentInfo) {
    eglSwapBuffers(global_display, eglGetCurrentSurface(EGL_DRAW));
    return VK_SUCCESS;
}

VkResult vkAcquireNextImageKHR(void* device, void* swapchain, uint64_t timeout, void* sem, void* fence, uint32_t* pIndex) {
    if (pIndex) *pIndex = 0;
    return VK_SUCCESS;
}

}
