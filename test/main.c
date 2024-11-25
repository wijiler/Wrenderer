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
pointLight2D light = {
    {0.f, -10.f, 0.f},
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
    camera.position.rotation.x += .001;
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
    scene.Renderer = &renderer;
    initializeScene(&scene);
    initPerspCamera(&camera, &renderer, (cameraTransform){(vec3){0, 0, 0}, (vec2){0, 0}}, 90);
    scene.camera = &camera;

    Sprite birb = createSprite("assets/birb.png", renderer.vkCore.nearestSampler, &renderer);
    birby1 = createNewSpriteInstance(&birb, renderer, &scene);
    birby2 = createNewSpriteInstance(&birb, renderer, &scene);
    birby3 = createNewSpriteInstance(&birb, renderer, &scene);
    updateSpriteInstance(&birby2, (transform2D){
                                      {0, -10, 0},
                                      {1, 1},
                                      0,
                                  },
                         &scene);
    updateSpriteInstance(&birby3, (transform2D){
                                      {0, -5, 0},
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
    mat4x4 a = {
        4, 3, 2, 1,
        3, 3, 5, 6,
        2, 5, 2, 8,
        1, 6, 8, 1};
    mat4x4 b = {
        4, 3, 2, 1,
        3, 3, 5, 6,
        2, 5, 2, 8,
        1, 6, 8, 1};
    mat4x4 test = mat4x4Mul(a, b);
    printf("%f , %f , %f , %f\n%f , %f , %f , %f\n%f , %f , %f , %f\n%f , %f , %f , %f\n", test._11, test._12, test._13, test._14, test._21, test._22, test._23, test._24, test._31, test._32, test._33, test._34, test._41, test._42, test._43, test._44);
    vec3 v3a = {1, 2, 3};
    vec3 v3b = {3, 4, 5};
    vec3 cross = vec3Cross(v3a, v3b);
    float dot = vec3Dot(v3a, v3b);
    printf("\n%f %f %f\n %f\n", cross.x, cross.y, cross.z, dot);
    wininfo.name = (char *)"Thing";
    wininfo.w = 1080;
    wininfo.h = 1080;
    launch_window(wininfo, &renderer, loop, init);

    destroyRenderer(&renderer);

    glfwDestroyWindow(renderer.window);
    glfwTerminate();
}