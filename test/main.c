#include <inttypes.h>
#include <renderer.h>
#include <stdio.h>
#include <util/util.h>
#include <windowing.h>

renderer_t renderer;
WREScene3D scene = {0};
WRECamera camera = {0};
winf_t wininfo = {0};
GraphBuilder builder = {0};
// spriteInstance birby1 = {0};
// spriteInstance birby2 = {0};
// spriteInstance birby3 = {0};
// spriteInstance birby4 = {0};
// spriteInstance birby5 = {0};
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
    camera.position.rotation.x = (WREMouseX / renderer.vkCore.extent.width);
    camera.position.rotation.y = (WREMouseY / renderer.vkCore.extent.height);
    updateCamera(&camera, &renderer);
    drawRenderer(&renderer, Index);
}

void inputCallback(int key, int action)
{
    switch (key)
    {
    case GLFW_KEY_W:
        camera.position.pos.z -= 1;
        break;
    case GLFW_KEY_S:
        camera.position.pos.z += 1;
        break;
    case GLFW_KEY_A:
        camera.position.pos.x += 1;
        break;
    case GLFW_KEY_D:
        camera.position.pos.x -= 1;
        break;
    case GLFW_KEY_SPACE:
        camera.position.pos.y -= 1;
        break;
    case GLFW_KEY_LEFT_SHIFT:
        camera.position.pos.y += 1;
        break;
    }

    updateCamera(&camera, &renderer);
}

void init()
{
    initRenderer(&renderer);
    renderer.rg = &builder;
    initializeScene3D(&scene, &renderer);
    WREmesh mesh = loadMeshFromGLTF("assets/dcubes.gltf", &renderer);
    createMeshInstance(&mesh, &scene, &renderer, (vec3){0, 0, 0}, (vec3){0, 0, 0}, (vec3){1, 1, 1});
    initPerspCamera(&camera, &renderer, (cameraTransform){(vec3){0, 0, 200}, (vec2){0, 0}}, 90);
    setActiveCamera(&camera, renderer);
    RenderPass pass = meshPass(&scene, &renderer);
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
    // updateSpriteInstance(&birby2, (transform2D){
    //                                   {0, -100, 0},
    //                                   {1, 1},
    //                                   0,
    //                               },
    //                      &scene);
    // updateSpriteInstance(&birby3, (transform2D){
    //                                   {0, -50, 0},
    //                                   {1, 1},
    //                                   0,
    //                               },
    //                      &scene);

    // updateSpriteInstance(&birby4, (transform2D){
    //                                   {0, 50, 0},
    //                                   {1, 1},
    //                                   0,
    //                               },
    //                      &scene);
    // updateSpriteInstance(&birby5, (transform2D){
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