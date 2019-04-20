#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"

IN(0, vec2, uv);
IN(1, vec4, inColor);

OUT(0, vec4, outColor);

// this could be the null (white) texture or something custom
UNIFORM0(0, sampler2D, currentTexture);

void main()
{
    vec4 uiCol = texture(currentTexture, uv);
    outColor = inColor * uiCol;
}