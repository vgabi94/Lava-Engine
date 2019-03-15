#include "buffers.h"

layout(constant_id = 0) const int NUM_LIGHTS = 32;

layout(std140, set = 1, binding = 0) uniform LightConsts
{
    LightSource Light[NUM_LIGHTS];
}