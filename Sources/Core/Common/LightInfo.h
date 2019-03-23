#pragma once
#include "MathTypes.h"

namespace Engine
{
	enum class LightType
	{
		Directional,
		Point
	};

	struct LightInfo
	{
		float intensity;
		bool isShadowCaster;
		LightType type;
		Vector4 color;
		Vector3 position;
		Vector3 direction;
	};
}
