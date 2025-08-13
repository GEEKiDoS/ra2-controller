#include "std_include.hpp"

#include <filesystem>

#include "d3d9/interface.hpp"
#include "d3d9/device.hpp"

#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

#include <d3d9.h>
#include <DisplayClass.h>
#include <TacticalClass.h>

#include <steam/steam_api.h>

#include "image_loader.hpp"

constexpr int BUILD_BAR_TOP = 228;
constexpr int BUILD_BAR_RIGHT = 21;

constexpr int BUILD_ICON_WIDTH = 60;
constexpr int BUILD_ICON_HEIGHT = 48;
constexpr int BUILD_ICON_GAP_H = 4;
constexpr int BUILD_ICON_GAP_V = 2;

#define INPUT_HANDLE(name, action_type) Input##action_type##Handle_t name; ImTextureID name##_icon; EInputActionOrigin name##_origin

struct
{
	InputActionSetHandle_t action_set;

	INPUT_HANDLE(a, DigitalAction);
	INPUT_HANDLE(b, DigitalAction);
	INPUT_HANDLE(x, DigitalAction);
	INPUT_HANDLE(y, DigitalAction);

	INPUT_HANDLE(up, DigitalAction);
	INPUT_HANDLE(down, DigitalAction);
	INPUT_HANDLE(left, DigitalAction);
	INPUT_HANDLE(right, DigitalAction);

	INPUT_HANDLE(lb, DigitalAction);
	INPUT_HANDLE(rb, DigitalAction);

	INPUT_HANDLE(lt, DigitalAction);
	INPUT_HANDLE(rt, DigitalAction);

	INPUT_HANDLE(ls, DigitalAction);
	INPUT_HANDLE(rs, DigitalAction);

	INPUT_HANDLE(select, DigitalAction);
	INPUT_HANDLE(start, DigitalAction);

	INPUT_HANDLE(map_scroll, AnalogAction);
	INPUT_HANDLE(cursor, AnalogAction);

	InputHandle_t controllers[STEAM_INPUT_MAX_COUNT];
} input_context;

bool init_steam_input()
{
	printf("init steam\n");

	if (!std::filesystem::exists("steam_appid.txt"))
	{
		std::ofstream steam_appid{ "steam_appid.txt" };
		steam_appid << "480";
	}

	SteamErrMsg errMsg;
	if (SteamAPI_InitEx(&errMsg) || !SteamAPI_IsSteamRunning())
	{
		MessageBoxA(nullptr, errMsg, "Steam API Error", MB_ICONERROR);
		return false;
	}

	printf("init steam input\n");

	auto* input = SteamInput();
	if (!input->Init(true))
	{
		MessageBoxA(nullptr, "Failed to initialize Steam Input", "Steam API Error", MB_ICONERROR);

		SteamAPI_Shutdown();
		return false;
	}

	Sleep(1000);
	input->RunFrame();

	memset(&input_context, 0, sizeof(input_context));

	input_context.action_set = input->GetActionSetHandle("ra2_control");

	input_context.a = input->GetDigitalActionHandle("btn_a");
	input_context.b = input->GetDigitalActionHandle("btn_b");
	input_context.x = input->GetDigitalActionHandle("btn_x");
	input_context.y = input->GetDigitalActionHandle("btn_y");

	input_context.up = input->GetDigitalActionHandle("btn_dpad_up");
	input_context.down = input->GetDigitalActionHandle("btn_dpad_down");
	input_context.left = input->GetDigitalActionHandle("btn_dpad_left");
	input_context.right = input->GetDigitalActionHandle("btn_dpad_right");

	input_context.lb = input->GetDigitalActionHandle("btn_lb");
	input_context.rb = input->GetDigitalActionHandle("btn_rb");

	input_context.lt = input->GetDigitalActionHandle("btn_lt");
	input_context.rt = input->GetDigitalActionHandle("btn_rt");

	input_context.ls = input->GetDigitalActionHandle("btn_lstick");
	input_context.rs = input->GetDigitalActionHandle("btn_rstick");

	input_context.start = input->GetDigitalActionHandle("btn_start");
	input_context.select = input->GetDigitalActionHandle("btn_select");

	input_context.map_scroll = input->GetAnalogActionHandle("map_scroll");
	input_context.cursor = input->GetAnalogActionHandle("cursor");

#define PRINT_HANDLE(_HANDLE) printf(#_HANDLE": %llu\n", input_context._HANDLE)

	PRINT_HANDLE(action_set);
	PRINT_HANDLE(a);
	PRINT_HANDLE(b);
	PRINT_HANDLE(x);
	PRINT_HANDLE(y);
	PRINT_HANDLE(up);
	PRINT_HANDLE(down);
	PRINT_HANDLE(left);
	PRINT_HANDLE(right);
	PRINT_HANDLE(lb);
	PRINT_HANDLE(rb);
	PRINT_HANDLE(lt);
	PRINT_HANDLE(rt);
	PRINT_HANDLE(ls);
	PRINT_HANDLE(rs);
	PRINT_HANDLE(start);
	PRINT_HANDLE(select);
	PRINT_HANDLE(map_scroll);
	PRINT_HANDLE(cursor);

	return true;
}

