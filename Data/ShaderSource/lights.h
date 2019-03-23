#ifndef LIGHTS_H
#define LIGHTS_H

#include "buffers.h"
#include "setslots.h"

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1

// layout(constant_id = 0) const int NUM_LIGHTS = 32;
#define MAX_NUM_LIGHTS 64 

// layout(std140, set = LIGHTSOURCE_SLOT, binding = 0) uniform LightConsts
// {
//     LightSource Light[NUM_LIGHTS];
// }

layout(std140, set = LIGHTSOURCE_SLOT, binding = 0) uniform LightSource g_LightSource[MAX_NUM_LIGHTS];

#endif