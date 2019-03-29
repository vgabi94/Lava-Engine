#pragma once
#include "MathTypes.h"

namespace Engine
{
	struct ObjPS
	{
		Matrix4 MVP;
		Matrix4 model;
		Vector3 eyePos;
	};

	struct SkyPS
	{
		Matrix4 ViewProj;
		float exposure;
		float gamma;
	};

	struct IrradPS
	{
		float deltaPhi;
		float deltaTheta;
	};

	struct PrenvPS
	{
		float roughness;
		unsigned int numSamples;
	};
}
