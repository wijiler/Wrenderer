#include <stdbool.h>
#include <util/sprite.h>

uint32_t spriteInstanceCount = 0;
uint32_t lightCount = 0;

Buffer spriteInstanceData = {0};
Buffer spriteTextureIDs = {0};
Buffer lightBuffer = {0};
static const int spriteIncrementAmount = 100;

Sprite createSprite(char *texturePath, char *normalPath, VkSampler sampler, renderer_t *renderer)
{
    Sprite sp = {0};
    Texture tex = loadImageFromPNG(texturePath, renderer);
    submitTexture(renderer, &tex, sampler);
    if (normalPath == NULL)
        sp.normal = (Texture){{0}, 0};
    else
    {
        Texture normal = loadImageFromPNG(normalPath, renderer);
        submitTexture(renderer, &normal, sampler);
        sp.normal = normal;
    }
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
        scene->spritePipeline.textureIDs = realloc(scene->spritePipeline.textureIDs, sizeof(Material2D) * (scene->spritePipeline.spriteInstanceCount + spriteIncrementAmount));

        if (spriteInstanceCount == 0)
        {
            BufferCreateInfo bci = {
                sizeof(transform2D) * (spriteInstanceCount + spriteIncrementAmount),
                BUFFER_USAGE_STORAGE_BUFFER | BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST,
                CPU_ONLY,
            };
            createBuffer(renderer.vkCore, bci, &spriteInstanceData);
            bci.dataSize = sizeof(Material2D) * (spriteInstanceCount + spriteIncrementAmount);
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
                bci.dataSize = sizeof(Material2D) * (spriteInstanceCount + spriteIncrementAmount);
                Buffer newBuf = {0};
                createBuffer(renderer.vkCore, bci, &newBuf);
                copyBuf(renderer.vkCore, spriteTextureIDs, newBuf, sizeof(Material2D) * (spriteInstanceCount), 0, 0);
                destroyBuffer(spriteTextureIDs, renderer.vkCore);
                spriteTextureIDs = newBuf;
            }
        }
    }
    scene->spritePipeline.textureIDs[scene->spritePipeline.spriteInstanceCount] = (Material2D){sprite->image.index, sprite->normal.index};
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
    memcpy(scene->spritePipeline.textureIDs + sprite->id, scene->spritePipeline.textureIDs + sprite->id + 1, sizeof(Material2D) * (spriteInstanceCount - 1));
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
        if (scene->spritePipeline.textureIDs[i].texId == sprite->image.index)
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
        VkDeviceAddress cameraBuf;
    } pc;
    pc data = {
        spriteTextureIDs.gpuAddress,
        spriteInstanceData.gpuAddress,
        activeCameraBuffer.gpuAddress,
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
        VkDeviceAddress cameraBuf;
    } pc;
    pc data = {
        lightCount,
        lightBuffer.gpuAddress,
        spriteInstanceData.gpuAddress,
        activeCameraBuffer.gpuAddress,
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
        addImageResource(&sPass, &WREalbedoBuffer, USAGE_COLORATTACHMENT);
        addImageResource(&sPass, &WREnormalBuffer, USAGE_COLORATTACHMENT);
        setExecutionCallBack(&sPass, spritePassCallback);
        addPass(&pipeline->builder, &sPass);
    }
    {
        RenderPass lightPass = newPass("lightPass2D", PASS_TYPE_GRAPHICS);
        lightPass.gPl = pipeline->lightPipeline;
        addSwapchainImageResource(&lightPass, renderer);
        addImageResource(&lightPass, &WREalbedoBuffer, USAGE_SAMPLED);
        addImageResource(&lightPass, &WREnormalBuffer, USAGE_SAMPLED);
        setExecutionCallBack(&lightPass, lightPassCallback);
        addPass(&pipeline->builder, &lightPass);
    }
}

void addNewLight(pointLight2D *light, WREScene2D *scene)
{
    scene->lights = realloc(scene->lights, sizeof(pointLight2D) * (lightCount + 1));
    if (lightCount == 0)
    {
        BufferCreateInfo bci = {
            sizeof(pointLight2D) * (lightCount + spriteIncrementAmount),
            BUFFER_USAGE_STORAGE_BUFFER | BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST,
            CPU_ONLY,
        };
        createBuffer(scene->Renderer->vkCore, bci, &lightBuffer);
    }
    scene->lights[lightCount] = *light;
    light->id = lightCount;
    light->on = true;
    scene->lightCount += 1;
    lightCount += 1;
}

void updateLight(pointLight2D *light, WREScene2D *scene)
{
    scene->lights[light->id] = *light;
    pushDataToBuffer(scene->lights, sizeof(pointLight2D) * scene->lightCount, lightBuffer, 0);
}

