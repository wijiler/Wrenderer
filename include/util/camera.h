#ifndef WRE_CAMERA_H__
#define WRE_CAMERA_H__

#include <WREmath.h>
#include <renderer.h>
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum
    {
        WRE_ORTHOGRAPHIC_CAM,
        WRE_PERSPECTIVE_CAM,
    } WRECAMERATYPE;

    typedef struct
    {
        vec3 pos;
        vec2 rotation;
    } cameraTransform;

    typedef struct
    {
        WRECAMERATYPE type;
        mat4x4 perspective;
        mat4x4 view;
        cameraTransform position;
        float fov;
    } WRECamera;

    extern bool cameraSet;
    extern Buffer activeCameraBuffer;

    void initOrthoCamera(WRECamera *cam, renderer_t *renderer, vec3 pos, vec2 rotation);
    void initPerspCamera(WRECamera *cam, renderer_t *renderer, cameraTransform transform, float fov);
    void setActiveCamera(WRECamera *cam, renderer_t renderer);
    void updateCamera(WRECamera *cam, renderer_t *renderer);
    vec3 getCameraForward(WRECamera *cam);
#ifdef __cplusplus
}
#endif
#endif