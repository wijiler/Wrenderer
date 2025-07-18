#include <backends/vulkan/debug.h>
#include <backends/vulkan/globals.h>
#include <backends/vulkan/shader.h>
#include <stdio.h>

WREshader createShader(char *Filename, WREshaderStage shaderStage)
{
    WREshader shader = {0};
    shader.shaderStage = shaderStage;

    FILE *file;
    uint32_t len;
    char *data;
    errno_t err = fopen_s(&file, Filename, "rb");
    if (err != 0)
    {
        printf("WRERen ERROR: Could not open file %s\n", Filename);
    }

    fseek(file, 0, SEEK_END);
    unsigned long file_size = ftell(file);
    rewind(file);

    data = malloc(file_size + 1);
    if (!data)
    {
        printf("WRERen ERROR: Memory allocation failed, your shader is somehow bigger than your mom.\n");
        fclose(file);
    }

    len = fread(data, 1, file_size, file);
    data[len] = '\0';

    if (len != file_size)
    {
        printf("WRERen Error: Could not read data for shader %s\n", Filename);
        free(data);
        fclose(file);
    }

    fclose(file);

    VkShaderModuleCreateInfo shaderCI = {0};
    shaderCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCI.codeSize = len;
    shaderCI.pCode = (uint32_t *)data;
    VkResult result = vkCreateShaderModule(WREdevice, &shaderCI, NULL, &shader.shaderObjects.Shader);
    if (result != VK_SUCCESS)
    {
        printf("WRERen: Error: Could not create shader %s\n", Filename);
        exit(1);
    }

    setVkDebugName(Filename, VK_OBJECT_TYPE_SHADER_MODULE, (uint64_t)shader.shaderObjects.Shader);

    return shader;
}

void addShaderBinding(WREvertexFormat *format, WREshaderBinding binding)
{
    if (format->bindingCount % 100 == 0)
    {
        format->bindings = realloc(format->bindings, sizeof(WREshaderBinding) * (format->bindingCount + 100));
        format->bindings[format->bindingCount] = (VkVertexInputBindingDescription){
            binding.binding,
            binding.elementSize,
            (VkVertexInputRate)binding.inputRate,
        };
        format->bindingCount += 1;
    }
}
void addShaderAttrib(WREvertexFormat *format, WREshaderAttribute attrib)
{
    if (format->bindingCount % 100 == 0)
    {
        format->attributes = realloc(format->attributes, sizeof(WREshaderBinding) * (format->attribCount + 100));
        format->attributes[format->attribCount] = (VkVertexInputAttributeDescription){
            attrib.slot,
            attrib.binding,
            attrib.format,
            attrib.offset,
        };
        format->attribCount += 1;
    }
}

void setPushConstants(WREshader *shader, void *pushConstants, size_t pcSize)
{
    if (pcSize > 128)
    {
        printf("WRERen Error: pushconstants size greater than 128 bytes, exiting\n");
        exit(-1);
        return;
    }
    shader->pushConstantsSize = pcSize;
    shader->pushconstants = malloc(128);
    memcpy(shader->pushconstants, pushConstants, pcSize);
}

void addShaderDescriptor(WREshader *shader, WREVKDescriptorSet set)
{
    shader->sets = realloc(shader->sets, sizeof(VkDescriptorSetLayout) * (shader->descriptorSetCount + 1));
    shader->sets[shader->descriptorSetCount] = set.layout;
    shader->descriptorSetCount += 1;
}