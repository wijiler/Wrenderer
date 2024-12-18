#define CGLTF_IMPLEMENTATION
#include <cgltf/cgltf.h>
#include <stdio.h>
#include <util/camera.h>
#include <util/mesh.h>

uint32_t meshCount = 0;
uint32_t maxMeshCount = 0;
uint32_t maxInstanceCount = 0;
Buffer meshBuff = {0};
Buffer sceneInstanceBuff = {0};

typedef struct
{
    uint32_t textureId;
    uint32_t normalId;
} gpuMaterial;
typedef struct
{
    VkDeviceAddress vert;
    VkDeviceAddress index;
    gpuMaterial mat;
} gpuMesh;
void initializePipelines3d(renderer_t *renderer, WREScene3D *scene)
{
    {
        uint64_t Len = 0;
        uint32_t *Shader = NULL;

        readShaderSPRV("./shaders/model.spv", &Len, &Shader);
        scene->gbufferPipeline.logicOpEnable = VK_FALSE;
        scene->gbufferPipeline.reasterizerDiscardEnable = VK_FALSE;
        scene->gbufferPipeline.polyMode = VK_POLYGON_MODE_FILL;
        scene->gbufferPipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        scene->gbufferPipeline.primitiveRestartEnable = VK_FALSE;
        scene->gbufferPipeline.depthBiasEnable = VK_FALSE;
        scene->gbufferPipeline.depthTestEnable = VK_TRUE;
        scene->gbufferPipeline.depthClampEnable = VK_FALSE;
        scene->gbufferPipeline.depthClipEnable = VK_FALSE;
        scene->gbufferPipeline.stencilTestEnable = VK_FALSE;
        scene->gbufferPipeline.alphaToCoverageEnable = VK_FALSE;
        scene->gbufferPipeline.rastSampleCount = VK_SAMPLE_COUNT_1_BIT;
        scene->gbufferPipeline.frontFace = VK_FRONT_FACE_CLOCKWISE;
        scene->gbufferPipeline.cullMode = VK_CULL_MODE_BACK_BIT;
        scene->gbufferPipeline.depthBoundsEnable = VK_FALSE;
        scene->gbufferPipeline.alphaToOneEnable = VK_TRUE;
        scene->gbufferPipeline.sampleMask = UINT32_MAX;
        scene->gbufferPipeline.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
        scene->gbufferPipeline.depthWriteEnable = VK_TRUE;
        scene->gbufferPipeline.maxDepth = 0;
        scene->gbufferPipeline.minDepth = 1;
        typedef struct
        {
            VkDeviceAddress MeshBuffer;
            VkDeviceAddress InstanceBuffer;
            VkDeviceAddress cameraBuf;
        } pc;
        setPushConstantRange(&scene->gbufferPipeline, sizeof(pc), SHADER_STAGE_ALL, 0);
        addDescriptorSetToGPL(&renderer->vkCore.textureDescriptorSet, &renderer->vkCore.textureDescriptor.layout, &scene->gbufferPipeline);
        addDescriptorSetToGPL(&renderer->vkCore.normalDescriptorSet, &renderer->vkCore.textureDescriptor.layout, &scene->gbufferPipeline);

        setShaderSLSPRV(renderer->vkCore, &scene->gbufferPipeline, Shader, Len);

        createPipelineLayout(renderer->vkCore, &scene->gbufferPipeline);
    }
    // {
    //     uint64_t Len = 0;
    //     uint32_t *Shader = NULL;

    //     readShaderSPRV("./shaders/modelLighting.spv", &Len, &Shader);
    //     scene->lightingPipeline.logicOpEnable = VK_FALSE;
    //     scene->lightingPipeline.reasterizerDiscardEnable = VK_FALSE;
    //     scene->lightingPipeline.polyMode = VK_POLYGON_MODE_FILL;
    //     scene->lightingPipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    //     scene->lightingPipeline.primitiveRestartEnable = VK_FALSE;
    //     scene->lightingPipeline.depthBiasEnable = VK_FALSE;
    //     scene->lightingPipeline.depthTestEnable = VK_TRUE;
    //     scene->lightingPipeline.depthClampEnable = VK_FALSE;
    //     scene->lightingPipeline.depthClipEnable = VK_FALSE;
    //     scene->lightingPipeline.stencilTestEnable = VK_FALSE;
    //     scene->lightingPipeline.alphaToCoverageEnable = VK_FALSE;
    //     scene->lightingPipeline.rastSampleCount = VK_SAMPLE_COUNT_1_BIT;
    //     scene->lightingPipeline.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    //     scene->lightingPipeline.cullMode = VK_CULL_MODE_NONE;
    //     scene->lightingPipeline.depthBoundsEnable = VK_FALSE;
    //     scene->lightingPipeline.alphaToOneEnable = VK_TRUE;
    //     scene->lightingPipeline.sampleMask = UINT32_MAX;
    //     typedef struct
    //     {
    //         uint32_t lightCount;
    //         VkDeviceAddress MeshBuf;
    //         VkDeviceAddress instanceBuf;
    //         VkDeviceAddress cameraBuf;
    //     } pc;
    //     setPushConstantRange(&scene->lightingPipeline, sizeof(pc), SHADER_STAGE_ALL, 0);
    //     addDescriptorSetToGPL(&WREgBuffer.sets[0].set, &WREgBuffer.layout, &scene->lightingPipeline);
    //     addDescriptorSetToGPL(&WREgBuffer.sets[1].set, &WREgBuffer.layout, &scene->lightingPipeline);

    //     setShaderSLSPRV(renderer->vkCore, &scene->lightingPipeline, Shader, Len);

    //     createPipelineLayout(renderer->vkCore, &scene->lightingPipeline);
    // }
}

