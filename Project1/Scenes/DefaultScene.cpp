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

	// Grid 생성
	grid = FObjectFactory::SpawnActor<AGrid>(EGridType::Triangle);

	// Gizmo 생성
	gizmo = FObjectFactory::SpawnActor<AGizmo>();

	// Sky 생성
	skysphere = FObjectFactory::SpawnActor<ASkySphere>();

}

DefaultScene::~DefaultScene()
{
}


void DefaultScene::Initialize()
{
	// ImGUI UI 생성
	IMGUI.Initialize();
}

void DefaultScene::Update(float deltatime)
{
	Scene::Update(deltatime);
}

void DefaultScene::Render()
{
	Scene::Render();
	IMGUI.RenderAll();

}
