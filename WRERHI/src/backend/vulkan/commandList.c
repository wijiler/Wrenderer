#include <backends/vulkan/commandList.h>
#include <backends/vulkan/globals.h>
#include <backends/vulkan/image.h>
#include <pipeline.h>
#include <stdio.h>

uint64_t submitValue = 0;
uint64_t frameIndex = 0;
uint64_t cBufIndex = 0;
static const VkCommandBufferBeginInfo cBufBeginInf = {
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    NULL,
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    NULL,
};
static const VkClearColorValue clearValue = {{0.0f, 0.0f, 0.0f, 0.0f}};

static const VkImageSubresourceRange imgSRR = {
    VK_IMAGE_ASPECT_COLOR_BIT,
    0,
    VK_REMAINING_MIP_LEVELS,
    0,
    VK_REMAINING_ARRAY_LAYERS,
};

typedef struct
{
    VkRenderingAttachmentInfo attInfo[8];
} cAttInfo;

VkRenderingAttachmentInfo generateatt_inf(WREAttachment att, WRECommandData info, uint32_t i)
{
    if (att.usage == WRE_COLOR_ATTACHMENT)
    {
        return (VkRenderingAttachmentInfo){
            VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            NULL,
            info.renderpass.frameBuffers[i].img->imgview,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            0,
            NULL,
            0,
            VK_ATTACHMENT_LOAD_OP_LOAD,
            VK_ATTACHMENT_STORE_OP_STORE,
            {{{0, 0, 0, 0}}},
        };
    }
    else if (att.usage == WRE_DEPTH_IMAGE)
    {
        return (VkRenderingAttachmentInfo){
            VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            NULL,
            info.renderpass.frameBuffers[i].img->imgview,
            VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            0,
            NULL,
            0,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            {{{0, 0, 0, 0}}},
        };
    }
    // im too lazy to deal with stencil buffers rn
    else
    {
        return (VkRenderingAttachmentInfo){0};
    }
}
cAttInfo genColorAttachmentInfo(WRECommandData info)
{
    cAttInfo inf = {0};
    for (uint32_t i = 0; i < info.renderpass.frameBufCount; i++)
    {
        inf.attInfo[i] = generateatt_inf(info.renderpass.frameBuffers[i], info, i);
    }
    return inf;
}

