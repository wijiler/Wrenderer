#ifndef WREBUFFER_H__
#define WREBUFFER_H__

#ifdef WREUSEVULKAN
#include <backends/vulkan/buffer.h>
typedef WREVkBuffer WREBuffer;
#endif

#endif