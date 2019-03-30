// Generates an irradiance cube from an environment map using convolution
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"

IN(0, vec3, inPos);
OUT(0, vec4, outColor);

UNIFORM0(0, sampler2D, samplerEnv);

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

DECL_IRRAD_PS;

void main()
{
	vec3 N = normalize(inPos);
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, N));
	up = cross(N, right);

	const float TWO_PI = PI * 2.0;
	const float HALF_PI = PI * 0.5;

	vec3 color = vec3(0.0);
	uint sampleCount = 0u;
	for (float phi = 0.0; phi < TWO_PI; phi += g_Irrad.deltaPhi) {
		for (float theta = 0.0; theta < HALF_PI; theta += g_Irrad.deltaTheta) {
			vec3 tempVec = cos(phi) * right + sin(phi) * up;
			vec3 sampleVector = cos(theta) * N + sin(theta) * tempVec;
			vec2 uv = SampleSphericalMap(normalize(sampleVector));
			uv.y = 1.0 - uv.y;
			color += texture(samplerEnv, uv).rgb * cos(theta) * sin(theta);
			sampleCount++;
		}
	}
	outColor = vec4(PI * color / float(sampleCount), 1.0);
}
