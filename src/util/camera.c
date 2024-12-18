#include <util/camera.h>

bool cameraSet = false;
Buffer activeCameraBuffer = {0};
WRECamera *activeCam = NULL;

void initOrthoCamera(WRECamera *cam, renderer_t *renderer, vec3 pos, vec2 rotation)
{
    cam->type = WRE_ORTHOGRAPHIC_CAM;

    cam->view = transposeMat4x4(fpsViewMatrix(pos, rotation.y + PI32, rotation.x));

    cam->position.pos = pos;
    cam->position.rotation = rotation;

    cam->perspective = transposeMat4x4(orthoProjMatrix(1, 0, 1, -1, -1, 1));
}

void initPerspCamera(WRECamera *cam, renderer_t *renderer, cameraTransform transform, float fov)
{
    cam->type = WRE_PERSPECTIVE_CAM;

    cam->view = transposeMat4x4(fpsViewMatrix(transform.pos, transform.rotation.y, transform.rotation.x));

    cam->position.pos = transform.pos;
    cam->position.rotation = transform.rotation;

    cam->perspective = transposeMat4x4(perspProjMatrix(fov, (float)renderer->vkCore.extent.width / renderer->vkCore.extent.height, 1, 0));
    cam->fov = fov;
}

void setActiveCamera(WRECamera *cam, renderer_t renderer)
{
    if (cameraSet == false)
    {
        BufferCreateInfo bci = {
            sizeof(WRECamera),
            BUFFER_USAGE_STORAGE_BUFFER,
            CPU_ONLY,
        };
        createBuffer(renderer.vkCore, bci, &activeCameraBuffer);
    }
    pushDataToBuffer(cam, sizeof(WRECamera), activeCameraBuffer, 0);
    activeCam = cam;
}

void updateCamera(WRECamera *cam, renderer_t *renderer)
{
    switch (cam->type)
    {
    case WRE_ORTHOGRAPHIC_CAM:
    {
        initOrthoCamera(cam, renderer, cam->position.pos, cam->position.rotation);
    }
    break;
    case WRE_PERSPECTIVE_CAM:
    {
        initPerspCamera(cam, renderer, cam->position, cam->fov);
    }
    break;
    }
    if (cam == activeCam)
        pushDataToBuffer(cam, sizeof(WRECamera), activeCameraBuffer, 0);
}