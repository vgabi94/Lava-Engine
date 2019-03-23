#ifndef BUFFERS_H
#define BUFFERS_H

#include "types.h"

struct LightSource
{
    gpuFloat4 position;
    gpuFloat4 direction;
    gpuFloat4 color;
    gpuInt2 type;
    gpuFloat2 atten;
    // TODO
};

struct FrameConsts
{
    int numLights;
    float ambientLight;
    float padding0;
    float padding1;
};

#endif