void initializeScene3D(WREScene3D *scene, renderer_t *renderer)
{
    if (maxMeshCount == 0)
    {
        {
            BufferCreateInfo bci = {
                sizeof(gpuMesh) * 100,
                BUFFER_USAGE_STORAGE_BUFFER,
                CPU_ONLY,
            };
            createBuffer(renderer->vkCore, bci, &meshBuff);
            bci.dataSize = sizeof(WREMeshInstance) * 100;
            createBuffer(renderer->vkCore, bci, &sceneInstanceBuff);
        }
        maxMeshCount = 100;
        maxInstanceCount = 100;
    }

    initializePipelines3d(renderer, scene);
    scene->MeshGroups = malloc(sizeof(WREMeshGroup) * maxMeshCount);
    for (uint32_t i = 0; i < maxMeshCount; i++)
    {
        scene->MeshGroups[i].instanceCount = 0;
        scene->MeshGroups[i].instances = NULL;
        scene->MeshGroups[i].mesh = (WREmesh){0};
    }
    scene->maxMeshGroupCount = maxMeshCount;
}

void allocateMesh(WREmesh *mesh, renderer_t *renderer)
{
    mesh->id = meshCount;
    {
        BufferCreateInfo bci = {
            sizeof(uint32_t) * mesh->indexCount,
            BUFFER_USAGE_STORAGE_BUFFER,
            CPU_ONLY,
        };
        createBuffer(renderer->vkCore, bci, &mesh->iBuf);
        pushDataToBuffer(mesh->indices, sizeof(uint32_t) * mesh->indexCount, mesh->iBuf, 0);
    }
    if (meshCount + 1 > maxMeshCount)
    {
        BufferCreateInfo bci = {
            sizeof(gpuMesh) * maxMeshCount + 100,
            BUFFER_USAGE_STORAGE_BUFFER,
            CPU_ONLY,
        };
        Buffer newBuf = {0};
        createBuffer(renderer->vkCore, bci, &newBuf);
        copyBuf(renderer->vkCore, meshBuff, newBuf, sizeof(gpuMesh) * maxMeshCount, 0, 0);
        maxMeshCount += 100;
        destroyBuffer(meshBuff, renderer->vkCore);
        meshBuff = newBuf;
    }

    gpuMesh meshData = {
        mesh->vBuf.gpuAddress,
        mesh->iBuf.gpuAddress,
        (gpuMaterial){mesh->material.AlbedoMap.index, mesh->material.NormalMap.index},
    };
    pushDataToBuffer(&meshData, sizeof(gpuMesh), meshBuff, sizeof(gpuMesh) * meshCount);
    meshCount += 1;
}

