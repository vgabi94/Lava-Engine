#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"
#include "lights.h"
#include "globalbuffers.h"

DECL_GLOBALS;

IN(0, vec3, FragPos);
IN(1, vec3, Normal);
IN(2, vec2, TexCoord);

OUT(0, vec4, finalColor);

UNIFORM0(0, sampler2D, Albedo);

void main()
{
    const float specularExponent = 64;
    const float ambient = 0.5;

    vec3 eyePos = EyePos;
    vec3 color = texture(Albedo, TexCoord).rgb;
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(eyePos - FragPos);
    vec4 lightColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < g_FrameConsts.numLights; ++i)
    {
        vec3 lightDir;
        if (g_LightSource[i].type.x == DIRECTIONAL_LIGHT)
        {
            lightDir = normalize(g_LightSource[i].direction.xyz);
        }
        else
        {
            lightDir = normalize(g_LightSource[i].position.xyz - FragPos);
        }

        float diffuse = max(dot(normal, lightDir), 0.0);
        vec3 halfwayDir = normalize(lightDir + viewDir);
	    float specular = pow(max(dot(normal, halfwayDir), 0.0), specularExponent);

        lightColor += (diffuse + specular) * 1/*intensity*/ * 1/*atten*/ * g_LightSource[i].color;
    }
    
    vec3 fragColor = color * (g_FrameConsts.ambientLight * vec3(1,1,1) + lightColor.rgb);
    finalColor = vec4(fragColor, 1);
}