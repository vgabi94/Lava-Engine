#define NK_IMPLEMENTATION
#include "UIManager.h"
#include "TextureManager.h"
#include "PipelineManager.h"
#include "MaterialManager.h"
#include "WorldManager.h"
#include <Engine\Input.h>
#include <Engine\Engine.h>
#include <Common\VertexDataTypes.h>
#include <Common\PushConstantsStructs.h>

namespace Engine
{
	UIManager g_UIManager;

	void UIManager::Init()
	{
		uint32_t white = g_TextureManager.GetColorTexture("white");
		mNullTexture.texture = nk_handle_id((int)white);
		mNullTexture.uv = nk_vec2(0, 0);

		nk_font_atlas_init_default(&mAtlas);
		
		nk_font_atlas_begin(&mAtlas);
		
		std::string fontPath = g_EngineSettings.TexturesPath + "\\" + std::string(DEF_FONT);

		mFont = nk_font_atlas_add_from_file(&mAtlas, fontPath.c_str(), 16, nullptr);
		assert(mFont);
		int width, height;
		
		const void* img = nk_font_atlas_bake(&mAtlas, &width, &height, NK_FONT_ATLAS_RGBA32);
		mAtlasTex = g_TextureManager.LoadTex2DFromData(img, width, height);
		
		nk_font_atlas_end(&mAtlas, nk_handle_id(mAtlasTex), &mNullTexture);

		assert(nk_init_default(&mUIContext, &mFont->handle) == 1);

		mVertexLayout[0] = { NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(VertexUI, position) };
		mVertexLayout[1] = { NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(VertexUI, uv) };
		mVertexLayout[2] = { NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(VertexUI, color) };
		mVertexLayout[3] = { NK_VERTEX_LAYOUT_END };

		mCfg.shape_AA = NK_ANTI_ALIASING_ON;
		mCfg.line_AA = NK_ANTI_ALIASING_ON;
		mCfg.vertex_layout = mVertexLayout.data();
		mCfg.vertex_size = sizeof(VertexUI);
		mCfg.vertex_alignment = NK_ALIGNOF(VertexUI);
		mCfg.circle_segment_count = 22;
		mCfg.curve_segment_count = 22;
		mCfg.arc_segment_count = 22;
		mCfg.global_alpha = 1.0f;
		mCfg.null = mNullTexture;

		mMaterial = nullptr;

		CreateUIBuffer();
	}
	
	void UIManager::Destroy()
	{
		vmaDestroyBuffer(g_BufferManager.mAllocator, mBuffer, mBufferAlloc);
		vmaDestroyBuffer(g_BufferManager.mAllocator, mIndBuffer, mIndBufferAlloc);
		nk_font_atlas_clear(&mAtlas);
		nk_free(&mUIContext);
	}
		
	void UIManager::MirrorInput()
	{
		nk_input_begin(&mUIContext);
		constexpr uint8_t PRESSED = 2;
		constexpr uint8_t RELEASED = 1;
		constexpr uint8_t HOLD = 4;
		// todo complete input mirroring
		// KEYBOARD state
		for (int i = 'a'; i <= 'z'; ++i)
		{
			if (g_Input.keyState[i] == PRESSED)
			{
				nk_input_char(&mUIContext, (char)i);
			}
		}
		
		// MOUSE state
		int x = (int)g_Input.mousePositionX;
		int y = (int)g_Input.mousePositionY;
		nk_input_motion(&mUIContext, x, y);
		
		nk_buttons mouseButtonMap[3] = { NK_BUTTON_LEFT, NK_BUTTON_RIGHT, NK_BUTTON_MIDDLE };
		for (int i = 0; i < 3; ++i)
		{
			nk_input_button(&mUIContext, mouseButtonMap[i], x, y, HAS_STATE(g_Input.mouseState[i], PRESSED) || HAS_STATE(g_Input.mouseState[i], HOLD));
		}
		
		nk_input_scroll(&mUIContext, nk_vec2(g_Input.scrollX, g_Input.scrollY));

		nk_input_end(&mUIContext);
	}

	void UIManager::SetupDrawBuffers()
	{
		nk_buffer_init_default(&mCmds);
		nk_buffer_init_fixed(&mVerts, mBufferAllocInfo.pMappedData, MAX_VERTEX_BUFFER);
		nk_buffer_init_fixed(&mIdx, mIndBufferAllocInfo.pMappedData, MAX_INDEX_BUFFER);

		auto res = nk_convert(&mUIContext, &mCmds, &mVerts, &mIdx, &mCfg);
		assert(res == NK_CONVERT_SUCCESS);
	}

