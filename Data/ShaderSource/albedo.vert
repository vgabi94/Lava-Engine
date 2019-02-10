#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"

DECL_POSITION;
DECL_GLOBALS;

IN(0, vec2, position);
IN(1, vec2, texcoord);

OUT(0, vec2, outTexCoord);

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    outTexCoord = texcoord;
}