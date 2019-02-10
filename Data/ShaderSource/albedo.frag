#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"

IN(0, vec2, inTexCoord);
OUT(0, vec4, outColor);
UNIFORM0(0, sampler2D, albedoTex);

void main()
{
    vec4 color = texture(albedoTex, inTexCoord);
    outColor = vec4(color.rgb, 1.0);
}