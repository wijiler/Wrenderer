#include <inttypes.h>
#include <renderer.h>
#include <util/util.hpp>
#include <windowing.h>

renderer_t renderer;
WREScene3D scene = {};
WRECamera camera = {};
winf_t wininfo = {};
GraphBuilder builder = {};

int ImageIndex = 0;
int FrameIndex = 0;
int Index = 0;
void loop()
{
    Index = FrameIndex % FRAMECOUNT;
    camera.position.rotation.x = (WREMouseX / renderer.vkCore.extent.width) * 10;
    camera.position.rotation.y = (WREMouseY / renderer.vkCore.extent.height) * 10;
    updateCamera(&camera, &renderer);
    drawRenderer(&renderer, Index);
}

void inputCallback(int key, int action)
{
    float speed = 100 * WREstats.deltaTime;
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
            camera.position.pos.x -= speed;
            break;
        case GLFW_KEY_D:
            camera.position.pos.x += speed;
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
    WREstats.targetFPS = 160;
    initRenderer(&renderer);
    renderer.rg = &builder;
    initializeScene3D(&scene, &renderer);
    scene = loadSceneGLTF("assets/testScenes/gltf/standard/the_utah_teapot/scene.gltf", &renderer);
    initPerspCamera(&camera, &renderer, {{0, 0, 0}, {0, 0}}, 90);
    setActiveCamera(&camera, renderer);
    meshPass(&scene, &renderer);
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