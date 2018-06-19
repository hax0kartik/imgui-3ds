// This was originally ported by carstene1ns for Nintendo Switch.
// 3ds port By Kartik(Pirater12) 

#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>
#include <vector>

#include "imgui/imgui.h"
#include "imgui/imgui_sw.h"

int main(int argc, char* argv[])
{
	u16 width = 320, height = 240;

	C3D_Tex *tex = (C3D_Tex*)malloc(sizeof(C3D_Tex));
    static const Tex3DS_SubTexture subt3x = { 512, 256, 0.0f, 1.0f, 1.0f, 0.0f };
   
    C2D_Image image = (C2D_Image){tex, &subt3x };
    C3D_TexInit(image.tex, 512, 256, GPU_RGBA8);
   // C3D_TexSetFilter(image.tex, GPU_LINEAR, GPU_LINEAR);
	C3D_TexSetFilter(image.tex, GPU_LINEAR, GPU_LINEAR);
	C3D_TexSetWrap(image.tex, GPU_REPEAT, GPU_REPEAT);
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	consoleInit(GFX_TOP, NULL);
	printf("Ho!\n");
	std::vector<uint32_t> pixel_buffer(width * height, 0);

	ImGui::CreateContext();
	//ImGui::SetMouseCursor()
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)width, (float)height);
 	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.MouseDrawCursor = true;
	imgui_sw::bind_imgui_painting();
	imgui_sw::SwOptions sw_options;
	imgui_sw::make_style_fast();

	touchPosition touch;
	while (aptMainLoop()) {
		hidScanInput();
		u32 kHeld = keysDown();
		io.DeltaTime = 1.0f / 60.0f;

		ImGui::NewFrame();
		ImGui::ShowDemoWindow(NULL);
		ImGui::Render();

		memset(io.NavInputs, 0, sizeof(io.NavInputs));
		#define MAP_BUTTON(NAV, BUTTON)       { if (kHeld & BUTTON) io.NavInputs[NAV] = 1.0f; }
		MAP_BUTTON(ImGuiNavInput_Activate,    KEY_A);
		MAP_BUTTON(ImGuiNavInput_Cancel,      KEY_B);
		MAP_BUTTON(ImGuiNavInput_Menu,        KEY_Y);
		MAP_BUTTON(ImGuiNavInput_Input,       KEY_X);
		MAP_BUTTON(ImGuiNavInput_DpadLeft,    KEY_DLEFT);
		MAP_BUTTON(ImGuiNavInput_DpadRight,   KEY_DRIGHT);
		MAP_BUTTON(ImGuiNavInput_DpadUp,      KEY_DUP);
		MAP_BUTTON(ImGuiNavInput_DpadDown,    KEY_DDOWN);
		MAP_BUTTON(ImGuiNavInput_FocusPrev,   KEY_L);
		MAP_BUTTON(ImGuiNavInput_FocusNext,   KEY_R);
		//MAP_BUTTON(ImGuiNavInput_TweakSlow,   KEY_L);
		//MAP_BUTTON(ImGuiNavInput_TweakFast,   KEY_R);
		#undef MAP_BUTTON
		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		hidTouchRead(&touch);

		printf("px:%d, py:%d\n", touch.px, touch.py);
		if(touch.px && touch.py)
		{
			io.MouseDown[0] = true;
			io.MousePos = ImVec2(touch.px, touch.py);
		}
		else
		io.MouseDown[0] = false;
		// fill gray (this could be any previous rendering)
		std::fill_n(pixel_buffer.data(), width * height, 0x19191919u);

		// overlay the GUI
		paint_imgui(pixel_buffer.data(), width, height, sw_options);

		for (u32 x = 0; x < width; x++)
        {
            for (u32 y = 0; y < height; y++)
            {
                u32 dstPos = ((((y >> 3) * (512 >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) * 4;
                u32 srcPos = (y * 320 + x) * 4;
                memcpy(&((u8*)image.tex->data)[dstPos], &((u8*)pixel_buffer.data())[srcPos], 4);
            }
        }
		// draw to screen
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32(32, 38, 100, 0xFF));
		C2D_SceneBegin(top);
		C2D_DrawImageAt(image, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
		C3D_FrameEnd(0);
	}

	imgui_sw::unbind_imgui_painting();
	ImGui::DestroyContext();
	gfxExit();

	return 0;
}
