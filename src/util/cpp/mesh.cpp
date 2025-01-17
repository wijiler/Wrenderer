#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <filesystem>
#include <stdio.h>
#include <string>
#include <util/camera.h>
#include <util/mesh.hpp>
#include <variant>

uint32_t meshCount = 0;
uint32_t maxMeshCount = 0;
uint32_t maxInstanceCount = 0;
Buffer meshBuff{};
Buffer sceneInstanceBuff{};
Image WREOutputImage{};

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
        scene->gbufferPipeline.maxDepth = 1;
        scene->gbufferPipeline.minDepth = 0;
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
    {
        uint64_t Len = 0;
        uint32_t *Shader = NULL;

        readShaderSPRV("./shaders/modelLighting.spv", &Len, &Shader);

        typedef struct
        {
            VkDeviceAddress cameraBuf;
            uint32_t lightCount;
            VkDeviceAddress lightBuffer;
            VkExtent2D screenSize;
        } pc;
        setComputePushConstantRange(&scene->lightingPipeline, sizeof(pc));
        addDescriptorSetToCPL(&WREgBuffer.sets[0].set, &WREgBuffer.layout, &scene->lightingPipeline);
        addDescriptorSetToCPL(&WREgBuffer.sets[1].set, &WREgBuffer.layout, &scene->lightingPipeline);

        setCompShaderSPRV(renderer->vkCore, &scene->lightingPipeline, Shader, Len);

        createComputePipelineLayout(renderer->vkCore, &scene->lightingPipeline);
    }
}