void switchLight(pointLight2D *light, WREScene2D *scene)
{
    light->on = !light->on;
    scene->lights[light->id] = *light;
    pushDataToBuffer(scene->lights, sizeof(pointLight2D) * scene->lightCount, lightBuffer, 0);
}

void setActiveScene(WREScene2D *scene)
{
    if (!cameraSet)
    {
        {
            BufferCreateInfo BCI = {
                sizeof(WRECamera),
                BUFFER_USAGE_STORAGE_BUFFER,
                CPU_ONLY,
            };
            createBuffer(scene->Renderer->vkCore, BCI, &activeCameraBuffer);
        }
    }
    WRECamera camera = *scene->camera;
    vkUnmapMemory(scene->Renderer->vkCore.lDev, activeCameraBuffer.associatedMemory);
    vkMapMemory(scene->Renderer->vkCore.lDev, activeCameraBuffer.associatedMemory, 0, sizeof(WRECamera), 0, (void *)&scene->camera);
    memcpy(scene->camera, &camera, sizeof(WRECamera));
    pushDataToBuffer(scene->lights, sizeof(pointLight2D) * scene->lightCount, lightBuffer, 0);
    pushDataToBuffer(scene->spritePipeline.spriteInstanceData, sizeof(transform2D) * scene->spritePipeline.spriteInstanceCount, spriteInstanceData, 0);
    pushDataToBuffer(scene->spritePipeline.textureIDs, sizeof(Material2D) * scene->spritePipeline.spriteInstanceCount, spriteTextureIDs, 0);
}

Image albedoBuffer2d = {0};

void initializePipelines(renderer_t *renderer, SpritePipeline *pipeline)
{
    {
        uint64_t Len = 0;
        uint32_t *Shader = NULL;

        readShaderSPRV("./shaders/sprite.spv", &Len, &Shader);
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
        pipeline->gbufferPipeline.depthBoundsEnable = VK_FALSE;
        pipeline->gbufferPipeline.alphaToOneEnable = VK_TRUE;
        pipeline->gbufferPipeline.sampleMask = UINT32_MAX;
        typedef struct
        {
            VkDeviceAddress SpriteBuffer;
            VkDeviceAddress InstanceBuffer;
            VkDeviceAddress cameraBuf;
        } pc;
        setPushConstantRange(&pipeline->gbufferPipeline, sizeof(pc), SHADER_STAGE_ALL, 0);
        addDescriptorSetToGPL(&renderer->vkCore.textureDescriptorSet, &renderer->vkCore.textureDescriptor.layout, &pipeline->gbufferPipeline);
        addDescriptorSetToGPL(&renderer->vkCore.normalDescriptorSet, &renderer->vkCore.textureDescriptor.layout, &pipeline->gbufferPipeline);

        setShaderSLSPRV(renderer->vkCore, &pipeline->gbufferPipeline, Shader, Len);

        createPipelineLayout(renderer->vkCore, &pipeline->gbufferPipeline);
    }
    {
        uint64_t Len = 0;
        uint32_t *Shader = NULL;

        readShaderSPRV("./shaders/spriteLighting.spv", &Len, &Shader);
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
        pipeline->lightPipeline.depthBoundsEnable = VK_FALSE;
        pipeline->lightPipeline.alphaToOneEnable = VK_TRUE;
        pipeline->lightPipeline.sampleMask = UINT32_MAX;
        typedef struct
        {
            uint32_t lightCount;
            VkDeviceAddress lightBuf;
            VkDeviceAddress instanceBuf;
            VkDeviceAddress cameraBuf;
        } pc;
        setPushConstantRange(&pipeline->lightPipeline, sizeof(pc), SHADER_STAGE_ALL, 0);
        addDescriptorSetToGPL(&WREgBuffer.sets[0].set, &WREgBuffer.layout, &pipeline->lightPipeline);
        addDescriptorSetToGPL(&WREgBuffer.sets[1].set, &WREgBuffer.layout, &pipeline->lightPipeline);

        setShaderSLSPRV(renderer->vkCore, &pipeline->lightPipeline, Shader, Len);

        createPipelineLayout(renderer->vkCore, &pipeline->lightPipeline);
    }
}

void initializeScene(WREScene2D *scene)
{
    initializePipelines(scene->Renderer, &scene->spritePipeline);
}

void updateCamera(WRECamera *cam, WREScene2D *scene)
{
    switch (cam->type)
    {
    case WRE_ORTHOGRAPHIC_CAM:
    {
        initOrthoCamera(cam, scene->Renderer, cam->position.pos, cam->position.rotation);
    }
    break;
    case WRE_PERSPECTIVE_CAM:
    {
        initPerspCamera(cam, scene->Renderer, cam->position, cam->fov);
    }
    break;
    }
    memcpy(scene->camera, cam, sizeof(WRECamera));
}