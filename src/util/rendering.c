#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stdbool.h>
#include <util/util.h>

MeshHandler spriteHandler = {0};

graphicsPipeline spritePipeline = {0};

uint32_t spriteInstanceCount = 0;
uint32_t spriteCount = 0;
uint64_t *textureIDs;
spriteInstance *spriteInstances;
transform2D *spriteInstanceDataCPU;
Buffer spriteInstanceData = {0};
Buffer spriteTextureIDs = {0};
const int spriteIncrementAmount = 100;

void initializePipelines(renderer_t renderer)
{
    {
        uint64_t Len = 0;
        uint32_t *Shader = NULL;

        readShaderSPRV("./shaders/sprite.spv", &Len, &Shader);
        spritePipeline.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        spritePipeline.colorBlending = VK_TRUE;
        spritePipeline.logicOpEnable = VK_FALSE;
        spritePipeline.reasterizerDiscardEnable = VK_FALSE;
        spritePipeline.polyMode = VK_POLYGON_MODE_FILL;
        spritePipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        spritePipeline.primitiveRestartEnable = VK_FALSE;
        spritePipeline.depthBiasEnable = VK_FALSE;
        spritePipeline.depthTestEnable = VK_FALSE;
        spritePipeline.depthClampEnable = VK_FALSE;
        spritePipeline.depthClipEnable = VK_FALSE;
        spritePipeline.stencilTestEnable = VK_FALSE;
        spritePipeline.alphaToCoverageEnable = VK_FALSE;
        spritePipeline.rastSampleCount = VK_SAMPLE_COUNT_1_BIT;
        spritePipeline.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        spritePipeline.cullMode = VK_CULL_MODE_NONE;
        spritePipeline.colorBlendEq = (VkColorBlendEquationEXT){
            VK_BLEND_FACTOR_SRC_ALPHA,
            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            VK_BLEND_OP_ADD,
            VK_BLEND_FACTOR_ONE,
            VK_BLEND_FACTOR_ZERO,
            VK_BLEND_OP_ADD,
        };
        spritePipeline.depthBoundsEnable = VK_FALSE;
        spritePipeline.alphaToOneEnable = VK_TRUE;
        spritePipeline.sampleMask = UINT32_MAX;
        setPushConstantRange(&spritePipeline, sizeof(VkDeviceAddress) * 2, SHADER_STAGE_VERTEX);
        createPipelineLayout(renderer.vkCore, &spritePipeline);
        setShaderSLSPRV(renderer.vkCore, &spritePipeline, Shader, Len);
    }
    {
        BufferCreateInfo bCI = {
            sizeof(transform2D) * spriteIncrementAmount,
            BUFFER_USAGE_STORAGE_BUFFER | BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST,
            CPU_ONLY,
        };
        createBuffer(renderer.vkCore, bCI, &spriteInstanceData);
        bCI.dataSize = sizeof(uint32_t) * spriteIncrementAmount;
        createBuffer(renderer.vkCore, bCI, &spriteTextureIDs);
    }
}

Sprite createSprite(char *path, renderer_t *renderer)
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
        stbi_set_flip_vertically_on_load(false);
    }
    submitTexture(renderer, &tex, renderer->vkCore.linearSampler);
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
        BufferCreateInfo bci = {
            sizeof(transform2D) * (spriteInstanceCount + spriteIncrementAmount - 1),
            BUFFER_USAGE_STORAGE_BUFFER | BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST,
            CPU_ONLY,
        };
        if (spriteInstanceCount != 0)
        {
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
    instance.id = spriteInstanceCount;
    instance.transform = (transform2D){
        (Vector3){0, 0, 0},
        (Vector2){1, 1},
        0,
    };
    spriteInstances[spriteInstanceCount] = instance;
    spriteInstanceCount += 1;

    return instance;
}
void updateSpriteInstance(spriteInstance *sprite, transform2D transform)
{
    spriteInstances[sprite->id].transform = transform;
    spriteInstanceDataCPU[sprite->id] = transform;
    *sprite = spriteInstances[sprite->id];
}
void removeSpriteInstance(spriteInstance *sprite)
{
    memcpy(spriteInstances + sprite->id, spriteInstances + sprite->id + 1, sizeof(spriteInstance) * (spriteInstanceCount - 1)); // shrink ->x_x<-
    memcpy(textureIDs + sprite->id, textureIDs + sprite->id + 1, sizeof(uint64_t) * (spriteInstanceCount - 1));                 // shrink ->x_x<-
    for (uint32_t i = sprite->id; i < spriteInstanceCount; i++)
    {
        spriteInstances[i].id -= 1;
    }
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
    vkCmdPushConstants(cBuf, self.gPl.plLayout, SHADER_STAGE_VERTEX, 0, sizeof(pc), &data);

    vkCmdDraw(cBuf, 6, spriteInstanceCount, 0, 0);

    pushDataToBuffer(spriteInstanceDataCPU, sizeof(transform2D) * spriteInstanceCount, spriteInstanceData, 0);
    pushDataToBuffer(textureIDs, sizeof(uint64_t) * spriteCount, spriteTextureIDs, 0);
}

RenderPass spritePass(renderer_t renderer)
{
    RenderPass sPass = newPass("SpritePass", PASS_TYPE_GRAPHICS);
    sPass.gPl = spritePipeline;
    addImageResource(&sPass, renderer.vkCore.currentScImg, USAGE_COLORATTACHMENT);
    setExecutionCallBack(&sPass, spritePassCallback);
    return sPass;
}