void WREvkExecuteCommandList(RendererCoreContext *context, RendererWindowContext *winContext, WREcommandList *list)
{
    cBufIndex = frameIndex % FramesInFlightCount;
    VkCommandBuffer currentCommandBuffer = context->graphicsCommandBuffers[cBufIndex];
    {
        VkSemaphoreWaitInfo semaWaitInfo =
            {
                VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
                NULL,
                0,
                1,
                &context->graphicsTimeline,
                &submitValue,
            };
        vkWaitSemaphores(WREdevice, &semaWaitInfo, UINT64_MAX);
        submitValue++;
    }
    {
        VkResult res = vkAcquireNextImageKHR(WREdevice, winContext->swapChain, UINT64_MAX, context->imgAvailable[cBufIndex], VK_NULL_HANDLE, &winContext->CSCImgIndex);
        if (res == VK_ERROR_OUT_OF_DATE_KHR)
        {
            // smooth resize
        }
        *winContext->CurrentSCImg = winContext->SCImgs[winContext->CSCImgIndex];
    }
    vkResetCommandBuffer(currentCommandBuffer, 0);
    vkBeginCommandBuffer(currentCommandBuffer, &cBufBeginInf);
    transitionImageInCmdBuf(currentCommandBuffer, winContext->CurrentSCImg, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT);
    vkCmdClearColorImage(currentCommandBuffer, winContext->CurrentSCImg->img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearValue, 1, &imgSRR);
    transitionImageInCmdBuf(currentCommandBuffer, winContext->CurrentSCImg, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = winContext->w;
    viewport.height = winContext->h;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
    vkCmdSetViewportWithCount(currentCommandBuffer, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = winContext->w;
    scissor.extent.height = winContext->h;

    vkCmdSetScissorWithCount(currentCommandBuffer, 1, &scissor);

    WREpipeline boundPipeline = {0};
    {
        for (uint64_t i = 0; i < list->commandCount; i++)
        {
            WRECommand cc = list->commands[i];
            switch (cc.type)
            {
            case WRE_COMMAND_TYPE_RENDERPASS_START:
            {
                VkRenderingInfo renInf = {
                    VK_STRUCTURE_TYPE_RENDERING_INFO,
                    NULL,
                    0,
                    {{0, 0}, {winContext->w, winContext->h}},
                    1,
                    0,
                    cc.data.renderpass.frameBufCount,
                    genColorAttachmentInfo(cc.data).attInfo,
                    NULL,
                    NULL,
                };
                vkCmdBeginRendering(currentCommandBuffer, &renInf);
            }
            break;

            case WRE_COMMAND_TYPE_RENDERPASS_END:
            {
                vkCmdEndRendering(currentCommandBuffer);
            }
            break;
            case WRE_COMMAND_TYPE_DRAW:
            {
                vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, boundPipeline.layout, 0, 1, &WREimagedescriptorSet, 0, NULL);
                vkCmdDraw(currentCommandBuffer, cc.data.draw_call[0], cc.data.draw_call[1], 0, 0);
            }
            break;
            case WRE_COMMAND_TYPE_COMPUTE_DISPATCH:
            {
                ;
                vkCmdDispatch(currentCommandBuffer, cc.data.compute_dispatch[0], cc.data.compute_dispatch[1], cc.data.compute_dispatch[2]);
            }
            break;
            case WRE_COMMAND_TYPE_PUSH_CONSTANTS:
            {
                vkCmdPushConstants(currentCommandBuffer, boundPipeline.layout, cc.data.push_constant.stage, cc.data.push_constant.offset, cc.data.push_constant.size, cc.data.push_constant.data);
            }
            break;
            case WRE_COMMAND_TYPE_SET_PIPELINE:
            {
                vkCmdBindPipeline(currentCommandBuffer, (VkPipelineBindPoint)cc.data.pipeline->type, cc.data.pipeline->pipeline);
                boundPipeline = *cc.data.pipeline;
            }
            break;
            case WRE_COMMAND_TYPE_BIND_VERTEX:
            {
                vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, &cc.data.buffer_binding.buf, &cc.data.buffer_binding.offset);
            }
            break;
            case WRE_COMMAND_TYPE_BIND_INDEX:
            {
                vkCmdBindIndexBuffer(currentCommandBuffer, cc.data.buffer_binding.buf, cc.data.buffer_binding.offset, VK_INDEX_TYPE_UINT32);
            }
            break;
            default:
                break;
            }
        }
    }
    transitionImageInCmdBuf(currentCommandBuffer, winContext->CurrentSCImg, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_MEMORY_READ_BIT);
    vkEndCommandBuffer(currentCommandBuffer);

    VkSemaphoreSubmitInfo wSemaSubInf = {
        VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        NULL,
        context->imgAvailable[cBufIndex],
        0,
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        0,
    };
    VkSemaphoreSubmitInfo timelineSubInf = {
        VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        NULL,
        context->graphicsTimeline,
        submitValue,
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        0,
    };

    VkSemaphoreSubmitInfo graphicsSubInf = {
        VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        NULL,
        context->renderFinished[winContext->CSCImgIndex],
        1,
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        0,
    };

    VkCommandBufferSubmitInfo cBufSubmit = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        NULL,
        currentCommandBuffer,
        0,
    };
    VkSubmitInfo2 subInfo = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        NULL,
        0,
        1,
        &wSemaSubInf,
        1,
        &cBufSubmit,
        2,
        (VkSemaphoreSubmitInfo[2]){timelineSubInf, graphicsSubInf},
    };
    vkQueueSubmit2(WREgraphicsQueue, 1, &subInfo, NULL);

    VkPresentInfoKHR presInfo = {
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        NULL,
        1,
        &context->renderFinished[winContext->CSCImgIndex],
        1,
        &winContext->swapChain,
        &winContext->CSCImgIndex,
        NULL,
    };
    vkQueuePresentKHR(WREgraphicsQueue, &presInfo);

    frameIndex++;
}

#undef CATTINFO