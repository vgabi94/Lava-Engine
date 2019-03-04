#include "PrimitiveManager.h"

namespace Engine
{
	PrimitiveManager g_PrimitiveManager;

	void PrimitiveManager::InitSkyBox()
	{
		m_SkyBox =
		{
			VertexPos(-1.0f,  1.0f, -1.0f),
			VertexPos(-1.0f, -1.0f, -1.0f),
			VertexPos(1.0f, -1.0f, -1.0f),
			VertexPos(1.0f, -1.0f, -1.0f),
			VertexPos(1.0f,  1.0f, -1.0f),
			VertexPos(-1.0f,  1.0f, -1.0f),

			VertexPos(-1.0f, -1.0f,  1.0f),
			VertexPos(-1.0f, -1.0f, -1.0f),
			VertexPos(-1.0f,  1.0f, -1.0f),
			VertexPos(-1.0f,  1.0f, -1.0f),
			VertexPos(-1.0f,  1.0f,  1.0f),
			VertexPos(-1.0f, -1.0f,  1.0f),
									   
			VertexPos(1.0f, -1.0f, -1.0f),
			VertexPos(1.0f, -1.0f,  1.0f),
			VertexPos(1.0f,  1.0f,  1.0f),
			VertexPos(1.0f,  1.0f,  1.0f),
			VertexPos(1.0f,  1.0f, -1.0f),
			VertexPos(1.0f, -1.0f, -1.0f),

			VertexPos(-1.0f, -1.0f,  1.0f),
			VertexPos(-1.0f,  1.0f,  1.0f),
			VertexPos(1.0f,  1.0f,  1.0f),
			VertexPos(1.0f,  1.0f,  1.0f),
			VertexPos(1.0f, -1.0f,  1.0f),
			VertexPos(-1.0f, -1.0f,  1.0f),

			VertexPos(-1.0f,  1.0f, -1.0f),
			VertexPos(1.0f,  1.0f, -1.0f),
			VertexPos(1.0f,  1.0f,  1.0f),
			VertexPos(1.0f,  1.0f,  1.0f),
			VertexPos(-1.0f,  1.0f,  1.0f),
			VertexPos(-1.0f,  1.0f, -1.0f),

			VertexPos(-1.0f, -1.0f, -1.0f),
			VertexPos(-1.0f, -1.0f,  1.0f),
			VertexPos(1.0f, -1.0f, -1.0f),
			VertexPos(1.0f, -1.0f, -1.0f),
			VertexPos(-1.0f, -1.0f,  1.0f),
			VertexPos(1.0f, -1.0f,  1.0f)
		};
	}
}
