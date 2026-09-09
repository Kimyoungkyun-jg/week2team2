#include "pch.h"
#include "DefaultScene.h"
#include "Camera.h"
#include "Renderer.h"
#include "PickingManager.h"
#include "SaveLoadManager.h"




DefaultScene::DefaultScene()
{
	// World Map Axis 생성
	worldAxises = FObjectFactory::SpawnActor<AWorldAxises>();

	// 기즈모 직접 생성
	gizmo = new AGizmo();

	// Sky 생성
	skysphere = FObjectFactory::SpawnActor<ASkySphere>();
}

DefaultScene::~DefaultScene()
{
	// 기즈모 직접 해제
	if (gizmo)
	{
		delete gizmo;
		gizmo = nullptr;
	}
}


void DefaultScene::Initialize()
{
	// ImGUI UI 생성
	IMGUI.Initialize();

	// Grid 초기화
	Ugrid.Initialize();
}

void DefaultScene::Update(float deltatime)
{
	Scene::Update(deltatime);

	// 기즈모 갱신
	if (gizmo)
	{
		gizmo->Update(deltatime);
	}
	
	Camera& cam = CAMERA;
	Ugrid.Update(cam.GetLocation());
}

void DefaultScene::Render()
{
	Ugrid.Render();
	Scene::Render();

	if (gizmo)
	{
		gizmo->Render();
	}

	IMGUI.RenderAll();
}
