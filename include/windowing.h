#ifndef WINDOWING
#define WINDOWING
#define GLFW_INCLUDE_VULKAN
#include <renderer.h>
#include <GLFW/glfw3.h>
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

    void launch_window(winf_t wininfo, renderer_t *renderer, void *PFN_update, void *PFN_start);
#ifdef __cplusplus
}
#endif
#endif
