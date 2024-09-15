// #include <rendergraph.hpp>

// using namespace jrv2;

// jrRenderPass::jrRenderPass(std::string name, passType type)
// {
//     this->pass = newPass(name.data(), type);
// }

// void jrRenderPass::addColorAttachment(Image image, VkClearValue clear)
// {
//     ::addColorAttachment(image, &this->pass, &clear);
// }

// void jrRenderPass::setDepthStencilAttachment(Image image)
// {
//     ::setDepthStencilAttachment(image, &this->pass);
// }

// void jrRenderPass::addImageResource(Image img, ResourceUsageFlags_t usage)
// {
//     ::addImageResource(&this->pass, img, usage);
// }

// void jrRenderPass::addBufferResource(Buffer buf, ResourceUsageFlags_t usage)
// {
//     ::addBufferResource(&this->pass, buf, usage);
// }

// void jrRenderPass::setExecutionCallback(void (*callBack)(RenderPass pass, VkCommandBuffer cBuf))
// {
//     ::setExecutionCallBack(&this->pass, callBack);
// }

// // ----------------------------------------------------------------------------

// void jrRenderGraph::addPass(jrRenderPass *pass)
// {
//     ::addPass(&this->builder, &pass->pass);
// }

// void jrRenderGraph::build(Image currentSwapChainImage)
// {
//     this->graph = buildGraph(&this->builder, currentSwapChainImage);
// }

// void jrRenderGraph::execute(VkCommandBuffer currentCommandBuffer)
// {
//     ::executeGraph(&this->graph, currentCommandBuffer);
// }