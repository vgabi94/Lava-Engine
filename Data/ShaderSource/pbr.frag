#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "common.h"
#include "pbrutil.h"
#include "lights.h"
#include "globalbuffers.h"
#include "util.h"

IN(0, vec3, inWorldPos);
IN(1, vec3, inNormal);
IN(2, vec2, inUV);

DECL_OBJ_PS;

UNIFORM0(0, samplerCube, samplerIrradiance);
UNIFORM0(1, sampler2D, samplerBRDFLUT);
UNIFORM0(2, samplerCube, prefilteredMap);

UNIFORM0(3, sampler2D, albedoMap);
UNIFORM0(4, sampler2D, normalMap);
UNIFORM0(5, sampler2D, aoMap);
UNIFORM0(6, sampler2D, metallicMap);
UNIFORM0(7, sampler2D, roughnessMap);

OUT(0, vec4, outColor);

#define ALBEDO pow(texture(albedoMap, inUV).rgb, vec3(2.2))

vec3 prefilteredReflection(vec3 R, float roughness)
{
	const float MAX_REFLECTION_LOD = 9.0; // todo: param/const
	float lod = roughness * MAX_REFLECTION_LOD;
	float lodf = floor(lod);
	float lodc = ceil(lod);
	vec3 a = textureLod(prefilteredMap, R, lodf).rgb;
	vec3 b = textureLod(prefilteredMap, R, lodc).rgb;
	return mix(a, b, lod - lodf);
}

vec3 specularContribution(vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness, vec3 lightColor)
{
	// Precalculate vectors and dot products	
	vec3 H = normalize (V + L);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);

	vec3 color = vec3(0.0);

	if (dotNL > 0.0) {
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness); 
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		vec3 F = F_Schlick(dotNV, F0);		
		vec3 spec = D * F * G / (4.0 * dotNL * dotNV + 0.001);		
		vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);			
		color += (kD * ALBEDO / PI + spec) * dotNL;
	}

	return color;
}

// See http://www.thetenthplanet.de/archives/1180
vec3 perturbNormal()
{
	vec3 tangentNormal = texture(normalMap, inUV).xyz * 2.0 - 1.0;

	vec3 q1 = dFdx(inWorldPos);
	vec3 q2 = dFdy(inWorldPos);
	vec2 st1 = dFdx(inUV);
	vec2 st2 = dFdy(inUV);

	vec3 N = normalize(inNormal);
	vec3 T = normalize(q1 * st2.t - q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

void main()
{		
	vec3 N = perturbNormal();
	vec3 V = normalize(g_Obj.EyePos - inWorldPos);
	vec3 R = reflect(-V, N); 

	float metallic = texture(metallicMap, inUV).r;
	float roughness = texture(roughnessMap, inUV).r;

	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, ALBEDO, metallic);

	vec3 Lo = vec3(0.0); 
	for (int i = 0; i < g_FrameConsts.numLights; ++i)
    {
        vec3 L;
        if (g_LightSource[i].type.x == DIRECTIONAL_LIGHT)
        {
            L = normalize(g_LightSource[i].direction.xyz);
        }
        else
        {
            L = normalize(g_LightSource[i].position.xyz - inWorldPos);
        }
		Lo += specularContribution(L, V, N, F0, metallic, roughness, g_LightSource[i].color);
	}
	
	vec2 brdf = texture(samplerBRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 reflection = prefilteredReflection(R, roughness).rgb;	
	vec3 irradiance = texture(samplerIrradiance, N).rgb;

	// Diffuse based on irradiance
	vec3 diffuse = irradiance * ALBEDO;	

	vec3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

	// Specular reflectance
	vec3 specular = reflection * (F * brdf.x + brdf.y);

	// Ambient part
	vec3 kD = 1.0 - F;
	kD *= 1.0 - metallic;	  
	vec3 ambient = (kD * diffuse + specular) * texture(aoMap, inUV).rrr;
	
	vec3 color = ambient + Lo;
	color = ApplyToneMapping(color, g_FrameConsts.exposure);	
	color = ApplyGammaCorrection(color, g_FrameConsts.gamma);

	outColor = vec4(color, 1.0);
}