#pragma comment(lib, "user32")				

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>						
#include <d3dcompiler.h>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>

#include "ImGui/imgui.h"				
#include "ImGui/imgui_internal.h"		
#include "ImGui/imgui_impl_dx11.h"		
#include "ImGui/imgui_impl_win32.h"		

#include "Vector.h"
#include "Renderer.h"
#include "UObject.h"
#include "Global.h"
#include "TemplateLibrary.h"
#include "CollisionManager.h"
#include "ObjectManager.h"

bool bUseGravity = true;

FVector ScreenToWorld(HWND hwnd, int MouseX, int MouseY)
{
	RECT rec;
	GetClientRect(hwnd, &rec);

	float width = (float)(rec.right - rec.left);
	float height = (float)(rec.bottom - rec.top);

	if (width <= 0.0f || height <= 0.0f)
	{
		return FVector(0.0f, 0.0f, 0.0f);
	}

	float aspect = width / height;

	float worldX = (2.0f * (float)MouseX / width - 1.0f) * aspect;
	float worldY = 1.0f - (2.0f * (float)MouseY / height);

	return FVector(worldX, worldY, 0.0f);
}

ImVec2 WorldToScreen(const FVector& World)
{
	ImGuiIO& io = ImGui::GetIO();

	float width = io.DisplaySize.x;
	float height = io.DisplaySize.y;

	if (width <= 0.0f || height <= 0.0f)
	{
		return ImVec2(0.0f, 0.0f);
	}

	float aspect = width / height;

	float screenX = (World.x / aspect + 1.0f) * 0.5f * width;
	float screenY = (1.0f - World.y) * 0.5f * height;

	return ImVec2(screenX, screenY);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	WCHAR WindowClass[] = L"Jungle3DEngineClass";
	WCHAR Title[] = L"3D Engine Base";

	WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };
	RegisterClassW(&wndclass);

	int windowWidth = 1920;
	int windowHeight = 1080;

	HWND hWnd = CreateWindowExW(0, WindowClass, Title,
		WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);

	URenderer& renderer = URenderer::GetInstance();
	renderer.Create(hWnd);
	renderer.CreateShader();
	renderer.CreateConstantBuffer();
	renderer.CreateVertexBufferInfos();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init((void*)hWnd);
	ImGui_ImplDX11_Init(renderer.Device, renderer.DeviceContext);

	const int targetFPS = 144;
	const double targetFrameTime = 1000.0 / targetFPS;
	const double fixedDeltaTime = 1.0 / 144.0;
	const double maxAccumulated = fixedDeltaTime * 5.0;
	double accumulator = 0.0;

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER startTime, endTime;
	double elapsedTime = 0.0;
	float deltaTime = 0.0f;

	bool bIsExit = false;
	bool bPressed = false;
	int MouseX = 0, MouseY = 0;
	FVector WorldMouseXY;
	ACollider* PressedCollider = nullptr;

	bool bPausePhysics = false;
	bool bStepOnce = false;
	bool bDrawContacts = true;
	bool bDrawColliders = true;
	float NormalLength = 40.0f;

	UObjectManager& ObjectManager = UObjectManager::GetInstance();
	CollisionManager& CM = CollisionManager::GetInstance();

	float aspect = (float)windowWidth / (float)windowHeight;

	float SpawnWidth = 0.2f;
	float SpawnHeight = 0.2f;
	float SpawnMass = 10.0f;
	int SelectedPrimitive = 0;
	const char* const PrimitiveItems[] = { "Rectangle", "Circle" };

	while (!bIsExit)
	{
		QueryPerformanceCounter(&startTime);

		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bIsExit = true;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			bIsExit = true;
		}

		if (!io.WantCaptureMouse)
		{
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(hWnd, &pt);
				MouseX = pt.x;
				MouseY = pt.y;
				WorldMouseXY = ScreenToWorld(hWnd, MouseX, MouseY);

				if (!bPressed)
				{
					bPressed = true;
					PressedCollider = CM.TracePoint(WorldMouseXY);
					if (PressedCollider)
					{
						PressedCollider->Clicked();
					}
				}
				else
				{
					if (PressedCollider)
					{
						PressedCollider->Pressed(WorldMouseXY);
					}
				}
			}
			else
			{
				if (bPressed)
				{
					bPressed = false;
					if (PressedCollider)
					{
						PressedCollider->Released(WorldMouseXY);
						PressedCollider = nullptr;
					}
				}
			}
		}

		deltaTime = (float)(elapsedTime / 1000.0);

		accumulator += (elapsedTime / 1000.0);

		while (accumulator >= fixedDeltaTime)
		{
			accumulator -= fixedDeltaTime;

			if (!bPausePhysics || bStepOnce)
			{
				for (int i = 0; i < (int)ObjectManager.AllObjects.size(); i++)
				{
					ObjectManager.AllObjects[i]->Tick((float)fixedDeltaTime);
				}

				if (bUseGravity)
				{
					for (ACollider* Collider : CM.colliders)
					{
						if (Collider->bUseGravity && Collider->GetMass() > 0.0f && !Collider->IsSleeping())
						{
							Collider->SetVelocity(Collider->GetVelocity() + Global::G * (float)fixedDeltaTime);
						}
					}
				}

				for (ACollider* Collider : CM.colliders)
				{
					if (Collider->GetMass() > 0.0f && !Collider->IsSleeping())
					{
						Collider->SetLocation(Collider->GetLocation() + Collider->GetVelocity() * (float)fixedDeltaTime);
						Collider->SetRotation(Collider->GetRotation() + Collider->GetAngularVelocity() * (float)fixedDeltaTime);
					}
				}

				CM.CheckCollisionAll((float)fixedDeltaTime);
			}
		}

		renderer.Prepare();
		renderer.PrepareShader();

		for (UObject* obj : ObjectManager.AllObjects)
		{
			if (AActor* Actor = dynamic_cast<AActor*>(obj))
			{
				if (!Actor->isInvalid)
				{
					Actor->Draw(renderer);
				}
			}
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (bDrawColliders)
		{
			ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

			for (ACollider* Collider : CM.colliders)
			{
				if (Collider->GetPrimitive() != EPrimitive::Rectangle)
				{
					continue;
				}

				OBB Box = MakeOBB(Collider);

				ImU32 Color = Collider->IsSleeping()
					? IM_COL32(150, 150, 150, 255)
					: IM_COL32(80, 200, 255, 255);

				for (int i = 0; i < 4; i++)
				{
					DrawList->AddLine(WorldToScreen(Box.vertex[i]),
						WorldToScreen(Box.vertex[(i + 1) % 4]), Color, 2.0f);
				}

				DrawList->AddCircleFilled(WorldToScreen(Box.vertex[0]), 4.0f, Color);
			}
		}

		if (bDrawContacts)
		{
			ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

			for (const CollisionInfo& Contact : CM.debugContacts)
			{
				for (int i = 0; i < Contact.pointCount; i++)
				{
					ImVec2 Point = WorldToScreen(Contact.points[i].position);
					ImVec2 Tip = ImVec2(Point.x + Contact.normal.x * NormalLength,
						Point.y - Contact.normal.y * NormalLength);

					DrawList->AddLine(Point, Tip, IM_COL32(255, 64, 64, 255), 2.0f);
					DrawList->AddCircleFilled(Point, 4.0f, IM_COL32(255, 220, 0, 255));
				}
			}
		}

		ImGui::Begin("3D Engine Base Inspector");
		ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);
		ImGui::Text("Mouse World: (%.3f, %.3f)", WorldMouseXY.x, WorldMouseXY.y);
		ImGui::Text("Selected ID: %d", PressedCollider ? PressedCollider->GetID() : -1);
		ImGui::Text("Colliders: %d", (int)CM.colliders.size());
		ImGui::Separator();

		ImGui::Text("Spawner");
		ImGui::Combo("Type", &SelectedPrimitive, PrimitiveItems, 2);
		ImGui::SliderFloat("Width/Radius", &SpawnWidth, 0.05f, 1.0f);
		ImGui::SliderFloat("Height", &SpawnHeight, 0.05f, 1.0f);
		ImGui::SliderFloat("Mass", &SpawnMass, 0.1f, 100.0f);
		if (ImGui::Button("Spawn at Center", ImVec2(150, 25)))
		{
			if (SelectedPrimitive == 0)
			{
				SpawnColider<ACollider>({ 0.0f, 0.5f, 0.0f }, EPrimitive::Rectangle, true, { SpawnWidth, SpawnHeight, 0.0f }, SpawnMass);
			}
			else
			{
				SpawnColider<ACircle>({ 0.0f, 0.5f, 0.0f }, EPrimitive::Circle, true, { SpawnWidth, SpawnWidth, 0.0f }, SpawnMass);
			}
		}

		ImGui::Separator();
		if (ImGui::Checkbox("Use Gravity", &bUseGravity))
		{
			if (bUseGravity)
			{
				for (ACollider* Collider : CM.colliders)
				{
					Collider->WakeUp();
				}
			}
		}
		ImGui::SameLine();
		ImGui::Checkbox("Pause Physics", &bPausePhysics);
		ImGui::SameLine();
		if (ImGui::Button("Step"))
		{
			bStepOnce = true;
		}
		ImGui::Checkbox("Draw Contacts", &bDrawContacts);
		ImGui::SameLine();
		ImGui::Checkbox("Draw Colliders", &bDrawColliders);

		ImGui::SeparatorText("Physics Solver");
		ImGui::SliderInt("Velocity Iter", &CM.velocityIterations, 1, 20);
		ImGui::SliderInt("Position Iter", &CM.positionIterations, 1, 20);
		ImGui::SliderFloat("Baumgarte", &CM.baumgarte, 0.05f, 1.0f);
		ImGui::SliderFloat("Slop", &CM.slop, 0.0f, 0.02f, "%.4f");
		ImGui::SliderFloat("Rolling", &CM.rollingResistance, 0.0f, 0.02f, "%.4f");
		ImGui::Checkbox("Sleep Enabled", &CM.bSleepEnabled);
		ImGui::SliderFloat("Linear Tol", &CM.linearSleepTolerance, 0.0f, 0.1f, "%.4f");
		ImGui::SliderFloat("Angular Tol", &CM.angularSleepTolerance, 0.0f, 0.3f, "%.4f");
		ImGui::SliderFloat("Time To Sleep", &CM.timeToSleep, 0.05f, 2.0f);

		ImGui::End();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		renderer.SwapBuffer();

		do
		{
			Sleep(0);
			QueryPerformanceCounter(&endTime);
			elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;
		} while (elapsedTime < targetFrameTime);

		elapsedTime = min(elapsedTime, maxAccumulated * 1000.0);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	renderer.ReleaseVertexBuffers();
	renderer.ReleaseConstantBuffer();
	renderer.ReleaseShader();
	renderer.Release();

	return 0;
}