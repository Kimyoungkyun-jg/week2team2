#pragma once

#include <windows.h>
#include <windowsx.h>
#include <wrl.h>
#include <comdef.h>

// D3D 및 유틸 라이브러리 링크
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "gdi32.lib")

// D3D / Direct2D / DirectX 헤더
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <DirectXMath.h>
#include <DirectXTex.h>

// IMGUI
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// C/C++ 표준 라이브러리
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <utility>
#include <ctime>
#include <cassert>
#include <filesystem>

// 프로젝트 공용 열거형
#include "enums.h"

// 매크로 정의 (키보드, 마우스, 시간, 씬)
#define KEY_PRESS(k) (!ImGui::GetIO().WantCaptureKeyboard && ImGui::IsKeyDown(k))
#define KEY_DOWN(k) (!ImGui::GetIO().WantCaptureKeyboard && ImGui::IsKeyPressed(k, false))
#define KEY_UP(k) (!ImGui::GetIO().WantCaptureKeyboard && ImGui::IsKeyReleased(k))

// 0 : 좌클릭, 1: 우클릭, 2 : 중간클릭 (UI 창 바깥 게임 화면 클릭만 인식)
#define MOUSE_PRESS(k) (!ImGui::GetIO().WantCaptureMouse && ImGui::IsMouseDown(k))
#define MOUSE_CLICK(k) (!ImGui::GetIO().WantCaptureMouse && ImGui::IsMouseClicked(k))
#define MOUSE_DBCLICK(k) (!ImGui::GetIO().WantCaptureMouse && ImGui::IsMouseDoubleClicked(k))
#define MOUSE_UP(k) (!ImGui::GetIO().WantCaptureMouse && ImGui::IsMouseReleased(k))

#define SCENE SceneManager::GetInstance()
#define DELTA ImGui::GetIO().DeltaTime

#define WIN_WIDTH 1920	
#define WIN_HEIGHT 1080


//Scene Header
#include "Scene.h"
#include "SceneManager.h"