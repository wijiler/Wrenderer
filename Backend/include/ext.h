#ifndef EXT_H_
#define EXT_H_
#ifdef __cplusplus
extern "C"
{
#endif
#include <vulkan/vulkan.h>
    // my hacky solution for sharing extension functions between files
    PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT_;
    PFN_vkCreateShadersEXT vkCreateShadersEXT_;
    PFN_vkCmdBindShadersEXT vkCmdBindShadersEXT_;
    PFN_vkCmdSetColorBlendEnableEXT vkCmdSetColorBlendEnableEXT_;
    PFN_vkCmdSetColorWriteMaskEXT vkCmdSetColorWriteMaskEXT_;
    PFN_vkCmdSetDepthClampEnableEXT vkCmdSetDepthClampEnableEXT_;
    PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT_;
    PFN_vkCmdSetLogicOpEnableEXT vkCmdSetLogicOpEnableEXT_;
    PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamplesEXT_;
    PFN_vkCmdSetColorBlendEquationEXT vkCmdSetColorBlendEquationEXT_;
    PFN_vkCmdSetSampleMaskEXT vkCmdSetSampleMaskEXT_;
    PFN_vkCmdSetAlphaToCoverageEnableEXT vkCmdSetAlphaToCoverageEnableEXT_;
    PFN_vkCmdSetAlphaToOneEnableEXT vkCmdSetAlphaToOneEnableEXT_;
    PFN_vkCmdSetDepthClipEnableEXT vkCmdSetDepthClipEnableEXT_;
    PFN_vkCmdSetLogicOpEXT vkCmdSetLogicOpEXT_;
    PFN_vkDestroyShaderEXT vkDestroyShaderEXT_;
#ifdef __cplusplus
}
#endif
#endif