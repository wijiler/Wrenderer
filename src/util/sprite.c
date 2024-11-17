#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stdbool.h>
#include <util/sprite.h>

uint32_t spriteInstanceCount = 0;
uint32_t lightCount = 0;

Buffer spriteInstanceData = {0};
Buffer spriteTextureIDs = {0};
Buffer lightBuffer = {0};
static const int spriteIncrementAmount = 100;

Sprite createSprite(char *path, VkSampler sampler, renderer_t *renderer)
{
    Sprite sp = {0};
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
    return sp;
}
spriteInstance createNewSpriteInstance(Sprite *sprite, renderer_t renderer, WREScene2D *scene)
{
    spriteInstance instance;
    sprite->instanceCount += 1;
    instance.parent = sprite;
    if (spriteInstanceCount % spriteIncrementAmount == 0)
    {
        scene->spritePipeline.spriteInstances = realloc(scene->spritePipeline.spriteInstances, sizeof(spriteInstance) * (spriteInstanceCount + spriteIncrementAmount));
        scene->spritePipeline.spriteInstanceData = realloc(scene->spritePipeline.spriteInstanceData, sizeof(transform2D) * (scene->spritePipeline.spriteInstanceCount + spriteIncrementAmount));
        scene->spritePipeline.textureIDs = realloc(scene->spritePipeline.textureIDs, sizeof(uint64_t) * (scene->spritePipeline.spriteInstanceCount + spriteIncrementAmount));

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
    scene->spritePipeline.textureIDs[scene->spritePipeline.spriteInstanceCount] = sprite->image.index;
    instance.id = spriteInstanceCount;
    instance.transform = (transform2D){
        {0, 0, 0},
        {1, 1},
        0,
    };
    scene->spritePipeline.spriteInstances[spriteInstanceCount] = instance;
    scene->spritePipeline.spriteInstanceData[scene->spritePipeline.spriteInstanceCount] = instance.transform;
    spriteInstanceCount += 1;
    scene->spritePipeline.spriteInstanceCount += 1;

    return instance;
}
void updateSpriteInstance(spriteInstance *sprite, transform2D transform, WREScene2D *scene)
{
    scene->spritePipeline.spriteInstances[sprite->id].transform = transform;
    scene->spritePipeline.spriteInstanceData[sprite->id] = transform;
    *sprite = scene->spritePipeline.spriteInstances[sprite->id];
    pushDataToBuffer(scene->spritePipeline.spriteInstanceData, sizeof(transform2D) * scene->spritePipeline.spriteInstanceCount, spriteInstanceData, 0);
}
void removeSpriteInstance(spriteInstance *sprite, WREScene2D *scene)
{
    memcpy(scene->spritePipeline.spriteInstances + sprite->id, scene->spritePipeline.spriteInstances + sprite->id + 1, sizeof(spriteInstance) * (spriteInstanceCount - 1));
    memcpy(scene->spritePipeline.textureIDs + sprite->id, scene->spritePipeline.textureIDs + sprite->id + 1, sizeof(uint64_t) * (spriteInstanceCount - 1));
    for (uint32_t i = sprite->id; i < spriteInstanceCount; i++)
    {
        scene->spritePipeline.spriteInstances[i].id -= 1;
    }
    pushDataToBuffer(scene->spritePipeline.spriteInstanceData, sizeof(transform2D) * scene->spritePipeline.spriteInstanceCount, spriteInstanceData, 0);
}

void deleteSpriteInstances(Sprite *sprite, WREScene2D *scene)
{
    for (uint32_t i = 0; i < scene->spritePipeline.spriteInstanceCount; i++)
    {
        if (scene->spritePipeline.textureIDs[i] == sprite->image.index)
        {
            removeSpriteInstance(&scene->spritePipeline.spriteInstances[i], scene);
        }
    }
}

const VkImageSubresourceRange colorImgSRR = {
    VK_IMAGE_ASPECT_COLOR_BIT,
    0,
    VK_REMAINING_MIP_LEVELS,
    0,
    VK_REMAINING_ARRAY_LAYERS,
};
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
    bindGraphicsPipeline(self.gPl, self, cBuf);
    vkCmdPushConstants(cBuf, self.gPl.plLayout, SHADER_STAGE_ALL, 0, sizeof(pc), &data);

    vkCmdDraw(cBuf, 6, spriteInstanceCount, 0, 0);
}