	void UIManager::FreeDrawBuffers()
	{
		nk_clear(&mUIContext);
		nk_buffer_free(&mCmds);
	}

	void UIManager::Update()
	{
		// TEST DEBUG
		enum { EASY, HARD };
		static int op = EASY;
		static float value = 0.6f;
		static int i = 20;
		if (nk_begin(&mUIContext, "Show", nk_rect(50, 50, 220, 220),
			NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE))
		{
			// fixed widget pixel width
			nk_layout_row_static(&mUIContext, 30, 80, 1);
			if (nk_button_label(&mUIContext, "button"))
			{
				// event handling
			}

			// fixed widget window ratio width
			nk_layout_row_dynamic(&mUIContext, 30, 2);
			if (nk_option_label(&mUIContext, "easy", op == EASY)) op = EASY;
			if (nk_option_label(&mUIContext, "hard", op == HARD)) op = HARD;

			// custom widget pixel width
			nk_layout_row_begin(&mUIContext, NK_STATIC, 30, 2);
			{
				nk_layout_row_push(&mUIContext, 50);
				nk_label(&mUIContext, "Volume:", NK_TEXT_LEFT);
				nk_layout_row_push(&mUIContext, 110);
				nk_slider_float(&mUIContext, 0, &value, 1.0f, 0.1f);
			}
			nk_layout_row_end(&mUIContext);
		}
		nk_end(&mUIContext);
	}

	void UIManager::CreateUIBuffer()
	{
		mBuffer = g_BufferManager.CreateBuffer(MAX_VERTEX_BUFFER, vk::BufferUsageFlagBits::eVertexBuffer,
			VMA_MEMORY_USAGE_CPU_TO_GPU, VMA_ALLOCATION_CREATE_MAPPED_BIT, mBufferAlloc, &mBufferAllocInfo);
		mIndBuffer = g_BufferManager.CreateBuffer(MAX_INDEX_BUFFER, vk::BufferUsageFlagBits::eIndexBuffer,
			VMA_MEMORY_USAGE_CPU_TO_GPU, VMA_ALLOCATION_CREATE_MAPPED_BIT, mIndBufferAlloc, &mIndBufferAllocInfo);
		g_vkDevice.waitIdle();
	}

	void UIManager::Draw(vk::CommandBuffer cmdBuff)
	{
		if (!mMaterial)
		{
			mMaterial = g_MaterialManager.NewMaterial("ui");
		}

		const Pipeline& pipe = PipelineOfType(mMaterial->mPipeType);
		vk::Pipeline pipeline = pipe.mPipeline;
		cmdBuff.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

		UiPS pc;
		pc.Ortho.row1 = Vector4(2.0f / (float)GWINDOW_WIDTH, 0.0f, 0.0f, 0.0f);
		pc.Ortho.row2 = Vector4(0.0f, -2.0f / (float)GWINDOW_HEIGHT, 0.0f, 0.0f);
		pc.Ortho.row3 = Vector4(0.0f, 0.0f, -1.0f, 0.0f);
		pc.Ortho.row4 = Vector4(-1.0f, 1.0f, 0.0f, 1.0f);
		cmdBuff.pushConstants(pipe.mPipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(UiPS), &pc);

		vk::DeviceSize vertOffset = 0;

		cmdBuff.bindVertexBuffers(0, 1, &mBuffer, &vertOffset);
		cmdBuff.bindIndexBuffer(mIndBuffer, 0, vk::IndexType::eUint16);
		
		vk::Viewport viewport(0.f, 0.f, (float)GWINDOW_WIDTH, (float)GWINDOW_HEIGHT, 0.f, 1.f);
		cmdBuff.setViewport(0, { viewport });

		uint32_t firstIndex = 0;
		const nk_draw_command* cmd = NULL;
		nk_draw_foreach(cmd, &mUIContext, &mCmds)
		{
			if (!cmd->elem_count) continue;

			int offX = std::max((int)cmd->clip_rect.x, 0);
			int offY = std::max((int)cmd->clip_rect.y, 0);
			uint32_t w = std::min((uint32_t)cmd->clip_rect.w, GWINDOW_WIDTH);
			uint32_t h = std::min((uint32_t)cmd->clip_rect.h, GWINDOW_HEIGHT);

			vk::Rect2D scissor(
				{ offX, offY },
				{ w, h }
			);
			cmdBuff.setScissor(0, { scissor });

			mMaterial->UpdateUniform(0, (uint32_t)cmd->texture.id);
			mMaterial->Bind(cmdBuff);

			cmdBuff.drawIndexed(cmd->elem_count, 1, firstIndex, 0, 0);
			firstIndex += cmd->elem_count;
		}
	}
}