InputHandle_t update_steam_input_controller()
{
	auto input = SteamInput();
	if (!input->GetConnectedControllers(input_context.controllers))
	{
		// controller not connected
		return 0;
	}

	const auto controller = input_context.controllers[0];

	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS];

#define UPDATE_ICON(name, action_type) \
	{ \
		auto n = SteamInput()->GetDigitalActionOrigins(controller, input_context.action_set, input_context.##name, origins); \
		if (n) \
		{ \
			if (origins[0] != input_context.##name##_origin) \
			{ \
				printf(#name" origin change from %d to %d\n", input_context.##name##_origin, origins[0]); \
				if (input_context.##name##_icon) \
				{ \
					reinterpret_cast<IDirect3DTexture9*>(input_context.##name##_icon)->Release(); \
				} \
					\
					const auto* png_path = SteamInput()->GetGlyphPNGForActionOrigin(origins[0], k_ESteamInputGlyphSize_Small, 0); \
					input_context.##name##_icon = load_texture_file(png_path); \
					input_context.##name##_origin = origins[0]; \
			} \
		}\
	}

	UPDATE_ICON(a, DigitalAction);
	UPDATE_ICON(b, DigitalAction);
	UPDATE_ICON(x, DigitalAction);
	UPDATE_ICON(y, DigitalAction);

	UPDATE_ICON(up, DigitalAction);
	UPDATE_ICON(down, DigitalAction);
	UPDATE_ICON(left, DigitalAction);
	UPDATE_ICON(right, DigitalAction);

	UPDATE_ICON(lb, DigitalAction);
	UPDATE_ICON(rb, DigitalAction);

	UPDATE_ICON(lt, DigitalAction);
	UPDATE_ICON(rt, DigitalAction);

	UPDATE_ICON(ls, DigitalAction);
	UPDATE_ICON(rs, DigitalAction);

	UPDATE_ICON(select, DigitalAction);
	UPDATE_ICON(start, DigitalAction);

	UPDATE_ICON(map_scroll, AnalogAction);
	UPDATE_ICON(cursor, AnalogAction);

	return controller;
}

extern "C" IDirect3D9* WINAPI create_d3d9(UINT sdk_version)
{
	static auto d3d9_create = ([] -> decltype(Direct3DCreate9)* {
		//auto d3d9 = LoadLibraryA("ext-ms-win-dx-d3d9-l1-1-0.dll");
		auto d3d9 = LoadLibraryA("dxvk.dll");

		char FN[MAX_PATH];
		GetModuleFileNameA(d3d9, FN, MAX_PATH);
		printf("%s\n", FN);

		if (!d3d9) return nullptr;

		return reinterpret_cast<decltype(Direct3DCreate9)*>(GetProcAddress(d3d9, "Direct3DCreate9"));
	})();

	if (!d3d9_create)
	{
		printf("fucked 1\n");
		return nullptr;
	}

	auto result = d3d9_create(sdk_version);
	if (result)
	{
		printf("good\n");
		return new d3d9_proxy(result);
	}

	printf("fucked 2\n");
	return nullptr;
}

void create_console()
{
	AllocConsole();

	FILE* f;
	auto _ = freopen_s(&f, "CONOUT$", "w+t", stdout);
	_ = freopen_s(&f, "CONOUT$", "w", stderr);
	_ = freopen_s(&f, "CONIN$", "r", stdin);
}

std::string debug_texts;

void draw_debug_window()
{
	ImGui::SetNextWindowPos(ImVec2{ 100, 900 });
	ImGui::Begin("test", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
	{
		ImGui::Image(input_context.a_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.b_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.x_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.y_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.up_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.down_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.left_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.right_icon, ImVec2(32, 32));
		
		ImGui::Image(input_context.lb_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.rb_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.lt_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.rt_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.ls_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.rs_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.start_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.select_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.map_scroll_icon, ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Image(input_context.cursor_icon, ImVec2(32, 32));

		ImGui::Text(debug_texts.data());
	}
	ImGui::End();

	debug_texts.clear();
}

void process_map_scroll(const InputHandle_t controller)
{
	auto* display = &DisplayClass::Instance;

	auto data = SteamInput()->GetAnalogActionData(controller, input_context.map_scroll);
	if (!data.bActive)
	{
		debug_texts += std::format("map_scroll ({}): inactive\n", uint64(input_context.map_scroll));
		return;
	}

	debug_texts += std::format("map_scroll: x={} y={}\n", data.x, data.y);

	constexpr auto SCROLL_SPEED = 20;

	auto west = std::max(-data.x, 0.f);
	auto east = std::max(data.x, 0.f);
	auto north = std::max(data.y, 0.f);
	auto south = std::max(-data.y, 0.f);
	
	if (east >= 0.001)
	{
		int dist = east * SCROLL_SPEED;
		display->ScrollMap(FacingType::East, dist, true);
	}
	if (west >= 0.001)
	{
		int dist = west * SCROLL_SPEED;
		display->ScrollMap(FacingType::West, dist, true);
	}
	if (north >= 0.001)
	{
		int dist = north * SCROLL_SPEED;
		display->ScrollMap(FacingType::North, dist, true);
	}
	if (south >= 0.001)
	{
		int dist = south * SCROLL_SPEED;
		display->ScrollMap(FacingType::South, dist, true);
	}
}

void process_cursor(const InputHandle_t controller)
{
	auto data = SteamInput()->GetAnalogActionData(controller, input_context.cursor);
	if (!data.bActive)
	{
		debug_texts += std::format("cursor ({}): inactive\n", uint64(input_context.cursor));
		return;
	}

	debug_texts += std::format("cursor: x={} y={}\n", data.x, data.y);

	constexpr auto MOUSE_SPEED = 25.f;

	POINT pos;
	GetCursorPos(&pos);
	SetCursorPos(int(float(pos.x) + data.x * MOUSE_SPEED), int(float(pos.y) - data.y * MOUSE_SPEED));
}

void process_input()
{
	SteamInput()->RunFrame();

	if (!update_steam_input_controller())
	{
		return;
	}

	// always use controller 0... for now
	const auto controller = input_context.controllers[0];

	auto type = SteamInput()->GetInputTypeForHandle(controller);
	debug_texts += std::format("controller type: {}\n", int(type));

	SteamInput()->ActivateActionSet(controller, input_context.action_set);

	process_map_scroll(controller);
	process_cursor(controller);
}

void on_present(d3d9_device_proxy* device)
{
	process_input();

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	draw_debug_window();

	ImGui::EndFrame();

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}
}

void init_imgui(d3d9_device_proxy* device)
{
	if (!init_steam_input())
		return;

	device->on_present(on_present);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
	io.IniFilename = nullptr;


	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(device->get_hwnd());
	ImGui_ImplDX9_Init(device);

	io.Fonts->AddFontDefault();
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		create_console();
		d3d9_device_proxy::on_create(init_imgui);
	}

	return TRUE;
}
