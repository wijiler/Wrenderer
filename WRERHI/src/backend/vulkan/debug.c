#include <backends/vulkan/debug.h>
#include <backends/vulkan/globals.h>

PFN_vkSetDebugUtilsObjectNameEXT _vkSetDebugUtilsObjectNameEXT;

void setVkDebugName(char *name, VkObjectType objectType, uint64_t handle)
{
#ifdef DEBUG
    if (_vkSetDebugUtilsObjectNameEXT == NULL)
    {
        _vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(WREvulkInstance, "vkSetDebugUtilsObjectNameEXT");
    }
    VkDebugUtilsObjectNameInfoEXT nameInf = {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        NULL,
        objectType,
        handle,
        name,
    };
    _vkSetDebugUtilsObjectNameEXT(WREdevice, &nameInf);
#endif
}