#include "types.h"

struct LightSource
{
    gpuVec4 position;
    gpuVec4 direction;
    // x = type of light
    // y = attenuation
    gpuVec4 info;
    // TODO
};