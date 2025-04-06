#ifndef WREPIPELINE_H__
#define WREPIPELINE_H__

#ifdef WREUSEVULKAN
#include <backends/vulkan/pipeline.h>
typedef WREVKPipeline WREpipeline;
#endif

#endif