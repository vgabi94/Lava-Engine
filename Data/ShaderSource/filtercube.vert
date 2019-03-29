#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"

DECL_POSITION;
DECL_SKY_PS;

IN(0, vec3, inPos);
OUT(0, vec3, outUVW);

void main() 
{
	outUVW = inPos;
	gl_Position = g_Sky.ViewProj * vec4(inPos, 1.0);
}
