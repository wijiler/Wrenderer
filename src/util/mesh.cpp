#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <filesystem>
#include <stdio.h>
#include <string>
#include <util/camera.hpp>
#include <util/mesh.hpp>

uint32_t meshCount = 0;
uint32_t maxMeshCount = 0;
uint32_t maxInstanceCount = 0;
Buffer meshBuff{};
Buffer sceneInstanceBuff{};

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
    scene->MeshGroups = (WREMeshGroup *)malloc(sizeof(WREMeshGroup) * maxMeshCount);
    for (uint32_t i = 0; i < maxMeshCount; i++)
    {
        scene->MeshGroups[i].instanceCount = 0;
        scene->MeshGroups[i].instances = NULL;
        scene->MeshGroups[i].mesh = {};
    }
    scene->maxMeshGroupCount = maxMeshCount;
}

void allocateMesh(WREmesh *mesh, renderer_t *renderer)
{
    mesh->id = meshCount;
    {
        BufferCreateInfo bci{
            (int)(sizeof(uint32_t) * mesh->indexCount),
            BUFFER_USAGE_STORAGE_BUFFER,
            CPU_ONLY,
        };
        createBuffer(renderer->vkCore, bci, &mesh->iBuf);
        pushDataToBuffer(mesh->indices, sizeof(uint32_t) * mesh->indexCount, mesh->iBuf, 0);
    }
    if (meshCount + 1 > maxMeshCount)
    {
        BufferCreateInfo bci = {
            (int)(sizeof(gpuMesh) * maxMeshCount + 100),
            BUFFER_USAGE_STORAGE_BUFFER,
            CPU_ONLY,
        };
        Buffer newBuf{};
        createBuffer(renderer->vkCore, bci, &newBuf);
        copyBuf(renderer->vkCore, meshBuff, newBuf, sizeof(gpuMesh) * maxMeshCount, 0, 0);
        maxMeshCount += 100;
        destroyBuffer(meshBuff, renderer->vkCore);
        meshBuff = newBuf;
    }

    gpuMesh meshData{
        mesh->vBuf.gpuAddress,
        mesh->iBuf.gpuAddress,
        {mesh->material.AlbedoMap.index, mesh->material.NormalMap.index},
    };
    pushDataToBuffer(&meshData, sizeof(gpuMesh), meshBuff, sizeof(gpuMesh) * meshCount);
    meshCount += 1;
}

char *getParentDirectory(char *filepath)
{
    char *dir = NULL;
    {
        uint32_t len = strlen(filepath);
        dir = (char *)malloc(sizeof(char) * len);
        memcpy(dir, filepath, sizeof(char) * len);
        int newLen = 0;
        for (uint32_t i = len; i >= 0; i--)
        {
            if (dir[i] == '/')
            {
                dir = (char *)realloc(dir, sizeof(char) * (i + 1));
                newLen = i + 1;
                break;
            }
        }
        dir[newLen] = '\0';
    }
    return dir;
}

