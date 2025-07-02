#define STB_IMAGE_IMPLEMENTATION
#include <deps/stb/stb_image.h>
#include <stdbool.h>
#include <texturing.h>
#ifdef WREUSEVULKAN
#include <backends/vulkan/buffer.h>
#include <backends/vulkan/descriptors.h>
#include <backends/vulkan/globals.h>
#include <backends/vulkan/image.h>
#include <backends/vulkan/initialization.h>

#endif

const VkImageSubresourceLayers srrl = {
    VK_IMAGE_ASPECT_COLOR_BIT,
    0,
    0,
    1,
};

Texture tex_count = 0;

Texture upload_texture(char *path)
{
    int texWidth, texHeight, texChannels;
    // stbi_set_flip_vertically_on_load(true);
    stbi_uc *img = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
#ifdef WREUSEVULKAN
    WREVKImage vk_img = createImage(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, (VkExtent2D){texWidth, texHeight}, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    WREVkBuffer buf = createBuffer(texHeight * texWidth * 4, CPU_GPU, TRANSFER_SRC | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    pushCPUBuffer(buf, img, texHeight * texWidth * 4);
    immediateSubmitBegin();
    VkBufferImageCopy copy_region = {
        0,
        0,
        0,
        srrl,
        {0, 0, 0},
        {texWidth, texHeight, 1},
    };
    transitionImageInCmdBuf(WREinstantCommandBuffer, &vk_img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_2_SHADER_READ_BIT);
    vkCmdCopyBufferToImage(WREinstantCommandBuffer, buf.buffer, vk_img.img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);
    createImageView(&vk_img, VK_IMAGE_ASPECT_COLOR_BIT);
    transitionImageInCmdBuf(WREinstantCommandBuffer, &vk_img, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_2_SHADER_READ_BIT);
    immediateSubmitEnd();
    destroyBuffer(&buf);
    write_descriptor_set(vk_img, WREdefaultLinearSampler, tex_count);
    tex_count += 1;
#endif
    return tex_count - 1;
}
