#include "../struct/stshader.h"
#include <GLES3/gl31.h>
#include <cstdint>
#include <vector>

extern "C" {

int32_t vkCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, int32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const void* pAllocator, VkPipeline* pPipelines) {
    if (!device || !pCreateInfos || !pPipelines) return -3;

    for (int32_t i = 0; i < createInfoCount; ++i) {
        const VkGraphicsPipelineCreateInfo& info = pCreateInfos[i];
        VkPipeline pipeline = new VkPipeline_T();
        
        pipeline->bindPoint = 0;
        pipeline->program = glCreateProgram();

        std::vector<GLuint> shaders;
        bool compileFailed = false;

        for (int32_t j = 0; j < info.stageCount; ++j) {
            GLenum glStage = 0;
            if (info.pStages[j].stage == 0x00000001) glStage = GL_VERTEX_SHADER;
            else if (info.pStages[j].stage == 0x00000010) glStage = GL_FRAGMENT_SHADER;
            else continue;

            GLuint shader = glCreateShader(glStage);
            const GLchar* source = static_cast<const GLchar*>(info.pStages[j].module->code);
            glShaderSource(shader, 1, &source, nullptr);
            glCompileShader(shader);

            GLint compiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            if (!compiled) {
                glDeleteShader(shader);
                compileFailed = true;
                break;
            }

            glAttachShader(pipeline->program, shader);
            shaders.push_back(shader);
        }

        if (compileFailed) {
            for (GLuint s : shaders) glDeleteShader(s);
            glDeleteProgram(pipeline->program);
            delete pipeline;
            return -3;
        }

        glLinkProgram(pipeline->program);

        GLint linked = 0;
        glGetProgramiv(pipeline->program, GL_LINK_STATUS, &linked);

        for (GLuint s : shaders) {
            glDeleteShader(s);
        }

        if (!linked) {
            glDeleteProgram(pipeline->program);
            delete pipeline;
            return -3;
        }

        pipeline->topology = 0x0004; 
        if (info.pInputAssemblyState) {
            int32_t vkTopo = info.pInputAssemblyState->topology;
            if (vkTopo == 0) pipeline->topology = 0x0000;
            else if (vkTopo == 1) pipeline->topology = 0x0001;
            else if (vkTopo == 2) pipeline->topology = 0x0002;
            else if (vkTopo == 3) pipeline->topology = 0x0004;
            else if (vkTopo == 4) pipeline->topology = 0x0005;
        }

        if (info.pRasterizationState) {
            pipeline->cullModeEnable = (info.pRasterizationState->cullMode != 0) ? 1 : 0;
            if (info.pRasterizationState->cullMode == 1) pipeline->cullFace = 0x0404;
            else if (info.pRasterizationState->cullMode == 2) pipeline->cullFace = 0x0405;
            else if (info.pRasterizationState->cullMode == 3) pipeline->cullFace = 0x0408;
            pipeline->frontFace = (info.pRasterizationState->frontFace == 0) ? 0x0901 : 0x0900;
        } else { pipeline->cullModeEnable = 0; }

        if (info.pDepthStencilState) {
            pipeline->depthTestEnable = info.pDepthStencilState->depthTestEnable;
            int32_t vkDepthOp = info.pDepthStencilState->depthCompareOp;
            if (vkDepthOp == 0) pipeline->depthCompareOp = 0x0200;
            else if (vkDepthOp == 1) pipeline->depthCompareOp = 0x0201;
            else if (vkDepthOp == 2) pipeline->depthCompareOp = 0x0202;
            else if (vkDepthOp == 3) pipeline->depthCompareOp = 0x0203;
            else if (vkDepthOp == 4) pipeline->depthCompareOp = 0x0204;
            else if (vkDepthOp == 5) pipeline->depthCompareOp = 0x0205;
            else if (vkDepthOp == 6) pipeline->depthCompareOp = 0x0206;
            else if (vkDepthOp == 7) pipeline->depthCompareOp = 0x0207;
        } else { pipeline->depthTestEnable = 0; }

        if (info.pColorBlendState && info.pColorBlendState->attachmentCount > 0) {
            const VkPipelineColorBlendAttachmentState& blend = info.pColorBlendState->pAttachments[0];
            pipeline->blendEnable = blend.blendEnable;
            pipeline->blendSrcFactor = 0x0302; 
            pipeline->blendDstFactor = 0x0303; 
            pipeline->blendEquation = 0x8006; 
        } else { pipeline->blendEnable = 0; }

        pPipelines[i] = pipeline;
    }
    return 0;
}

int32_t vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache, int32_t createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const void* pAllocator, VkPipeline* pPipelines) {
    if (!device || !pCreateInfos || !pPipelines) return -3;

    for (int32_t i = 0; i < createInfoCount; ++i) {
        VkPipeline pipeline = new VkPipeline_T();
        pipeline->bindPoint = 1;
        pipeline->program = glCreateProgram();

        GLuint shader = glCreateShader(0x91B9);
        const GLchar* source = static_cast<const GLchar*>(pCreateInfos[i].stage.module->code);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            glDeleteShader(shader);
            glDeleteProgram(pipeline->program);
            delete pipeline;
            return -3;
        }

        glAttachShader(pipeline->program, shader);
        glLinkProgram(pipeline->program);
        
        GLint linked = 0;
        glGetProgramiv(pipeline->program, GL_LINK_STATUS, &linked);
        
        glDeleteShader(shader);

        if (!linked) {
            glDeleteProgram(pipeline->program);
            delete pipeline;
            return -3;
        }

        pPipelines[i] = pipeline;
    }
    return 0;
}

void vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const void* pAllocator) {
    if (pipeline) {
        if (pipeline->program != 0) glDeleteProgram(pipeline->program);
        delete pipeline;
    }
}

}        
