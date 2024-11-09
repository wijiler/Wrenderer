#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stdbool.h>
#include <util/sprite.h>
uint32_t spriteInstanceCount = 0;
uint32_t spriteCount = 0;
uint32_t lightCount = 0;
uint64_t *textureIDs;
spriteInstance *spriteInstances;
transform2D *spriteInstanceDataCPU;
pointLight2D *lights;
Buffer spriteInstanceData = {0};
Buffer spriteTextureIDs = {0};
Buffer lightBuffer = {0};
static const int spriteIncrementAmount = 100;

Sprite createSprite(char *path, VkSampler sampler, renderer_t *renderer)
{
    Sprite sp = {0};
    sp.id = spriteCount;
    int texWidth, texHeight, texChannels;
    stbi_set_flip_vertically_on_load(true);
    stbi_uc *img = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    Texture tex = createTexture(renderer->vkCore, texWidth, texHeight);
    {
        BufferCreateInfo tci = {
            texWidth * texHeight * 4,
            BUFFER_USAGE_TRANSFER_SRC,
            CPU_ONLY,
        };

        Buffer buf;

        createBuffer(renderer->vkCore, tci, &buf);
        pushDataToBuffer(img, texWidth * texHeight * 4, buf, 0);
        copyDataToTextureImage(renderer->vkCore, &tex.img, &buf, texWidth, texHeight);

        destroyBuffer(buf, renderer->vkCore);
        stbi_image_free(img);
    }
    submitTexture(renderer, &tex, sampler);
    stbi_set_flip_vertically_on_load(false);
    sp.image = tex;
    spriteCount += 1;
    return sp;
}
spriteInstance createNewSpriteInstance(Sprite *sprite, renderer_t renderer)
{
    spriteInstance instance;
    sprite->instanceCount += 1;
    instance.parent = sprite;
    if (spriteInstanceCount % spriteIncrementAmount == 0)
    {
        spriteInstances = realloc(spriteInstances, sizeof(spriteInstance) * (spriteInstanceCount + spriteIncrementAmount));
        spriteInstanceDataCPU = realloc(spriteInstanceDataCPU, sizeof(transform2D) * (spriteInstanceCount + spriteIncrementAmount));
        textureIDs = realloc(textureIDs, sizeof(uint64_t) * (spriteInstanceCount + spriteIncrementAmount));

        if (spriteInstanceCount == 0)
        {
            BufferCreateInfo bci = {
                sizeof(transform2D) * (spriteInstanceCount + spriteIncrementAmount),
                BUFFER_USAGE_STORAGE_BUFFER | BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST,
                CPU_ONLY,
            };
            createBuffer(renderer.vkCore, bci, &spriteInstanceData);
            bci.dataSize = sizeof(uint64_t) * (spriteInstanceCount + spriteIncrementAmount);
            createBuffer(renderer.vkCore, bci, &spriteTextureIDs);
        }

        if (spriteInstanceCount != 0)
        {
            BufferCreateInfo bci = {
                sizeof(transform2D) * (spriteInstanceCount + spriteIncrementAmount - 1),
                BUFFER_USAGE_STORAGE_BUFFER | BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST,
                CPU_ONLY,
            };
            {
                Buffer newBuf = {0};
                createBuffer(renderer.vkCore, bci, &newBuf);
                copyBuf(renderer.vkCore, spriteInstanceData, newBuf, sizeof(transform2D) * (spriteInstanceCount), 0, 0);
                destroyBuffer(spriteInstanceData, renderer.vkCore);
                spriteInstanceData = newBuf;
            }
            {
                bci.dataSize = sizeof(uint64_t) * (spriteInstanceCount + spriteIncrementAmount);
                Buffer newBuf = {0};
                createBuffer(renderer.vkCore, bci, &newBuf);
                copyBuf(renderer.vkCore, spriteTextureIDs, newBuf, sizeof(uint64_t) * (spriteInstanceCount), 0, 0);
                destroyBuffer(spriteTextureIDs, renderer.vkCore);
                spriteTextureIDs = newBuf;
            }
        }
    }
    textureIDs[spriteInstanceCount] = sprite->image.index;
    instance.id = spriteInstanceCount;
    instance.transform = (transform2D){
        {0, 0, 0},
        {1, 1},
        0,
    };
    spriteInstances[spriteInstanceCount] = instance;
    spriteInstanceDataCPU[spriteInstanceCount] = instance.transform;
    spriteInstanceCount += 1;
    pushDataToBuffer(spriteInstanceDataCPU, sizeof(transform2D) * spriteInstanceCount, spriteInstanceData, 0);
    pushDataToBuffer(textureIDs, sizeof(uint64_t) * spriteInstanceCount, spriteTextureIDs, 0);

    return instance;
}
void updateSpriteInstance(spriteInstance *sprite, transform2D transform)
{
    spriteInstances[sprite->id].transform = transform;
    spriteInstanceDataCPU[sprite->id] = transform;
    *sprite = spriteInstances[sprite->id];
    pushDataToBuffer(spriteInstanceDataCPU, sizeof(transform2D) * spriteInstanceCount, spriteInstanceData, 0);
}
void removeSpriteInstance(spriteInstance *sprite)
{
    memcpy(spriteInstances + sprite->id, spriteInstances + sprite->id + 1, sizeof(spriteInstance) * (spriteInstanceCount - 1)); // shrink ->x_x<-
    memcpy(textureIDs + sprite->id, textureIDs + sprite->id + 1, sizeof(uint64_t) * (spriteInstanceCount - 1));                 // shrink ->x_x<-
    for (uint32_t i = sprite->id; i < spriteInstanceCount; i++)
    {
        spriteInstances[i].id -= 1;
    }
    pushDataToBuffer(spriteInstanceDataCPU, sizeof(transform2D) * spriteInstanceCount, spriteInstanceData, 0);
}

