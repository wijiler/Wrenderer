#ifndef WINDOWING
#define WINDOWING
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <renderer.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        int w, h;
        char *name;
    } winf_t;

    void launch_window(winf_t wininfo, renderer_t *renderer, void (*update)(), void (*start)(), void (*input)(int key, int action));
    extern double WREMouseX, WREMouseY;
#ifdef __cplusplus
}
#endif
#endif
