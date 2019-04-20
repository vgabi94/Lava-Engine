#define NK_IMPLEMENTATION
#include "UIManager.h"
#include "TextureManager.h"
#include "PipelineManager.h"
#include "MaterialManager.h"
#include <Engine\Input.h>
#include <Engine\Engine.h>
#include <Common\VertexDataTypes.h>

namespace Engine
{
	UIManager g_UIManager;

	void UIManager::Init()
	{
		nk_font_atlas_init_default(&mAtlas);
		
		nk_font_atlas_begin(&mAtlas);
		
		mFont = nk_font_atlas_add_from_file(&mAtlas, "", 16, nullptr);
		int width, height;
		
		const void* img = nk_font_atlas_bake(&mAtlas, &width, &height, NK_FONT_ATLAS_RGBA32);
		mAtlasTex = g_TextureManager.LoadTex2DFromData(img, width, height);
		
		nk_font_atlas_end(&mAtlas, nk_handle_id(mAtlasTex), nullptr);

		nk_init_default(&mUIContext, &mFont->handle);

		uint32_t white = g_TextureManager.GetColorTexture("white");
		mNullTexture.texture = nk_handle_id((int)white);
		mNullTexture.uv = nk_vec2(0, 0);

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
	}
	
	void UIManager::Destroy()
	{
		nk_font_atlas_clear(&mAtlas);
		nk_free(&mUIContext);
	}
		
	void UIManager::MirrorInput()
	{
		nk_input_begin(&mUIContext);
		constexpr int PRESSED = 2;
		constexpr int RELEASED = 1;
		
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
			int down = g_Input.mouseState[i] == PRESSED ? 1 : 0;
			if (down || g_Input.mouseState[i] == RELEASED)
				nk_input_button(&mUIContext, mouseButtonMap[i], x, y, down);
		}
		
		nk_input_scroll(&mUIContext, nk_vec2(g_Input.scrollX, g_Input.scrollY));

		nk_input_end(&mUIContext);
	}

	void UIManager::SetupDrawBuffers()
	{
		nk_buffer_init_default(&mCmds);
		nk_buffer_init_default(&mVerts);
		nk_buffer_init_default(&mIdx);

		nk_convert(&mUIContext, &mCmds, &mVerts, &mIdx, &mCfg);
		VertexUIList vertList((VertexUI*)mVerts.memory.ptr, (VertexUI*)mVerts.memory.ptr + mVerts.memory.size / sizeof(VertexUI));
		IndexList idxList((uint32_t*)mIdx.memory.ptr, (uint32_t*)mIdx.memory.ptr + mIdx.memory.size / sizeof(uint32_t));
		
		mVBO = g_BufferManager.AllocateUI(vertList, idxList);
		mIdxCount = idxList.size();
	}

	void UIManager::FreeDrawBuffers()
	{
		nk_buffer_free(&mCmds);
		nk_buffer_free(&mVerts);
		nk_buffer_free(&mIdx);

		nk_clear(&mUIContext);
		g_BufferManager.DeleteUI();
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

		vk::Buffer vbo = g_BufferManager.GetUIBuffer();
		vk::DeviceSize vertOffset = 0;
		vk::DeviceSize indOffset = mVBO.second;
		
		vk::Viewport viewport(0.f, 0.f, (float)GWINDOW_WIDTH, (float)GWINDOW_HEIGHT, 0.f, 1.f);
		cmdBuff.setViewport(0, { viewport });

		cmdBuff.bindVertexBuffers(0, 1, &vbo, &vertOffset);
		cmdBuff.bindIndexBuffer(vbo, indOffset, vk::IndexType::eUint32);
		// TODO PUSH CONSTANTS with ortho projection!
		const nk_draw_command* cmd = NULL;
		nk_draw_foreach(cmd, &mUIContext, &mCmds)
		{
			if (!cmd->elem_count) continue;

			vk::Rect2D scissor(
				{ (int)cmd->clip_rect.x, (int)cmd->clip_rect.y },
				{ (uint32_t)cmd->clip_rect.w, (uint32_t)cmd->clip_rect.h }
			);
			cmdBuff.setScissor(0, { scissor });

			mMaterial->UpdateUniform(0, cmd->texture.id);

			cmdBuff.drawIndexed(mIdxCount, 1, 0, 0, 0);
		}
	}
}

