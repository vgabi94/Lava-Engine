#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"

IN(0, vec3, fragColor);

OUT(0, vec4, outColor);

void main()
{
    outColor = vec4(fragColor, 1.0);
}