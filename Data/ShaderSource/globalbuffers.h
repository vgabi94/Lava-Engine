#ifndef GLOBALBUFFERS_H
#define GLOBALBUFFERS_H

#include "buffers.h"
#include "setslots.h"

layout(std140, set = FRAMECONSTS_SLOT, binding = 0) uniform FrameConstsBlock
{ 
    FrameConsts g_FrameConsts;
};

#endif