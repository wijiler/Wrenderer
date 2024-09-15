#ifndef RENDERGRAPH_FANCY_H_
#define RENDERGRAPH_FANCY_H_

#include <renderer.h>
#include <string>
namespace jrv2
{

class jrRenderPass
{
  public:
    jrRenderPass(std::string name, passType type);
    ~jrRenderPass();
    void addColorAttachment(Image image, VkClearValue clear);
    void setDepthStencilAttachment(Image image);
    void addImageResource(Image image, ResourceUsageFlags_t usage);
    void addBufferResource(Buffer buf, ResourceUsageFlags_t usage);
    void setExecutionCallback(void (*callBack)(RenderPass, VkCommandBuffer cBuf));

    RenderPass pass{};

  private:
  protected:
};
class jrRenderGraph
{
  public:
    void addPass(jrRenderPass *pass);
    void build(Image currentSwapChainImage);
    void execute(VkCommandBuffer currentCommandBuffer);

    RenderGraph graph;
    GraphBuilder builder;

  private:
  protected:
};
} // namespace jrv2

#endif