#pragma once
#include "MathTypes.h"
#include <vulkan\vulkan.hpp>
#include <array>

namespace Engine
{
	struct Texture;

	struct SkySettings
	{
		Vector3 color;
		unsigned int hdrTex;
		unsigned int hdrEnv;
		bool useTex;
		float exposure;
		float gamma;
		float ambient;

		void GetClearColor(float& r, float& g, float& b) const
		{
			r = color.x;
			g = color.y;
			b = color.z;
		}
	};

	struct IBLProbeInfo
	{
		Vector3 position;
		Matrix4* matrices;
	};

	struct IBLProbe
	{
		friend class World;

		const Vector3& GetPosition() const { return mPosition; }

		uint32_t GetIrradMap() const;
		uint32_t GetPrefEnvMap() const;
		uint32_t GetBrdfMap() const;
	private:
		uint32_t mResIndex;
		Vector3 mPosition;
	};
}