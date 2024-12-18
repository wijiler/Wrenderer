#ifndef WRE_SPRITE_H__
#define WRE_SPRITE_H__
#include <renderer.h>
#include <util/camera.h>
#include <util/math.h>

typedef struct
{
    Texture image, normal;
    uint32_t instanceCount, id;
} Sprite;

typedef struct
{
    uint32_t texId, normalId;

} Material2D;

typedef struct
{
    transform2D transform;
    Sprite *parent;
    int id;
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
    Material2D *textureIDs;

} SpritePipeline;

typedef struct
{
    SpritePipeline spritePipeline;

    uint32_t lightCount;
    pointLight2D *lights;

    WRECamera *camera;
    renderer_t *Renderer;
} WREScene2D;

void addNewLight(pointLight2D *light, WREScene2D *scene);
void updateLight(pointLight2D *light, WREScene2D *scene);
void switchLight(pointLight2D *light, WREScene2D *scene);
void setActiveScene2D(WREScene2D *scene);
void initializeScene2D(WREScene2D *scene);

Sprite createSprite(char *texturePath, char *normalPath, VkSampler sampler, renderer_t *renderer);
spriteInstance createNewSpriteInstance(Sprite *sprite, renderer_t renderer, WREScene2D *scene);
void updateSpriteInstance(spriteInstance *sprite, transform2D transform, WREScene2D *scene);
void removeSpriteInstance(spriteInstance *sprite, WREScene2D *scene);
void deleteSpriteInstances(Sprite *sprite, WREScene2D *scene);
void spritePass(renderer_t renderer, SpritePipeline *pipeline);
void updateCamera(WRECamera *cam, WREScene2D *scene);

#endif