void createMeshInstance(WREmesh *mesh, WREScene3D *scene, renderer_t *renderer, vec3 position, vec3 rot, vec3 scale)
{
    if (maxMeshCount > scene->maxMeshGroupCount)
    {
        scene->MeshGroups = (WREMeshGroup *)realloc(scene->MeshGroups, sizeof(WREMeshGroup) * maxMeshCount);
        scene->maxMeshGroupCount = maxMeshCount;
    }
    WREMeshInstance instance{};
    mat4x4 modelMatrix = {};
    mat4x4 translation = mat4x4Translate(position, modelMatrix);
    mat4x4 rotation = mat4x4RotateQuat(eulerToQuaternion(rot));
    mat4x4 scaleMat = mat4x4Scale(scale, modelMatrix);
    instance.transform = mat4x4Mul(scaleMat, mat4x4Mul(rotation, translation));

    WREMeshGroup *group = &scene->MeshGroups[mesh->id];
    group->mesh = *mesh;
    if (group->instances == NULL)
        scene->meshGroupCount += 1;
    group->instances = (WREMeshInstance *)realloc(group->instances, sizeof(WREMeshInstance) * (group->instanceCount + 1));
    group->instances[group->instanceCount] = instance;
    if (scene->totalInstanceCount + 1 > maxInstanceCount)
    {
        maxInstanceCount += 100;
        BufferCreateInfo bci = {
            (int)(sizeof(WREMeshInstance) * maxInstanceCount + 100),
            BUFFER_USAGE_STORAGE_BUFFER,
            CPU_ONLY,
        };
        Buffer newBuf{};
        createBuffer(renderer->vkCore, bci, &newBuf);
        copyBuf(renderer->vkCore, sceneInstanceBuff, newBuf, sizeof(WREMeshInstance) * maxInstanceCount, 0, 0);
        maxInstanceCount += 100;
        destroyBuffer(sceneInstanceBuff, renderer->vkCore);
        sceneInstanceBuff = newBuf;
    }
    group->instanceCount += 1;
    scene->totalInstanceCount += 1;
}

void createMeshInstanceQuat(WREmesh *mesh, WREScene3D *scene, renderer_t *renderer, vec3 position, Quat rot, vec3 scale)
{
    if (maxMeshCount > scene->maxMeshGroupCount)
    {
        scene->MeshGroups = (WREMeshGroup *)realloc(scene->MeshGroups, sizeof(WREMeshGroup) * maxMeshCount);
        scene->maxMeshGroupCount = maxMeshCount;
    }
    WREMeshInstance instance{};
    mat4x4 translation = mat4x4Translate(position, identity4x4);
    mat4x4 rotation = transposeMat4x4(mat4x4RotateQuat(rot));
    mat4x4 scaleMat = mat4x4Scale(scale, identity4x4);
    instance.transform = transposeMat4x4(mat4x4Mul(mat4x4Mul(translation, rotation), scaleMat));

    WREMeshGroup *group = &scene->MeshGroups[mesh->id];
    group->mesh = *mesh;
    if (group->instances == NULL)
        scene->meshGroupCount += 1;
    group->instances = (WREMeshInstance *)realloc(group->instances, sizeof(WREMeshInstance) * (group->instanceCount + 1));
    group->instances[group->instanceCount] = instance;
    if (scene->totalInstanceCount + 1 > maxInstanceCount)
    {
        maxInstanceCount += 100;
        BufferCreateInfo bci = {
            (int)(sizeof(WREMeshInstance) * maxInstanceCount + 100),
            BUFFER_USAGE_STORAGE_BUFFER,
            CPU_ONLY,
        };
        Buffer newBuf{};
        createBuffer(renderer->vkCore, bci, &newBuf);
        copyBuf(renderer->vkCore, sceneInstanceBuff, newBuf, sizeof(WREMeshInstance) * maxInstanceCount, 0, 0);
        maxInstanceCount += 100;
        destroyBuffer(sceneInstanceBuff, renderer->vkCore);
        sceneInstanceBuff = newBuf;
    }
    group->instanceCount += 1;
    scene->totalInstanceCount += 1;
}

