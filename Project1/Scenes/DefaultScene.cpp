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

	// Grid 초기화
	Ugrid.Initialize();
}

void DefaultScene::Update(float deltatime)
{
	Scene::Update(deltatime);
	
	Camera& cam = CAMERA;
	Ugrid.Update(cam.GetLocation());
}

void DefaultScene::Render()
{
	Ugrid.Render();
	Scene::Render();
	IMGUI.RenderAll();
}
