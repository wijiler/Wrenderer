#include <context.h>
#include <pipeline.h>
#include <windowing.h>

WREContexObject context = {0};
int main()
{
    Wremonitor monitor = getMonitorInfo();
    WREwindow window = openWindow("Wre2 Rendering Test Bench", monitor.w / 7, monitor.h / 7, 1920, 1080);
    context.window = window;
    initializeContext(&context);
    WREShader shader = createShader("A:\\projects\\WRE2\\Test\\shader\\test.spv", WRE_SHADER_STAGE_VERTEX | WRE_SHADER_STAGE_FRAGMENT);
    WREpipeline pipeline = createPipeline("trianglePipeline", (WREvertexFormat){0, NULL, 0, NULL}, &shader, 1, WRE_BACK_CULLING, WRE_WINDING_CW, (VkFormat[8]){VK_FORMAT_R8G8B8A8_SRGB}, 1);

    WRECommandList list = {0};
    initializeCommandList(&list);

    bindPipeline(&list, pipeline);
    startRenderPass(&list, (WREImage[]){{0}});
    drawCall(&list, 3, 1);
    endRenderPass(&list);
    dispatchCompute(&list, 32, 32, 32);

    submitCommandList(&context, list);

    // VkCommandBufferBeginInfo begInfo = {
    //     VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    //     NULL,
    //     0,
    //     NULL,
    // };
    // VkRenderingAttachmentInfo renAttInf = {
    //     VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
    //     NULL,
    //     context.window.context.SCImgs[0].imgview,
    //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    //     0,
    //     NULL,
    //     0,
    //     VK_ATTACHMENT_LOAD_OP_LOAD,
    //     VK_ATTACHMENT_STORE_OP_STORE,
    //     {{{0, 0, 0, 0}}},
    // };
    // VkRenderingInfo renInf = {
    //     VK_STRUCTURE_TYPE_RENDERING_INFO,
    //     NULL,
    //     0,
    //     {{0, 0}, {1920, 1080}},
    //     1,
    //     0,
    //     1,
    //     &renAttInf,
    //     NULL,
    //     NULL,
    // };
    // context.window.context.SCImgs[0].access = 0;
    // transitionImage(&context.window.context.SCImgs[0], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    // vkBeginCommandBuffer(context.core.graphicsCommandBuffers[0], &begInfo);

    // vkCmdBeginRendering(context.core.graphicsCommandBuffers[0], &renInf);
    // vkCmdBindPipeline(context.core.graphicsCommandBuffers[0], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
    // VkViewport viewport = {};
    // viewport.x = 0;
    // viewport.y = 0;
    // viewport.width = 1920;
    // viewport.height = 1080;
    // viewport.minDepth = 0.f;
    // viewport.maxDepth = 1.f;

    // vkCmdSetViewportWithCount(context.core.graphicsCommandBuffers[0], 1, &viewport);

    // VkRect2D scissor = {};
    // scissor.offset.x = 0;
    // scissor.offset.y = 0;
    // scissor.extent.width = 1980;
    // scissor.extent.height = 1080;

    // vkCmdSetScissorWithCount(context.core.graphicsCommandBuffers[0], 1, &scissor);

    // vkCmdDraw(context.core.graphicsCommandBuffers[0], 3, 1, 0, 0);

    // vkCmdEndRendering(context.core.graphicsCommandBuffers[0]);

    // vkEndCommandBuffer(context.core.graphicsCommandBuffers[0]);
    // transitionImage(&context.window.context.SCImgs[0], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_MEMORY_READ_BIT);

    // VkSemaphoreSubmitInfo wSemaSubInf = {
    //     VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
    //     NULL,
    //     context.core.renderFinished[0],
    //     0,
    //     VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT_KHR,
    //     0,
    // };
    // VkSemaphoreSubmitInfo timelineSubInf = {
    //     VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
    //     NULL,
    //     context.core.graphicsTimeline,
    //     1,
    //     VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
    //     0,
    // };

    // VkSemaphoreSubmitInfo graphicsSubInf = {
    //     VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
    //     NULL,
    //     context.core.imgAvailable[0],
    //     1,
    //     VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
    //     0,
    // };

    // VkSemaphoreSubmitInfo semaSubInfo[2] = {timelineSubInf, graphicsSubInf};

    // VkCommandBufferSubmitInfo cBufSubmit = {
    //     VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
    //     NULL,
    //     context.core.graphicsCommandBuffers[0],
    //     0,
    // };
    // VkSubmitInfo2 subInfo = {
    //     VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
    //     NULL,
    //     0,
    //     1,
    //     &wSemaSubInf,
    //     1,
    //     &cBufSubmit,
    //     2,
    //     semaSubInfo,
    // };
    // vkQueueSubmit2(WREgraphicsQueue, 1, &subInfo, NULL);

    // uint32_t indices = 0;
    // VkPresentInfoKHR presInfo = {
    //     VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    //     NULL,
    //     1,
    //     &context.core.renderFinished[0],
    //     1,
    //     &context.window.context.swapChain,
    //     &indices,
    //     NULL,
    // };
    // vkQueuePresentKHR(WREgraphicsQueue, &presInfo);

    while (!glfwWindowShouldClose(window.window))
    {
        glfwPollEvents();
    }
    terminateContext(&context);

    return 0;
}