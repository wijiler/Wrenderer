#include <util/camera.h>

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
