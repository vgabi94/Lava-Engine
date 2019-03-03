#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"

DECL_SKY_GLOBALS;

IN(0, vec3, FragPos);

OUT(0, vec4, finalColor);

UNIFORM0(0, sampler2D, skyTex);

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(FragPos));
    vec3 color = texture(skyTex, uv).rgb;
    
    color = ApplyToneMapping(color, Exposure);
	color = ApplyGammaCorrection(color, Gamma);
	
    finalColor = vec4(color, 1.0);
}