void lightPassCallback(RenderPass self, VkCommandBuffer cBuf)
{
    typedef struct
    {
        uint32_t lightCount;
        VkDeviceAddress LightBuffer;
        VkDeviceAddress InstanceBuffer;
    } pc;
    pc data = {
        lightCount,
        lightBuffer.gpuAddress,
        spriteInstanceData.gpuAddress,
    };
    bindGraphicsPipeline(self.gPl, self, cBuf);
    vkCmdPushConstants(cBuf, self.gPl.plLayout, SHADER_STAGE_ALL, 0, sizeof(pc), &data);

    vkCmdDraw(cBuf, 6, spriteInstanceCount, 0, 0);
}

void spritePass(renderer_t renderer, SpritePipeline *pipeline)
{
    {
        RenderPass sPass = newPass("gbfferPass2D", PASS_TYPE_GRAPHICS);
        sPass.gPl = pipeline->gbufferPipeline;
        addImageResource(&sPass, &WREalbedoBuffer2D, USAGE_COLORATTACHMENT);
        setExecutionCallBack(&sPass, spritePassCallback);
        addPass(&pipeline->builder, &sPass);
    }
    {
        RenderPass lightPass = newPass("lightPass2D", PASS_TYPE_GRAPHICS);
        lightPass.gPl = pipeline->lightPipeline;
        addSwapchainImageResource(&lightPass, renderer);
        addImageResource(&lightPass, &WREalbedoBuffer2D, USAGE_SAMPLED);
        setExecutionCallBack(&lightPass, lightPassCallback);
        addPass(&pipeline->builder, &lightPass);
    }
}

void addNewLight(pointLight2D light, WREScene2D *scene)
{
    scene->lights = realloc(scene->lights, sizeof(pointLight2D) * (lightCount + 1));
    if (lightCount == 0)
    {
        BufferCreateInfo bci = {
            sizeof(light) * (lightCount + spriteIncrementAmount),
            BUFFER_USAGE_STORAGE_BUFFER | BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST,
            CPU_ONLY,
        };
        createBuffer(scene->Renderer->vkCore, bci, &lightBuffer);
    }
    scene->lights[lightCount] = light;
    scene->lightCount += 1;
    lightCount += 1;
}

void setActiveScene(WREScene2D *scene)
{
    pushDataToBuffer(scene->spritePipeline.spriteInstanceData, sizeof(transform2D) * scene->spritePipeline.spriteInstanceCount, spriteInstanceData, 0);
    pushDataToBuffer(scene->spritePipeline.textureIDs, sizeof(uint64_t) * scene->spritePipeline.spriteInstanceCount, spriteTextureIDs, 0);
    pushDataToBuffer(scene->lights, sizeof(pointLight2D) * scene->lightCount, lightBuffer, 0);
}

Image albedoBuffer2d = {0};