void initializeScene3D(WREScene3D *scene, renderer_t *renderer)
{
    if (WREOutputImage.image == NULL)
    {
        WREOutputImage = createImage(renderer->vkCore, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TYPE_2D, renderer->vkCore.extent.width, renderer->vkCore.extent.height, VK_IMAGE_ASPECT_COLOR_BIT);
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
    scene->totalInstanceCount += 1;
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
}

void meshGPass(RenderPass self, VkCommandBuffer cBuf)
{
    WREScene3D *scene = (WREScene3D *)self.resources[0].value.arbitrary;
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

const VkClearColorValue opclearValue = {{0.0f, 0.0f, 0.0f, 0.0f}};
const VkImageSubresourceRange cimgSRR = {
    VK_IMAGE_ASPECT_COLOR_BIT,
    0,
    VK_REMAINING_MIP_LEVELS,
    0,
    VK_REMAINING_ARRAY_LAYERS,
};
void meshBlitPass(RenderPass self, VkCommandBuffer cBuf)
{
    VkImageBlit2 meshBlit = {
        VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
        NULL,
        {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            0,
            1,
        },
        {{0, 0, 0}, {1080, 1080, 1}},
        {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            0,
            1,
        },
        {{0, 0, 0}, {1080, 1080, 1}},
    };
    VkBlitImageInfo2 blitInf = {
        VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
        NULL,
        WREOutputImage.image,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        self.resources[1].value.swapChainImage->image,
        self.resources[1].value.swapChainImage->CurrentLayout,
        1,
        &meshBlit,
        VK_FILTER_LINEAR,
    };
    vkCmdBlitImage2(cBuf, &blitInf);

    transitionLayout(cBuf, &WREOutputImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, ACCESS_TRANSFER_WRITE, VK_PIPELINE_STAGE_2_BLIT_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT);
    vkCmdClearColorImage(cBuf, WREOutputImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &opclearValue, 1, &cimgSRR);
    transitionLayout(cBuf, &WREOutputImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, ACCESS_TRANSFER_READ, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT);
}
void meshLightPass(RenderPass self, VkCommandBuffer cBuf)
{
    typedef struct
    {
        VkDeviceAddress cameraBuf;
        uint32_t lightCount;
        VkDeviceAddress lightBuffer;
        VkExtent2D screenSize;
    } pc;
    pc constants = {};
}

void meshPass(WREScene3D *scene, renderer_t *renderer)
{
    // std::string clearPassName = "MeshClear";
    // RenderPass clearPass = newPass((char *)clearPassName.c_str(), PASS_TYPE_TRANSFER);
    // clearPass.gPl = scene->gbufferPipeline;
    // addImageResource(&clearPass, &WREOutputImage, USAGE_TRANSFER_DST);
    // setExecutionCallBack(&clearPass, meshClearPass);
    // addPass(renderer->rg, &clearPass);

    std::string gpassName = "Meshpass";
    RenderPass gPass = newPass((char *)gpassName.c_str(), PASS_TYPE_GRAPHICS);
    gPass.gPl = scene->gbufferPipeline;
    addArbitraryResource(&gPass, scene);
    addImageResource(&gPass, &WREOutputImage, USAGE_COLORATTACHMENT);
    addImageResource(&gPass, &WREalbedoBuffer, USAGE_COLORATTACHMENT);
    addImageResource(&gPass, &WREnormalBuffer, USAGE_COLORATTACHMENT);
    setDepthAttachment(&WREdepthBuffer, &gPass);
    setExecutionCallBack(&gPass, meshGPass);
    addPass(renderer->rg, &gPass);

    // std::string lpassName = "MeshLightingPass";
    // RenderPass lPass = newPass((char *)lpassName.c_str(), PASS_TYPE_GRAPHICS);
    // lPass.gPl = scene->gbufferPipeline;
    // addImageResource(&lPass, &WREOutputImage, USAGE_COLORATTACHMENT);
    // addImageResource(&lPass, &WREalbedoBuffer, USAGE_SAMPLED);
    // addImageResource(&lPass, &WREnormalBuffer, USAGE_SAMPLED);
    // addArbitraryResource(&lPass, scene);
    // setExecutionCallBack(&lPass, meshLightPass);
    // addPass(renderer->rg, &lPass);

    std::string bpassName = "MeshBlit";

    RenderPass blitPass = newPass((char *)bpassName.c_str(), PASS_TYPE_BLIT);
    blitPass.gPl = scene->gbufferPipeline;
    addImageResource(&blitPass, &WREOutputImage, USAGE_TRANSFER_SRC);
    addSwapchainImageResource(&blitPass, *renderer);
    setExecutionCallBack(&blitPass, meshBlitPass);
    addPass(renderer->rg, &blitPass);
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
    auto asset = parser.loadGltf(data.get(), path, fastgltf::Options::LoadExternalBuffers | fastgltf::Options::GenerateMeshIndices | fastgltf::Options::DecomposeNodeMatrices);
    if (auto error = asset.error(); error != fastgltf::Error::None)
        return scene;

    if (asset->meshes.empty())
        return scene;
    std::vector<Texture> textures;
    for (fastgltf::Image &imageGltf : asset->images)
    {
        if (auto p = std::get_if<fastgltf::sources::URI>(&imageGltf.data))
        {
            char *texPath = (char *)malloc(sizeof(char) * (strlen(path) + p->uri.string().length()));
            sprintf(texPath, "%s%s", path, p->uri.c_str());

            if (std::filesystem::exists(texPath))
            {
                Texture tex = loadImageFromPNG(texPath, renderer);
                textures.push_back(tex);
            }
            else
            {
                textures.push_back(WREMissingTexture);
            }
        }
        else
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
        }
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
            for (fastgltf::Attribute &attrib : prim.attributes)
            {
                if (attrib.name == "COLOR_0")
                {
                    fastgltf::Accessor &colorAccessor = asset->accessors[attrib.accessorIndex];
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
                else if (attrib.name == "TEXCOORD_0")
                {
                    fastgltf::Accessor &uvAccessor = asset->accessors[attrib.accessorIndex];
                    if (uvAccessor.bufferViewIndex.has_value())
                    {
                        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(asset.get(), uvAccessor, [&](fastgltf::math::fvec2 uv, size_t idx)
                                                                                  {
                                                                    mesh.vertices[idx].uv.x = uv.x();
                                                                    mesh.vertices[idx].uv.y = -uv.y(); });
                    }
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
                    fastgltf::Texture &tex = asset->textures[mat.pbrData.baseColorTexture->textureIndex];
                    if (tex.imageIndex.has_value())
                    {
                        if (textures[tex.imageIndex.value()].img.image != WREMissingTexture.img.image && textures[tex.imageIndex.value()].img.image != WREDefaultTexture.img.image)
                            submitTexture(renderer, &textures[tex.imageIndex.value()], renderer->vkCore.linearSampler);
                        mesh.material.AlbedoMap = textures[tex.imageIndex.value()];
                    }
                }
                if (mat.normalTexture.has_value())
                {
                    fastgltf::Texture &tex = asset->textures[mat.normalTexture->textureIndex];
                    if (tex.imageIndex.has_value())
                    {
                        submitNormal(renderer, &textures[tex.imageIndex.value()], renderer->vkCore.linearSampler);
                        mesh.material.NormalMap = textures[tex.imageIndex.value()];
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