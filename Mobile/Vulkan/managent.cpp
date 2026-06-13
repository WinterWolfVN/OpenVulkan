#include <GLES3/gl31.h>
#include <vector>

struct FakeCommandBuffer {
    enum CommandType { BIND_PIPELINE, BIND_DESCRIPTOR, DRAW };
    struct Command {
        CommandType type;
        GLuint programID;
        GLuint vaoID;
    };
    std::vector<Command> commands;
};

struct FakeSubmitInfo {
    FakeCommandBuffer* pCommandBuffers;
    int commandBufferCount;
    GLsync glFence; 
};

void TranslateAndSubmitQueue(FakeSubmitInfo* submitInfo) {
    if (!submitInfo || submitInfo->commandBufferCount == 0) return;

    for (int i = 0; i < submitInfo->commandBufferCount; ++i) {
        FakeCommandBuffer* cmdBuf = &submitInfo->pCommandBuffers[i];

        for (const auto& cmd : cmdBuf->commands) {
            switch (cmd.type) {
                case FakeCommandBuffer::BIND_PIPELINE:
                    glUseProgram(cmd.programID);
                    break;

                case FakeCommandBuffer::BIND_DESCRIPTOR:
                    glBindVertexArray(cmd.vaoID); 
                    break;

                case FakeCommandBuffer::DRAW:
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0); 
                    break;
            }
        }
    }

    submitInfo->glFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

void Management_RunFrame(FakeSubmitInfo* graphicsSubmit, FakeSubmitInfo* computeSubmit) {
    if (computeSubmit) {
        TranslateAndSubmitQueue(computeSubmit);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    if (graphicsSubmit) {
        TranslateAndSubmitQueue(graphicsSubmit);
    }

    if (graphicsSubmit && graphicsSubmit->glFence) {
        glClientWaitSync(graphicsSubmit->glFence, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
        glDeleteSync(graphicsSubmit->glFence);
    }
}
