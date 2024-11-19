#include <inttypes.h>
#include <renderer.h>
#include <util/util.h>
#include <windowing.h>

renderer_t renderer;
WREScene2D scene = {0};
winf_t wininfo = {0};
GraphBuilder builder = {0};
spriteInstance birby1 = {0};
spriteInstance birby2 = {0};
spriteInstance birby3 = {0};
pointLight2D light = {
    {0.f, -10.f, 1.f},
    {1.f, 1.f, 1.f, 1.f},
    1.0f,
    true,
    0,
};
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
                                  },
                         &scene);
    light.pos.y -= 0.001f;
    updateLight(&light, &scene);
    drawRenderer(&renderer, Index);
}

void init()
{
    initRenderer(&renderer);
    scene.Renderer = &renderer;
    initializeScene(&scene);

    Sprite birb = createSprite("assets/birb.png", renderer.vkCore.nearestSampler, &renderer);
    birby1 = createNewSpriteInstance(&birb, renderer, &scene);
    birby2 = createNewSpriteInstance(&birb, renderer, &scene);
    birby3 = createNewSpriteInstance(&birb, renderer, &scene);
    updateSpriteInstance(&birby2, (transform2D){
                                      {0, -10, 1},
                                      {1, 1},
                                      0,
                                  },
                         &scene);
    updateSpriteInstance(&birby3, (transform2D){
                                      {0, -5, 1},
                                      {1, 1},
                                      0,
                                  },
                         &scene);

    addNewLight(&light, &scene);
    setActiveScene(&scene);
    spritePass(renderer, &scene.spritePipeline);
    copyGraph(&scene.spritePipeline.builder, &builder);
    renderer.rg = &builder;
}
int main(void)
{
    wininfo.name = (char *)"Thing";
    wininfo.w = 1080;
    wininfo.h = 1080;
    launch_window(wininfo, &renderer, loop, init);

    destroyRenderer(&renderer);

    glfwDestroyWindow(renderer.window);
    glfwTerminate();
}