void initializePipelines(renderer_t *renderer, SpritePipeline *pipeline)
{
    {
        uint64_t Len = 0;
        uint32_t *Shader = NULL;

        readShaderSPRV("./shaders/sprite.spv", &Len, &Shader);
        pipeline->gbufferPipeline.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        pipeline->gbufferPipeline.colorBlending = VK_TRUE;
        pipeline->gbufferPipeline.logicOpEnable = VK_FALSE;
        pipeline->gbufferPipeline.reasterizerDiscardEnable = VK_FALSE;
        pipeline->gbufferPipeline.polyMode = VK_POLYGON_MODE_FILL;
        pipeline->gbufferPipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pipeline->gbufferPipeline.primitiveRestartEnable = VK_FALSE;
        pipeline->gbufferPipeline.depthBiasEnable = VK_FALSE;
        pipeline->gbufferPipeline.depthTestEnable = VK_FALSE;
        pipeline->gbufferPipeline.depthClampEnable = VK_FALSE;
        pipeline->gbufferPipeline.depthClipEnable = VK_FALSE;
        pipeline->gbufferPipeline.stencilTestEnable = VK_FALSE;
        pipeline->gbufferPipeline.alphaToCoverageEnable = VK_FALSE;
        pipeline->gbufferPipeline.rastSampleCount = VK_SAMPLE_COUNT_1_BIT;
        pipeline->gbufferPipeline.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        pipeline->gbufferPipeline.cullMode = VK_CULL_MODE_NONE;
        pipeline->gbufferPipeline.colorBlendEq = (VkColorBlendEquationEXT){
            VK_BLEND_FACTOR_SRC_ALPHA,
            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            VK_BLEND_OP_ADD,
            VK_BLEND_FACTOR_ONE,
            VK_BLEND_FACTOR_ZERO,
            VK_BLEND_OP_ADD,
        };
        pipeline->gbufferPipeline.depthBoundsEnable = VK_FALSE;
        pipeline->gbufferPipeline.alphaToOneEnable = VK_TRUE;
        pipeline->gbufferPipeline.sampleMask = UINT32_MAX;
        typedef struct
        {
            VkDeviceAddress SpriteBuffer;
            VkDeviceAddress InstanceBuffer;
        } pc;
        setPushConstantRange(&pipeline->gbufferPipeline, sizeof(pc), SHADER_STAGE_ALL, 0);
        addSetLayoutToGPL(&renderer->vkCore.tdSetLayout, &pipeline->gbufferPipeline);
        addDescriptorSetToGPL(&renderer->vkCore.tdescriptorSet, &pipeline->gbufferPipeline);

        setShaderSLSPRV(renderer->vkCore, &pipeline->gbufferPipeline, Shader, Len);

        createPipelineLayout(renderer->vkCore, &pipeline->gbufferPipeline);
    }
    {
        uint64_t Len = 0;
        uint32_t *Shader = NULL;

        readShaderSPRV("./shaders/spriteLighting.spv", &Len, &Shader);
        pipeline->lightPipeline.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        pipeline->lightPipeline.colorBlending = VK_TRUE;
        pipeline->lightPipeline.logicOpEnable = VK_FALSE;
        pipeline->lightPipeline.reasterizerDiscardEnable = VK_FALSE;
        pipeline->lightPipeline.polyMode = VK_POLYGON_MODE_FILL;
        pipeline->lightPipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pipeline->lightPipeline.primitiveRestartEnable = VK_FALSE;
        pipeline->lightPipeline.depthBiasEnable = VK_FALSE;
        pipeline->lightPipeline.depthTestEnable = VK_FALSE;
        pipeline->lightPipeline.depthClampEnable = VK_FALSE;
        pipeline->lightPipeline.depthClipEnable = VK_FALSE;
        pipeline->lightPipeline.stencilTestEnable = VK_FALSE;
        pipeline->lightPipeline.alphaToCoverageEnable = VK_FALSE;
        pipeline->lightPipeline.rastSampleCount = VK_SAMPLE_COUNT_1_BIT;
        pipeline->lightPipeline.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        pipeline->lightPipeline.cullMode = VK_CULL_MODE_NONE;
        pipeline->lightPipeline.colorBlendEq = (VkColorBlendEquationEXT){
            VK_BLEND_FACTOR_SRC_ALPHA,
            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            VK_BLEND_OP_ADD,
            VK_BLEND_FACTOR_ONE,
            VK_BLEND_FACTOR_ZERO,
            VK_BLEND_OP_ADD,
        };
        pipeline->lightPipeline.depthBoundsEnable = VK_FALSE;
        pipeline->lightPipeline.alphaToOneEnable = VK_TRUE;
        pipeline->lightPipeline.sampleMask = UINT32_MAX;
        typedef struct
        {
            uint32_t lightCount;
            VkDeviceAddress lightBuf;
            VkDeviceAddress instanceBuf;
        } pc;
        setPushConstantRange(&pipeline->lightPipeline, sizeof(pc), SHADER_STAGE_ALL, 0);
        addSetLayoutToGPL(&WREgBuffer2D.layout, &pipeline->lightPipeline);
        addDescriptorSetToGPL(&WREgBuffer2D.sets[0].set, &pipeline->lightPipeline);

        setShaderSLSPRV(renderer->vkCore, &pipeline->lightPipeline, Shader, Len);

        createPipelineLayout(renderer->vkCore, &pipeline->lightPipeline);
    }
}

void initializeScene(WREScene2D *scene)
{
    initializePipelines(scene->Renderer, &scene->spritePipeline);
}