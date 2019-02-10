#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"

DECL_POSITION;
DECL_GLOBALS;

IN(0, vec3, position);
IN(1, vec3, normal);
IN(2, vec2, texcoord);

OUT(0, vec3, FragPos);
OUT(1, vec3, Normal);
OUT(2, vec2, TexCoord);

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    FragPos = (Model * vec4(position, 1.0)).xyz;
    Normal = (Model * vec4(normal, 0)).xyz; // because we have uniform scaling and it doesn't affect normals
    TexCoord = texcoord;
}