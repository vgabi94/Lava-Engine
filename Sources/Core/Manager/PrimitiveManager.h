#pragma once
#include <Common\VertexDataTypes.h>

#define GPrimitiveManager Engine::g_PrimitiveManager

namespace Engine
{
	class PrimitiveManager
	{
	public:
		void Init() { InitSkyBox(); }
		void Destroy() { }

		const VertexPosList& GetSkyBox() const { return m_SkyBox; };

	private:
		void InitSkyBox();

		VertexPosList m_SkyBox;
	};

	extern PrimitiveManager g_PrimitiveManager;
}