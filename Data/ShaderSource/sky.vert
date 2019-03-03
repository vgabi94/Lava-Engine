#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"

DECL_POSITION;
DECL_SKY_GLOBALS;

IN(0, vec3, position);
OUT(0, vec3, FragPos);

void main()
{
    FragPos = position;
    // ViewProj without translation
    vec4 clipPos = ViewProj * vec4(position, 1.0);
    gl_Position = clipPos.xyww;
}