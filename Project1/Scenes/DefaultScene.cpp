#include "pch.h"
#include "DefaultScene.h"
#include "Camera.h"
#include "Renderer.h"
#include "PickingManager.h"


DefaultScene::DefaultScene()
{
	cube = FObjectFactory::SpawnColider<ACube>(FVector(0.0f, 0.0f, 0.0f), EPrimitive::Cube, { 1.0f, 1.0f, 1.0f });
}

DefaultScene::~DefaultScene()
{
}


void DefaultScene::Initialize()
{
}

void DefaultScene::Update(float deltatime)
{
	Scene::Update(deltatime);
}

void DefaultScene::Render()
{
	Scene::Render();

	ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_FirstUseEver);
	ImGui::Begin("Scene & Camera Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	// 카메라 디버그 섹션
	ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.4f, 1.0f), "[ Camera Controls ]");
	Camera& cam = Camera::GetInstance();
	FVector camLoc = cam.GetLocation();
	if (ImGui::DragFloat3("Cam Pos", &camLoc.x, 0.05f, -20.0f, 20.0f))
	{
		cam.SetLocation(camLoc);
	}
	FVector camRot = cam.GetRotation();
	if (ImGui::DragFloat3("Cam Rot", &camRot.x, 0.01f, -3.14f, 3.14f))
	{
		cam.SetRotation(camRot);
	}
	if (ImGui::Button("Reset Camera (0, 0, -3)"))
	{
		cam.SetLocation(FVector(0.0f, 0.0f, -3.0f));
		cam.SetRotation(FVector(0.0f, 0.0f, 0.0f));
	}
	FVector camFwd = cam.GetForward();
	ImGui::Text("Forward: (%.2f, %.2f, %.2f)", camFwd.x, camFwd.y, camFwd.z);

	ImGui::Separator();
	
	if (ImGui::IsMouseClicked(0)) {
		// pick 테스트 코드 부분입니다! F5 로 출력 확인해보세요 :)
		ray = PickingManager::GetInstance().ScreenToWorldRay(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y,
			Renderer::GetInstance().ViewportInfo.Width, Renderer::GetInstance().ViewportInfo.Height);
		UObject * pickedObj = PickingManager::GetInstance().Pick(ray);
		if (pickedObj) {
			OutputDebugStringA("hit!");
		}
	}

	// 큐브 디버그 섹션
	ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "[ Cube Controls ]");
	if (cube)
	{
		FVector loc = cube->GetLocation();
		if (ImGui::DragFloat3("Cube Pos", &loc.x, 0.01f, -5.0f, 5.0f))
		{
			cube->SetLocation(loc);
		}

		FVector scale = cube->GetScale();
		if (ImGui::DragFloat3("Cube Scale", &scale.x, 0.01f, 0.01f, 5.0f))
		{
			cube->SetScale(scale);
		}

		FVector rot = cube->GetRotation();
		bool bRotChanged = false;
		if (ImGui::DragFloat("Rotation X", &rot.x, 0.01f, -3.14f, 3.14f))
		{
			bRotChanged = true;
		}
		if (ImGui::DragFloat("Rotation Y", &rot.y, 0.01f, -3.14f, 3.14f))
		{
			bRotChanged = true;
		}
		if (ImGui::DragFloat("Rotation Z", &rot.z, 0.01f, -3.14f, 3.14f))
		{
			bRotChanged = true;
		}

		if (bRotChanged)
		{
			cube->SetRotation(rot);
		}
	}

	

	ImGui::End();
}
