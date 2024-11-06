#ifndef WREUTIL_H_
#define WREUTIL_H_

#include <renderer.h>

typedef struct
{
    float x, y;
} Vector2;

typedef struct
{
    float x, y, z;
} Vector3;

typedef struct
{
    float x, y, z, w;
} Vector4;

typedef Vector4 Quat;

typedef struct
{
    Vector3 origin;
    Vector2 scale;
    float rotation;
} transform2D;

typedef struct
{
    Vector3 origin;
    Vector3 scale;
    Quat rotation;
} transform3D;

typedef struct
{
    Texture image;
    uint32_t instanceCount, id;
} Sprite;

typedef struct
{
    transform2D transform;
    Sprite *parent;
    uint32_t id;
} spriteInstance;

typedef struct
{
    Vector3 pos;
    Vector4 color;
} pointLight2D;

extern graphicsPipeline spritePipeline;
void initializePipelines(renderer_t renderer);

Sprite createSprite(char *path, VkSampler sampler, renderer_t *renderer);
spriteInstance createNewSpriteInstance(Sprite *sprite, renderer_t renderer);
void updateSpriteInstance(spriteInstance *sprite, transform2D transform);
void removeSpriteInstance(spriteInstance *sprite);
void deleteSprite(Sprite *sprite);
RenderPass spritePass(renderer_t renderer);
void addNewLight(pointLight2D light, renderer_t renderer);
#endif