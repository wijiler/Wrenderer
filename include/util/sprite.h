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
    vec3 pos;
    vec4 color;
    float radius;
    bool on;
    uint32_t id;
} pointLight2D;

typedef struct
{
    GraphBuilder builder;
    graphicsPipeline gbufferPipeline;
    graphicsPipeline lightPipeline;
    //
    uint32_t spriteInstanceCount;
    spriteInstance *spriteInstances;
    transform2D *spriteInstanceData;
    uint64_t *textureIDs;

} SpritePipeline;

typedef struct
{
    SpritePipeline spritePipeline;

    uint32_t lightCount;
    pointLight2D *lights;

    renderer_t *Renderer;
} WREScene2D;

void addNewLight(pointLight2D *light, WREScene2D *scene);
void updateLight(pointLight2D *light, WREScene2D *scene);
void setActiveScene(WREScene2D *scene);
void initializeScene(WREScene2D *scene);

Sprite createSprite(char *path, VkSampler sampler, renderer_t *renderer);
spriteInstance createNewSpriteInstance(Sprite *sprite, renderer_t renderer, WREScene2D *scene);
void updateSpriteInstance(spriteInstance *sprite, transform2D transform, WREScene2D *scene);
void removeSpriteInstance(spriteInstance *sprite, WREScene2D *scene);
void deleteSpriteInstances(Sprite *sprite, WREScene2D *scene);
void spritePass(renderer_t renderer, SpritePipeline *pipeline);
#endif