#include <inttypes.h>
#include <renderer.h>
#include <stdio.h>
#include <util/util.h>
#include <windowing.h>

renderer_t renderer;
WREScene2D scene = {0};
WRECamera camera = {0};
winf_t wininfo = {0};
GraphBuilder builder = {0};
spriteInstance birby1 = {0};
spriteInstance birby2 = {0};
spriteInstance birby3 = {0};
spriteInstance birby4 = {0};
spriteInstance birby5 = {0};
pointLight2D light = {
    {100.f, 0.f, 0.f},
    {1, 0.996, 0.816, 3.f},
    3.f,
    true,
    0,
};
int ImageIndex = 0;
int FrameIndex = 0;
int Index = 0;
float thing = 0;
int mul = 1;
void loop()
{
    FrameIndex++;
    Index = FrameIndex % FRAMECOUNT;
    thing += 0.001;
    if (thing >= 6.28319)
        thing = 0;
    camera.position.rotation.x += .001;
    updateLight(&light, &scene);
    updateCamera(&camera, &scene);
    updateSpriteInstance(&birby1, (transform2D){
                                      {0, 0, 0},
                                      {1, 1},
                                      thing,
                                  },
                         &scene);
    drawRenderer(&renderer, Index);
}

void init()
{
    initRenderer(&renderer);
    loadMeshFromGLTF("assets/dcubes.gltf", &renderer);
    scene.Renderer = &renderer;
    initializeScene(&scene);
    initPerspCamera(&camera, &renderer, (cameraTransform){(vec3){0, 0, 0}, (vec2){0, 0}}, 90);
    scene.camera = &camera;

    Sprite birb = createSprite("assets/birb.png", NULL, renderer.vkCore.nearestSampler, &renderer);
    Sprite birb2 = createSprite("assets/Wrenderer.png", NULL, renderer.vkCore.nearestSampler, &renderer);
    Sprite bnuyu = createSprite("assets/bnuyu.jpg", NULL, renderer.vkCore.linearSampler, &renderer);
    birby1 = createNewSpriteInstance(&bnuyu, renderer, &scene);
    birby2 = createNewSpriteInstance(&birb, renderer, &scene);
    birby3 = createNewSpriteInstance(&birb, renderer, &scene);
    birby4 = createNewSpriteInstance(&birb2, renderer, &scene);
    birby5 = createNewSpriteInstance(&birb2, renderer, &scene);
    updateSpriteInstance(&birby2, (transform2D){
                                      {0, -100, 0},
                                      {1, 1},
                                      0,
                                  },
                         &scene);
    updateSpriteInstance(&birby3, (transform2D){
                                      {0, -50, 0},
                                      {1, 1},
                                      0,
                                  },
                         &scene);

    updateSpriteInstance(&birby4, (transform2D){
                                      {0, 50, 0},
                                      {1, 1},
                                      0,
                                  },
                         &scene);
    updateSpriteInstance(&birby5, (transform2D){
                                      {0, 100, 0},
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