void createMeshInstanceTransform(WREmesh *mesh, WREScene3D *scene, renderer_t *renderer, mat4x4 mat)
{
    if (maxMeshCount > scene->maxMeshGroupCount)
    {
        scene->MeshGroups = (WREMeshGroup *)realloc(scene->MeshGroups, sizeof(WREMeshGroup) * maxMeshCount);
        scene->maxMeshGroupCount = maxMeshCount;
    }
    WREMeshInstance instance{};
    instance.transform = mat;

    WREMeshGroup *group = &scene->MeshGroups[mesh->id];
    group->mesh = *mesh;
    if (group->instances == NULL)
        scene->meshGroupCount += 1;
    group->instances = (WREMeshInstance *)realloc(group->instances, sizeof(WREMeshInstance) * (group->instanceCount + 1));
    group->instances[group->instanceCount] = instance;
    if (scene->totalInstanceCount + 1 > maxInstanceCount)
    {
        maxInstanceCount += 100;
        BufferCreateInfo bci = {
            (int)(sizeof(WREMeshInstance) * maxInstanceCount + 100),
            BUFFER_USAGE_STORAGE_BUFFER,
            CPU_ONLY,
        };
        Buffer newBuf{};
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
    WREScene3D *scene = (WREScene3D *)self.resources[1].value.arbitrary;
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
    std::string passName = "Meshpass";
    RenderPass gPass = newPass((char *)passName.c_str(), PASS_TYPE_GRAPHICS);
    gPass.gPl = scene->gbufferPipeline;
    addSwapchainImageResource(&gPass, *renderer);
    setDepthAttachment(&WREdepthBuffer, &gPass);
    addArbitraryResource(&gPass, scene);
    setExecutionCallBack(&gPass, meshGPass);

    return gPass;
}

mat4x4 fgltfToNative(fastgltf::math::fmat4x4 in)
{
    mat4x4 out;
    fastgltf::math::fvec4 row1 = in.row(0);
    fastgltf::math::fvec4 row2 = in.row(1);
    fastgltf::math::fvec4 row3 = in.row(2);
    fastgltf::math::fvec4 row4 = in.row(3);
    out._11 = row1.x();
    out._12 = row1.y();
    out._13 = row1.z();
    out._14 = row1.w();

    out._21 = row2.x();
    out._22 = row2.y();
    out._23 = row2.z();
    out._24 = row2.w();

    out._31 = row3.x();
    out._32 = row3.y();
    out._33 = row3.z();
    out._34 = row3.w();

    out._41 = row4.x();
    out._42 = row4.y();
    out._43 = row4.z();
    out._44 = row4.w();
    return out;
}

WREScene3D loadSceneGLTF(char *filepath, renderer_t *renderer)
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
    WREScene3D scene{};
    initializeScene3D(&scene, renderer);

    fastgltf::Parser parser;

    auto data = fastgltf::GltfDataBuffer::FromPath(filepath);
    if (data.error() != fastgltf::Error::None)
        return scene;

    char *path = getParentDirectory(filepath);
    auto asset = parser.loadGltf(data.get(), path, fastgltf::Options::LoadExternalBuffers | fastgltf::Options::LoadExternalImages | fastgltf::Options::GenerateMeshIndices | fastgltf::Options::DecomposeNodeMatrices);
    if (auto error = asset.error(); error != fastgltf::Error::None)
        return scene;

    if (asset->meshes.empty())
        return scene;
    std::vector<Texture> textures;
    for (fastgltf::Image &imageGltf : asset->images)
    {
        char *texPath = (char *)malloc(sizeof(char) * (strlen(path) + imageGltf.name.length() + 5));
        sprintf(texPath, "%s%s%s", path, imageGltf.name.c_str(), ".png");
        if (std::filesystem::exists(texPath))
        {
            Texture tex = loadImageFromPNG(texPath, renderer);
            submitTexture(renderer, &tex, renderer->vkCore.linearSampler);
            textures.push_back(tex);
        }
        else
        {
            textures.push_back(WREMissingTexture);
        }
        free(texPath);
    }
    std::vector<WREmesh> meshes;
    for (auto &meshGltf : asset->meshes)
    {
        WREmesh mesh{};
        mesh.material.AlbedoMap = WREDefaultTexture;
        mesh.material.NormalMap = WREDefaultNormal;

        fastgltf::Primitive &prim = meshGltf.primitives[0];
        if (prim.indicesAccessor.has_value())
        {
            fastgltf::Accessor &indexAccessor = asset->accessors[prim.indicesAccessor.value()];
            mesh.indices = (uint32_t *)malloc(sizeof(uint32_t) * indexAccessor.count);
            mesh.indexCount = indexAccessor.count;
            fastgltf::iterateAccessorWithIndex<uint32_t>(asset.get(), indexAccessor, [&](std::uint32_t index, size_t idx)
                                                         { mesh.indices[idx] = index; });
            int vertcount = 0;
            fastgltf::Attribute *positionIt = prim.findAttribute("POSITION");
            if (positionIt->accessorIndex < asset->accessors.size())
            {
                fastgltf::Accessor &positionAccessor = asset->accessors[positionIt->accessorIndex];
                if (positionAccessor.bufferViewIndex.has_value())
                {
                    mesh.vertices = (WREVertex3D *)malloc(sizeof(WREVertex3D) * positionAccessor.count);
                    {
                        BufferCreateInfo bci{
                            (int)(sizeof(WREVertex3D) * positionAccessor.count),
                            BUFFER_USAGE_STORAGE_BUFFER,
                            CPU_ONLY,
                        };
                        createBuffer(renderer->vkCore, bci, &mesh.vBuf);
                    }
                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset.get(), positionAccessor, [&](fastgltf::math::fvec3 pos, size_t idx)
                                                                              { 
                                                    mesh.vertices[idx].pos.x = pos.x(); 
                                                    mesh.vertices[idx].pos.y = pos.y(); 
                                                    mesh.vertices[idx].pos.z = pos.z(); 
                                                    mesh.vertices->color = {1,1,1,1};
                                                    mesh.vertices->uv = {0,0}; });
                    vertcount = positionAccessor.count;
                }
            }
            fastgltf::Attribute *colorIt = prim.findAttribute("COLOR_0");
            if (colorIt->accessorIndex < asset->accessors.size())
            {
                fastgltf::Accessor &colorAccessor = asset->accessors[colorIt->accessorIndex];
                if (colorAccessor.bufferViewIndex.has_value())
                {
                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset.get(), colorAccessor, [&](fastgltf::math::fvec3 color, size_t idx)
                                                                              {
                                                                        mesh.vertices[idx].color.x = color.x();
                                                                        mesh.vertices[idx].color.y = color.y();
                                                                        mesh.vertices[idx].color.z = color.z();
                                                                        mesh.vertices[idx].color.w = 1; });
                }
            }
            fastgltf::Attribute *uvIt = prim.findAttribute("TEXCOORD_0");
            if (uvIt->accessorIndex < asset->accessors.size())
            {
                fastgltf::Accessor &uvAccessor = asset->accessors[uvIt->accessorIndex];
                if (uvAccessor.bufferViewIndex.has_value())
                {
                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(asset.get(), uvAccessor, [&](fastgltf::math::fvec2 uv, size_t idx)
                                                                              { 
                                                                    mesh.vertices[idx].uv.x = uv.x();
                                                                    mesh.vertices[idx].uv.y = uv.y(); });
                }
            }
            pushDataToBuffer(mesh.vertices, sizeof(WREVertex3D) * vertcount, mesh.vBuf, 0);
        }
        for (uint32_t i = 0; i < meshGltf.primitives.size(); i++)
        {
            fastgltf::Primitive &prim = meshGltf.primitives[i];
            if (prim.materialIndex.has_value())
            {
                fastgltf::Material &mat = asset->materials[prim.materialIndex.value()];
                if (mat.pbrData.baseColorTexture.has_value())
                {
                    auto &tex = asset->textures[mat.pbrData.baseColorTexture->textureIndex];
                    if (tex.imageIndex.has_value())
                    {
                        mesh.material.AlbedoMap = textures[tex.imageIndex.value()];
                    }
                }
                break;
            }
        }
        allocateMesh(&mesh, renderer);
        meshes.push_back(mesh);
    }
    fastgltf::iterateSceneNodes(asset.get(), 0, fastgltf::math::fmat4x4(), [&](fastgltf::Node &node, fastgltf::math::fmat4x4 matrix)
                                {
                                    if(node.meshIndex.has_value()) 
                                    {
                                        mat4x4 transform = fgltfToNative(matrix);
                                        createMeshInstanceTransform(&meshes[node.meshIndex.value()], &scene, renderer, transposeMat4x4(transform));
                                    } });
    return scene;
}