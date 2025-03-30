#include <backends/vulkan/globals.h>
#include <backends/vulkan/shader.h>
#include <stdio.h>

WREShader createShader(char *Filename, WREshaderStage shaderStage)
{
    WREShader shader = {0};
    shader.shaderStage = shaderStage;
    shader.shaderType = WRE_SHADER_TYPE_FIXED_FUNCTION;

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
    vkCreateShaderModule(WREDevice, &shaderCI, NULL, &shader.shaderObjects.Shader);

    if ((shaderStage & WRE_SHADER_STAGE_COMPUTE) != 0)
    {
        VkPipelineShaderStageCreateInfo cshad = {0};
        cshad.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        cshad.pNext = NULL;
        cshad.stage = VK_SHADER_STAGE_COMPUTE_BIT;

        cshad.pName = "compMain";
        cshad.module = shader.shaderObjects.Shader;
        shader.shaderObjects.stageInfo[0] = cshad;
        return shader;
    }
    if ((shaderStage & WRE_SHADER_STAGE_VERTEX) != 0)
    {
        VkPipelineShaderStageCreateInfo vshad = {0};
        vshad.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vshad.pNext = NULL;
        vshad.stage = VK_SHADER_STAGE_VERTEX_BIT;

        vshad.pName = "vertMain";
        vshad.module = shader.shaderObjects.Shader;
        shader.shaderObjects.stageInfo[0] = vshad;
    }
    if ((shaderStage & WRE_SHADER_STAGE_FRAGMENT) != 0)
    {
        VkPipelineShaderStageCreateInfo fshad = {0};
        fshad.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fshad.pNext = NULL;
        fshad.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

        fshad.pName = "fragMain";
        fshad.module = shader.shaderObjects.Shader;
        shader.shaderObjects.stageInfo[1] = fshad;
    }

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
