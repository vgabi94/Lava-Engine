#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"

DECL_POSITION;
DECL_UI_PS;

IN(0, vec2, position);
IN(1, vec2, texcoord);
IN(2, vec4, color);

OUT(0, vec2, uvOut);
OUT(1, vec4, colorOut);

void main()
{
    gl_Position = g_UI.Ortho * vec4(position, 0.0, 1.0);
    uvOut = texcoord;
    colorOut = color;
}