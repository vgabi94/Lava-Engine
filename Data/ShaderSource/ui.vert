#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"

DECL_POSITION;
DECL_UI_PS;

IN(0, vec2, position);
IN(1, vec2, texcoord);
IN(2, uvec4, color);

OUT(0, vec2, uvOut);
OUT(1, vec4, colorOut);

void main()
{
    gl_Position = g_UI.Ortho * vec4(position, 0.0, 1.0);
    gl_Position.y = -gl_Position.y;
    uvOut = texcoord;
    colorOut = vec4(color[0]/255.0, color[1]/255.0, color[2]/255.0, color[3]/255.0);
}