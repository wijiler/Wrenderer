#ifndef WRE_CAMERA_H__
#define WRE_CAMERA_H__

#include <renderer.h>
#include <util/math.h>

typedef struct
{
    mat4x4 perspective;
    mat4x4 view;
    Image *renderTarget;
} WRECamera;

#endif