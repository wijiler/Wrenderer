#include <inttypes.h>
#include <renderer.h>
#include <util/util.h>
#include <windowing.h>

renderer_t renderer;
winf_t wininfo = {0};
GraphBuilder builder = {0};

int ImageIndex = 0;
int FrameIndex = 0;
int Index = 0;
void loop()
{
    FrameIndex++;
    Index = FrameIndex % FRAMECOUNT;

    drawRenderer(&renderer, Index);
}

void init()
{
    initRenderer(&renderer);
    initializePipelines(renderer);

    Sprite birb = createSprite("assets/birb.png", &renderer);
    spriteInstance birby1 = createNewSpriteInstance(&birb, renderer);
    updateSpriteInstance(&birby1, (transform2D){
                                      .origin = {0, 0, 0},
                                      .scale = {1, 1},
                                      .rotation = 0,
                                  });
    RenderPass spPass = spritePass(renderer);

    addPass(&builder, &spPass);
    renderer.rg = &builder;

}
int main(void)
{
    wininfo.name = (char *)"Thing";
    wininfo.w = 1920;
    wininfo.h = 1080;
    launch_window(wininfo, &renderer, loop, init);

    destroyRenderer(&renderer);

    glfwDestroyWindow(renderer.vkCore.window);
    glfwTerminate();
}