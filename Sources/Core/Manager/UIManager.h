#pragma once

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
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

		void Update();

	private:
		void CreateUIBuffer();

		static constexpr const char* DEF_FONT = "Roboto-Regular.ttf";
		static constexpr size_t MAX_VERTEX_BUFFER = 512 * 1024;
		static constexpr size_t MAX_INDEX_BUFFER = 128 * 1024;

		nk_context mUIContext;
		nk_font* mFont;
		nk_font_atlas mAtlas;
		uint32_t mAtlasTex;

		nk_draw_null_texture mNullTexture;
		nk_buffer mCmds, mVerts, mIdx;
		nk_convert_config mCfg;
		std::array<nk_draw_vertex_layout_element, 4> mVertexLayout;
		uint32_t mIdxCount;
		Material* mMaterial;

		vk::Buffer mBuffer;
		vk::Buffer mIndBuffer;
		VmaAllocation mBufferAlloc;
		VmaAllocation mIndBufferAlloc;
		VmaAllocationInfo mBufferAllocInfo;
		VmaAllocationInfo mIndBufferAllocInfo;
	};

	extern UIManager g_UIManager;
}