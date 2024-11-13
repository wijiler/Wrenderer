#include <inttypes.h>
#include <renderer.h>
#include <util/util.h>
#include <windowing.h>

renderer_t renderer;
winf_t wininfo = {0};
GraphBuilder builder = {0};
spriteInstance birby1 = {0};
spriteInstance birby2 = {0};
spriteInstance birby3 = {0};
int ImageIndex = 0;
int FrameIndex = 0;
int Index = 0;
float thing = 0;
void loop()
{
    FrameIndex++;
    Index = FrameIndex % FRAMECOUNT;
    thing += 0.001;
    if (thing >= 6.28319)
        thing = 0;
    updateSpriteInstance(&birby1, (transform2D){
                                      {0, 0, 1},
                                      {1, 1},
                                      thing,
                                  });
    drawRenderer(&renderer, Index);
}

void init()
{
    initRenderer(&renderer);
    initializePipelines(renderer);

    Sprite birb = createSprite("assets/birb.png", renderer.vkCore.nearestSampler, &renderer);
    birby1 = createNewSpriteInstance(&birb, renderer);
    birby2 = createNewSpriteInstance(&birb, renderer);
    birby3 = createNewSpriteInstance(&birb, renderer);
    updateSpriteInstance(&birby2, (transform2D){
                                      {0, -10, 1},
                                      {1, 1},
                                      0,
                                  });
    updateSpriteInstance(&birby3, (transform2D){
                                      {0, -5, 1},
                                      {1, 1},
                                      0,
                                  });
    pointLight2D light = {
        {0.f, -10.f, 1.f},
        {1.f, 1.f, 1.f, 1.f},
        1.0f,
    };
    addNewLight(light, renderer);
    spritePass(renderer, &spritePipeline);
    copyGraph(&spritePipeline.builder, &builder);
    renderer.rg = &builder;
}
int main(void)
{
    wininfo.name = (char *)"Thing";
    wininfo.w = 1080;
    wininfo.h = 1080;
    launch_window(wininfo, &renderer, loop, init);

    destroyRenderer(&renderer);

    glfwDestroyWindow(renderer.vkCore.window);
    glfwTerminate();
}