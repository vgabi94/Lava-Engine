#pragma once

#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_UINT_DRAW_INDEX
#define NK_BUTTON_TRIGGER_ON_RELEASE
#include <nuklear.h>
#include <vulkan\vulkan.hpp>
#include "BufferManager.h"
#include <Engine\Material.h>

namespace Engine
{
	class UIManager
	{
	public:
		void Init();
		void Destroy();

		void MirrorInput();
		void SetupDrawBuffers();
		void Draw(vk::CommandBuffer cmdBuf);
		void FreeDrawBuffers();

	private:
		static constexpr const char* DEF_FONT = "Roboto-Regular.ttf";

		nk_context mUIContext;
		nk_font* mFont;
		nk_font_atlas mAtlas;
		uint32_t mAtlasTex;

		nk_draw_null_texture mNullTexture;
		nk_buffer mCmds, mVerts, mIdx;
		nk_convert_config mCfg;
		std::array<nk_draw_vertex_layout_element, 4> mVertexLayout;
		IndexOffset mVBO;
		uint32_t mIdxCount;
		Material* mMaterial;
	};

	extern UIManager g_UIManager;
}