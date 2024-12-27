#include <inttypes.h>
#include <renderer.h>
#include <stdio.h>
#include <util/util.hpp>
#include <windowing.h>

renderer_t renderer;
WREScene3D scene = {};
WRECamera camera = {};
winf_t wininfo = {};
GraphBuilder builder = {};
// spriteInstance birby1 = {};
// spriteInstance birby2 = {};
// spriteInstance birby3 = {};
// spriteInstance birby4 = {};
// spriteInstance birby5 = {};
// pointLight2D light = {
//     {0.f, 0.f, 0.f},
//     {1, 0.996, 0.816, .2f},
//     1.f,
//     true,
//     0,
// };
int ImageIndex = 0;
int FrameIndex = 0;
int Index = 0;
void loop()
{
    FrameIndex++;
    Index = FrameIndex % FRAMECOUNT;
    camera.position.rotation.x = (WREMouseX / renderer.vkCore.extent.width) * 10;
    camera.position.rotation.y = (WREMouseY / renderer.vkCore.extent.height) * 10;
    updateCamera(&camera, &renderer);
    drawRenderer(&renderer, Index);
}

void inputCallback(int key, int action)
{
    float speed = 1000 * WREstats.deltaTime;
    if (action != GLFW_RELEASE)
    {
        switch (key)
        {
        case GLFW_KEY_W:
            camera.position.pos.z -= speed;
            break;
        case GLFW_KEY_S:
            camera.position.pos.z += speed;
            break;
        case GLFW_KEY_A:
            camera.position.pos.x += speed;
            break;
        case GLFW_KEY_D:
            camera.position.pos.x -= speed;
            break;
        case GLFW_KEY_SPACE:
            camera.position.pos.y += speed;
            break;
        case GLFW_KEY_LEFT_SHIFT:
            camera.position.pos.y -= speed;
            break;
        case GLFW_KEY_ESCAPE:
            exit(0);
            break;
        }
    }
}

void init()
{
    initRenderer(&renderer);
    renderer.rg = &builder;
    initializeScene3D(&scene, &renderer);
    WREScene3D mesh = loadSceneGLTF("assets/testScenes/gltf/deccers/dcubes.gltf", &renderer);
    initPerspCamera(&camera, &renderer, {{0, 0, 0}, {0, 0}}, 90);
    setActiveCamera(&camera, renderer);
    RenderPass pass = meshPass(&mesh, &renderer);
    addPass(renderer.rg, &pass);

    // scene.Renderer = &renderer;
    // initializeScene2D(&scene);
    // scene.camera = &camera;

    // Sprite birb = createSprite("assets/birb.png", NULL, renderer.vkCore.nearestSampler, &renderer);
    // Sprite birb2 = createSprite("assets/Wrenderer.png", NULL, renderer.vkCore.nearestSampler, &renderer);
    // Sprite bnuyu = createSprite("assets/bnuyu.jpg", NULL, renderer.vkCore.linearSampler, &renderer);
    // birby1 = createNewSpriteInstance(&bnuyu, renderer, &scene);
    // birby2 = createNewSpriteInstance(&birb, renderer, &scene);
    // birby3 = createNewSpriteInstance(&birb, renderer, &scene);
    // birby4 = createNewSpriteInstance(&birb2, renderer, &scene);
    // birby5 = createNewSpriteInstance(&birb2, renderer, &scene);
    // updateSpriteInstance(&birby2, {
    //                                   {0, -100, 0},
    //                                   {1, 1},
    //                                   0,
    //                               },
    //                      &scene);
    // updateSpriteInstance(&birby3, {
    //                                   {0, -50, 0},
    //                                   {1, 1},
    //                                   0,
    //                               },
    //                      &scene);

    // updateSpriteInstance(&birby4, {
    //                                   {0, 50, 0},
    //                                   {1, 1},
    //                                   0,
    //                               },
    //                      &scene);
    // updateSpriteInstance(&birby5, {
    //                                   {0, 100, 0},
    //                                   {1, 1},
    //                                   0,
    //                               },
    //                      &scene);

    // addNewLight(&light, &scene);
    // setActiveScene2D(&scene);
    // spritePass(renderer, &scene.spritePipeline);
    // copyGraph(&scene.spritePipeline.builder, &builder);
}

int main(void)
{
    wininfo.name = (char *)"Thing";
    wininfo.w = 1080;
    wininfo.h = 1080;
    launch_window(wininfo, &renderer, loop, init, inputCallback);

    destroyRenderer(&renderer);

    glfwDestroyWindow(renderer.window);
    glfwTerminate();
}