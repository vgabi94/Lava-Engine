#ifndef UTIL_H
#define UTIL_H

// From http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 Uncharted2Tonemap(vec3 color)
{
	const float A = 0.15;
	const float B = 0.50;
	const float C = 0.10;
	const float D = 0.20;
	const float E = 0.02;
	const float F = 0.30;
	const float W = 11.2;
	return ((color*(A*color+C*B)+D*E)/(color*(A*color+B)+D*F))-E/F;
}

vec3 ApplyToneMapping(vec3 color, float exposure)
{
    color = Uncharted2Tonemap(color * exposure);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));
    return color;
}

vec3 ApplyGammaCorrection(vec3 color, float gamma)
{
    return pow(color, vec3(1.0f / gamma));
}

#endif