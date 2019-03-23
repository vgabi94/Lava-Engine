#pragma once
#include "MathTypes.h"

namespace Engine
{
	struct DefaultPS
	{
		Matrix4 MVP;
		Matrix4 model;
		Vector3 eyePos;
		int numLights;
	};

	struct SkyPS
	{
		Matrix4 ViewProj;
		float exposure;
		float gamma;
	};
}