WREmesh loadMeshFromGLTF(char *filepath, renderer_t *renderer)
{
    WREmesh mesh = {0};
    cgltf_options meshoptions = {0};
    cgltf_data *meshdata = NULL;
    cgltf_result meshresult = cgltf_parse_file(&meshoptions, filepath, &meshdata);
    char *dir = NULL;
    {
        uint32_t len = strlen(filepath);
        dir = malloc(sizeof(char) * len);
        memcpy(dir, filepath, sizeof(char) * len);
        int newLen = 0;
        for (uint32_t i = len; i >= 0; i--)
        {
            if (dir[i] == '/')
            {
                dir = realloc(dir, sizeof(char) * (i + 1));
                newLen = i + 1;
                break;
            }
        }
        dir[newLen] = '\0';
    }

    if (meshresult != cgltf_result_success)
    {
        printf("Could not load gltf %s E%i\n", filepath, meshresult);
        return mesh;
    }
    if (meshdata->meshes_count <= 0)
    {
        printf("No mesh contained in gltf file %s\n", filepath);
        return mesh;
    }
    cgltf_load_buffers(&meshoptions, meshdata, filepath);
    cgltf_mesh gltfMesh = meshdata->meshes[0];
    int vertCount = 0;
    for (uint32_t i = 0; i < gltfMesh.primitives_count; i++)
    {
        if (gltfMesh.primitives[i].type == cgltf_primitive_type_triangles)
        {
            cgltf_primitive prim = gltfMesh.primitives[i];
            mesh.indexCount = prim.indices[0].count;
            mesh.indices = malloc(sizeof(uint32_t) * prim.indices[0].count);
            uint16_t *Idat = (uint16_t *)prim.indices->buffer_view->buffer->data + prim.indices->buffer_view->offset / sizeof(uint16_t) + prim.indices->offset / sizeof(uint16_t);
            for (uint32_t i = 0; i < prim.indices[0].count; i++)
            {
                mesh.indices[i] = Idat[i];
            }

            for (uint32_t j = 0; j < prim.attributes_count; j++)
            {
                cgltf_attribute attrib = prim.attributes[j];
                if (mesh.vertices == NULL && (attrib.type == cgltf_attribute_type_position || attrib.type == cgltf_attribute_type_color || attrib.type == cgltf_attribute_type_texcoord))
                {
                    vertCount = attrib.data->count;
                    mesh.vertices = malloc(sizeof(WREVertex3D) * attrib.data->count);
                    {
                        BufferCreateInfo bci = {
                            sizeof(WREVertex3D) * vertCount,
                            BUFFER_USAGE_STORAGE_BUFFER,
                            CPU_ONLY,
                        };
                        createBuffer(renderer->vkCore, bci, &mesh.vBuf);
                    }
                }

                switch (attrib.type)
                {
                case cgltf_attribute_type_position:
                {
                    vec3 *data = (vec3 *)attrib.data->buffer_view->buffer->data + attrib.data->buffer_view->offset / sizeof(vec3) + attrib.data->offset / sizeof(vec3);

                    for (uint32_t d = 0; d < attrib.data->count; d++)
                    {
                        mesh.vertices[d].pos = data[d];
                    }
                }
                break;
                case cgltf_attribute_type_color:
                {
                    vec4 *data = (vec4 *)attrib.data->buffer_view->buffer->data + attrib.data->buffer_view->offset / sizeof(vec4) + attrib.data->offset / sizeof(vec4);

                    for (uint32_t d = 0; d < attrib.data->count; d++)
                    {
                        mesh.vertices[d].color = data[d];
                    }
                }
                break;
                case cgltf_attribute_type_texcoord:
                {
                    vec2 *data = (vec2 *)attrib.data->buffer_view->buffer->data + attrib.data->buffer_view->offset / sizeof(vec2) + attrib.data->offset / sizeof(vec2);
                    for (uint32_t d = 0; d < attrib.data->count; d++)
                    {
                        mesh.vertices[d].uv = data[d];
                    }
                }
                break;
                default:
                    break;
                }
            }
            pushDataToBuffer(mesh.vertices, sizeof(WREVertex3D) * vertCount, mesh.vBuf, 0);
            break;
        }
    }

    if (meshdata->textures_count > 0)
    {
        char *texture = strcat(dir, meshdata->textures[0].image->uri); // fuck whichever microsoft dev telling me this shit unsafe, I make sure this shit null terminated myself kys
        Texture tex = loadImageFromPNG(texture, renderer);
        submitTexture(renderer, &tex, renderer->vkCore.nearestSampler);
        mesh.material.AlbedoMap = tex;
        mesh.material.NormalMap = (Texture){{0}, 0};
    }
    if (maxMeshCount == 0)
    {
        {
            BufferCreateInfo bci = {
                sizeof(gpuMesh) * 100,
                BUFFER_USAGE_STORAGE_BUFFER,
                CPU_ONLY,
            };
            createBuffer(renderer->vkCore, bci, &meshBuff);
            bci.dataSize = sizeof(WREMeshInstance) * 100;
            createBuffer(renderer->vkCore, bci, &sceneInstanceBuff);
        }
        maxMeshCount = 100;
        maxInstanceCount = 100;
    }
    allocateMesh(&mesh, renderer);
    cgltf_free(meshdata);
    return mesh;
}

