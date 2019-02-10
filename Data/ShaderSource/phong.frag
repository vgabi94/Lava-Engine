#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"

DECL_GLOBALS;

IN(0, vec3, FragPos);
IN(1, vec3, Normal);
IN(2, vec2, TexCoord);

OUT(0, vec4, finalColor);

UNIFORM0(0, sampler2D, Albedo);

void main()
{
    vec3 lightPos = vec3(-10, 0, 0);
    vec3 eyePos = EyePos;

    vec3 color = texture(Albedo, TexCoord).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float specularExponent = 64;
    float ambient = 0.5;

    vec3 viewDir = normalize(eyePos - FragPos);
    float diffuse = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
	float specular = pow(max(dot(normal, halfwayDir), 0.0), specularExponent);

    vec3 fragColor = color * (diffuse + specular + ambient);
    finalColor = vec4(fragColor, 1);
}