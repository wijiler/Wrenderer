#ifndef WRE_SPRITE_H__
#define WRE_SPRITE_H__
#include <renderer.h>
#include <util/math.h>

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

Sprite createSprite(char *path, VkSampler sampler, renderer_t *renderer);
spriteInstance createNewSpriteInstance(Sprite *sprite, renderer_t renderer);
void updateSpriteInstance(spriteInstance *sprite, transform2D transform);
void removeSpriteInstance(spriteInstance *sprite);
void deleteSprite(Sprite *sprite);
RenderPass spritePass(renderer_t renderer);
void addNewLight(pointLight2D light, renderer_t renderer);
#endif