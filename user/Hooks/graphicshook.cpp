#include <includes.h>
#include <pch-il2cpp.h>
#include <il2cpp-appdata.h>
#include <helpers.h>
#include <Utils/gameutility.h>
#include <vector>

// ImGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"

// D3D11
#include <d3d11.h>
#include <dxgi.h>

// Kiero
#include <../kiero/kiero.h>

using namespace app;
using std::vector;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma region ===[Definitions]===
typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;
#pragma endregion

#pragma region  ===[Declarations]===
HMODULE baseModule;
HWND window = NULL;
Present oPresent;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;
#pragma endregion

void DrawPlayerActions(PlayerSetup* player)
{
	auto localPlayer = GetLocalPlayer();
	if (ImGui::Button("Teleport to player"))
	{
		if (localPlayer != nullptr && localPlayer->fields.isAlive && !localPlayer->fields.isSpectator && !localPlayer->fields.finished)
		{
			Rigidbody_set_position(localPlayer->fields.rb, player->fields.pos, nullptr);
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Kill"))
	{
		PlayerSetup_KillPlayer(player, KillType__Enum::Hammer, (*Vector3__TypeInfo)->static_fields->zeroVector, 0, nullptr);
	}

	if (ImGui::Button("Boost away"))
	{
		Vector3 vec = Vector3_Create(100, 100, 100);
		PlayerSetup_HitPlayer(player, vec, vec, 12, true, KillType__Enum::Hit, 0, true, false, nullptr);
	}
}

void DrawMainWindow()
{
	ImGui::Begin("BroCheat by JNNJ(competition against stikosek)");
	static bool posInit = false;

	if (!posInit)
	{
		ImGui::SetWindowSize(ImVec2(400, 600));
		posInit = true;
	}

	if (ImGui::CollapsingHeader("Information"))
	{
		auto networkManager = (*NetworkManager__TypeInfo)->static_fields->instance;

		if (networkManager != nullptr)
		{
			ImGui::Text(("Bots: " + std::to_string(networkManager->fields.botCount)).c_str());
		}
	}

	if (ImGui::CollapsingHeader("Player"))
	{
		ImGui::Checkbox("GodMode", &CheatState::godmode);

		static float x, y, z;

		ImGui::SliderFloat("x", &x, 0, 20);
		ImGui::SliderFloat("y", &y, 0, 20);
		ImGui::SliderFloat("z", &z, 0, 20);

		if (ImGui::Button("hit player"))
		{
			PlayerSetup_HitPlayer(GetLocalPlayer(), Vector3_Create(x, y, z), Vector3_Create(x, y, z), 1, false, KillType__Enum::Hit, 0, false, false, nullptr);
		}
	}
	
	if (ImGui::CollapsingHeader("Movement"))
	{
		ImGui::Checkbox("Enabled", &CheatState::speed);
		ImGui::SameLine();
		ImGui::SliderFloat("Speed", &CheatState::speedValue, 1, 20);

		ImGui::Checkbox("Fly(NoClip)", &CheatState::fly);

		ImGui::Checkbox("AirJump", &CheatState::airjump);

		ImGui::Checkbox("Infinite Superboost", &CheatState::infiniteSuperboost);
	}

	if (ImGui::CollapsingHeader("Playerlist"))
	{
		auto networkManager = (*NetworkManager__TypeInfo)->static_fields->instance;

		if (networkManager != nullptr)
		{
			ImGui::BeginChild("All Players", ImVec2(0, 0), true);

			auto list = networkManager->fields.allPlayer;

			for (int i = 0; i < List_1_PlayerSetup__get_Count(list, nullptr); i++)
			{
				auto player = List_1_PlayerSetup__get_Item(list, i, nullptr);

				if (player->fields.isSpectator)
					continue;

				if (ImGui::TreeNode(FormatPlayername(player).c_str()))
				{
					DrawPlayerActions(player);
					ImGui::TreePop();
				}
			}
			ImGui::EndChild();
		}
	}

	if (ImGui::CollapsingHeader("Checkpoints"))
	{
		PlayerSetup* localPlayer = GetLocalPlayer();

		static vector<Vector3> positions;
		static int currentItem = 0;

		if (currentItem > positions.size())
		{
			currentItem = 0;
		}

		if (ImGui::BeginListBox("Positions"))
		{
			for (int i = 0; i < positions.size(); i++)
			{
				const bool isSelected = (i == currentItem);
				auto pos = positions[i];

				if (ImGui::Selectable((string("Position ") + std::to_string(i)).c_str(), isSelected))
					currentItem = i;
				
				if(isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndListBox();
		}

		if (ImGui::Button("Save current Position"))
		{
			if (localPlayer != nullptr && localPlayer->fields.isAlive && !localPlayer->fields.isSpectator)
			{
				positions.push_back(Rigidbody_get_position(localPlayer->fields.rb, nullptr));
			}
		}

		if (ImGui::Button("Delete selected Position"))
		{
			if (localPlayer != nullptr && localPlayer->fields.isAlive && !localPlayer->fields.isSpectator && positions.size() > 0)
			{
				positions.erase(positions.begin() + currentItem);
			}
		}

		if (ImGui::Button("TP to selected Position"))
		{
			if (localPlayer != nullptr && localPlayer->fields.isAlive && !localPlayer->fields.isSpectator && positions.size() > 0)
			{
				Rigidbody_set_position(localPlayer->fields.rb, positions[currentItem], nullptr);
			}
		}
	}

	if (ImGui::CollapsingHeader("Other"))
	{
		ImGui::Checkbox("Premium", &CheatState::premium);
		if (ImGui::Button("Finish"))
		{
			auto networkManager = (*NetworkManager__TypeInfo)->static_fields->instance;

			if (networkManager != nullptr)
			{
				auto player = networkManager->fields.myPS;

				if (player != nullptr)
				{
					PlayerSetup_Finish(player, nullptr);
				}
			}
		}

		ImGui::SliderFloat("Reward multiplier", &CheatState::rewardMultiplier, 1, 10000000, "%.2f");
	}

	ImGui::End();
}

#pragma region ===[WndProc]===
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}
#pragma endregion

#pragma region ===[InitImGui]===
void InitImGui()
{
	CheatLog::LogMessage("Initializing ImGUI");

	CheatState::showMenu = true;

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Remove ConfigFile
	io.IniFilename = NULL;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.Fonts->AddFontDefault();

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}
#pragma endregion

#pragma region ===[hkPresent]===
bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	#pragma region ===[Init]===
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			// Get context
			pDevice->GetImmediateContext(&pContext);
			// Get SwapChain
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			// Get Window
			window = sd.OutputWindow;
			// Create BackBuffer
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			// Render to our own BackBuffer
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			// Release BackBuffer
			pBackBuffer->Release();
			// Get Original WindowProc
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			// Initialize ImGui
			InitImGui();
			init = true;
		}
		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}
	#pragma endregion

	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		CheatState::showMenu = !CheatState::showMenu;
	}

	if (CheatState::showMenu)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		DrawMainWindow();

		ImGui::Render();

		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	return oPresent(pSwapChain, SyncInterval, Flags);
}
#pragma endregion

HWND GetWindow()
{
	return window;
}

void HookGraphics()
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)&oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
}