void deleteSprite(Sprite *sprite)
{
    for (uint32_t i = 0; i < spriteInstanceCount; i++)
    {
        if (spriteInstances[i].parent == sprite)
        {
            removeSpriteInstance(&spriteInstances[i]);
        }
    }
}

void spritePassCallback(RenderPass self, VkCommandBuffer cBuf)
{
    typedef struct
    {
        VkDeviceAddress SpriteBuffer;
        VkDeviceAddress InstanceBuffer;
    } pc;
    pc data = {
        spriteTextureIDs.gpuAddress,
        spriteInstanceData.gpuAddress,
    };
    bindGraphicsPipeline(self.gPl, cBuf);
    vkCmdPushConstants(cBuf, self.gPl.plLayout, SHADER_STAGE_ALL, 0, sizeof(pc), &data);

    vkCmdDraw(cBuf, 6, spriteInstanceCount, 0, 0);
}

RenderPass spritePass(renderer_t renderer)
{
    RenderPass sPass = newPass("SpritePass", PASS_TYPE_GRAPHICS);
    sPass.gPl = spritePipeline.gbufferPass;
    addImageResource(&sPass, renderer.vkCore.currentScImg, USAGE_COLORATTACHMENT);
    setExecutionCallBack(&sPass, spritePassCallback);
    return sPass;
}

void addNewLight(pointLight2D light, renderer_t renderer)
{
    lights = realloc(lights, sizeof(pointLight2D) * (lightCount + 1));
    if (lightCount == 0)
    {
        BufferCreateInfo bci = {
            sizeof(light) * (lightCount + spriteIncrementAmount),
            BUFFER_USAGE_STORAGE_BUFFER | BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST,
            CPU_ONLY,
        };
        createBuffer(renderer.vkCore, bci, &lightBuffer);
    }
    lights[lightCount] = light;
    lightCount += 1;
    pushDataToBuffer(lights, sizeof(pointLight2D) * lightCount, lightBuffer, 0);
}