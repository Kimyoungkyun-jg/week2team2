#include "pch.h"
#include "DefaultScene.h"
#include "Camera.h"
#include "Renderer.h"


DefaultScene::DefaultScene()
{
	// 기즈모만 단독으로 스폰 (위치: 원점 0, 0, 0 / 크기: 1, 1, 1)
	gizmo = FObjectFactory::SpawnActor<AGizmo>(FVector(0.0f, 0.0f, 0.0f), { 1.0f, 1.0f, 1.0f });
	//cube = FObjectFactory::SpawnColider<ACube>(FVector(0.0f, 0.0f, 0.0f), { 1.0f, 1.0f, 1.0f });
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

	// 기즈모 디버그 섹션
	ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "[ Gizmo Controls ]");
	if (gizmo)
	{
		FVector loc = gizmo->GetLocation();
		if (ImGui::DragFloat3("Gizmo Pos", &loc.x, 0.01f, -10.0f, 10.0f))
		{
			gizmo->SetLocation(loc);
		}

		FVector scale = gizmo->GetScale();
		if (ImGui::DragFloat3("Gizmo Scale", &scale.x, 0.01f, 0.01f, 5.0f))
		{
			gizmo->SetScale(scale);
		}

		FVector rot = gizmo->GetRotation();
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
			gizmo->SetRotation(rot);
		}
	}
	ImGui::End();
}
