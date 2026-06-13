#include <GLES3/gl31.h>
#include <stdint.h>
#include <string.h>

typedef struct { uint32_t sType; const void* pNext; uint32_t flags; } VkHeader;

extern "C" {

uint64_t vkCreateGraphicsPipelines(void* device, uint64_t pipelineCache, uint32_t createInfoCount, const void* pCreateInfos, const void* pAllocator, uint64_t* pPipelines) {
    
    GLuint program = glCreateProgram();    
    if (pPipelines) *pPipelines = (uint64_t)program;
    return 0; 
}

void vkCmdDraw(void* commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
}

uint32_t vkCreateBuffer(void* device, const VkHeader* pCreateInfo, const void* pAllocator, uint64_t* pBuffer) {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    if (pBuffer) *pBuffer = (uint64_t)vbo;
    return 0; 
}

}