void createMeshInstance(WREmesh *mesh, WREScene3D *scene, renderer_t *renderer, vec3 position, vec3 rot, vec3 scale)
{
    if (maxMeshCount > scene->maxMeshGroupCount)
    {
        scene->MeshGroups = realloc(scene->MeshGroups, sizeof(WREMeshGroup) * maxMeshCount);
        scene->maxMeshGroupCount = maxMeshCount;
    }
    WREMeshInstance instance = {0};
    mat4x4 modelMatrix = identity4x4;
    mat4x4Translate(position, &modelMatrix);
    modelMatrix = mat4x4Mul(modelMatrix, mat4x4RotateQuat(eulerToQuaternion(rot)));
    mat4x4Scale(scale, &modelMatrix);
    instance.transform = modelMatrix;

    WREMeshGroup *group = &scene->MeshGroups[mesh->id];
    group->mesh = *mesh;
    if (group->instances == NULL)
        scene->meshGroupCount += 1;
    group->instances = realloc(group->instances, sizeof(WREMeshInstance) * (group->instanceCount + 1));
    group->instances[group->instanceCount] = instance;
    if (scene->totalInstanceCount + 1 > maxInstanceCount)
    {
        maxInstanceCount += 100;
        BufferCreateInfo bci = {
            sizeof(WREMeshInstance) * maxInstanceCount + 100,
            BUFFER_USAGE_STORAGE_BUFFER,
            CPU_ONLY,
        };
        Buffer newBuf = {0};
        createBuffer(renderer->vkCore, bci, &newBuf);
        copyBuf(renderer->vkCore, sceneInstanceBuff, newBuf, sizeof(WREMeshInstance) * maxInstanceCount, 0, 0);
        maxInstanceCount += 100;
        destroyBuffer(sceneInstanceBuff, renderer->vkCore);
        sceneInstanceBuff = newBuf;
    }
    group->instanceCount += 1;
    scene->totalInstanceCount += 1;
}

void meshGPass(RenderPass self, VkCommandBuffer cBuf)
{
    WREScene3D *scene = self.resources[1].value.arbitrary;
    typedef struct
    {
        VkDeviceAddress InstanceBuffer;
        VkDeviceAddress MeshBuffer;
        VkDeviceAddress cameraBuf;
    } pc;
    int accum = 0;
    for (uint32_t i = 0; i < scene->meshGroupCount; i++)
    {
        WREMeshGroup group = scene->MeshGroups[i];
        pushDataToBuffer(group.instances, sizeof(WREMeshInstance) * group.instanceCount, sceneInstanceBuff, sizeof(WREMeshInstance) * accum);
        pc data = {
            sceneInstanceBuff.gpuAddress + (sizeof(WREMeshInstance) * accum),
            meshBuff.gpuAddress + (sizeof(gpuMesh) * group.mesh.id),
            activeCameraBuffer.gpuAddress,
        };
        accum += group.instanceCount;

        bindGraphicsPipeline(self.gPl, self, cBuf);
        vkCmdPushConstants(cBuf, self.gPl.plLayout, SHADER_STAGE_ALL, 0, sizeof(pc), &data);

        vkCmdDraw(cBuf, group.mesh.indexCount, group.instanceCount, 0, 0);
    }
}

RenderPass meshPass(WREScene3D *scene, renderer_t *renderer)
{
    RenderPass gPass = newPass("name", PASS_TYPE_GRAPHICS);
    gPass.gPl = scene->gbufferPipeline;
    addSwapchainImageResource(&gPass, *renderer);
    setDepthAttachment(&WREdepthBuffer, &gPass);
    addArbitraryResource(&gPass, scene);
    setExecutionCallBack(&gPass, meshGPass);

    return gPass;
}

WREScene3D loadSceneGLTF()
{
}