#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"

IN(0, vec3, inPos);
IN(1, vec3, inNormal);
IN(2, vec2, inUV);

DECL_POSITION;
DECL_OBJ_PS;

OUT(0, vec3, outWorldPos);
OUT(1, vec3, outNormal);
OUT(2, vec2, outUV);

void main() 
{
	outWorldPos = vec3(g_Obj.Model * vec4(inPos, 1.0));
	outNormal = mat3(g_Obj.Model) * inNormal;
	outUV = inUV;
	outUV.t = 1.0 - inUV.t;
	gl_Position =  g_Obj.MVP * vec4(inPos, 1.0);
}
