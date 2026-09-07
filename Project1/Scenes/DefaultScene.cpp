#include "pch.h"
#include "DefaultScene.h"
#include "Camera.h"
#include "Renderer.h"
#include "PickingManager.h"
#include "SaveLoadManager.h"


DefaultScene::DefaultScene()
{
	cube = FObjectFactory::SpawnColider<ACube>(FVector(0.0f, 0.0f, 0.0f), { 1.0f, 1.0f, 1.0f });
	cube->SetColor(FLinearColor::Blue);

	cube2 = FObjectFactory::SpawnColider<ACube>(FVector(10.0f, 0.0f, 0.0f), { 1.0f, 1.0f, 1.0f });
	cube2->SetColor(FLinearColor::Red);

	sphere = FObjectFactory::SpawnActor<ASphere>(FVector(-10.0f, 0.0f, 0.0f));
	sphere->SetColor(FLinearColor::Green);

	gizmo = FObjectFactory::SpawnActor<AGizmo>();
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

	//카메라 속도 및 회전 조절
	ImGui::SliderFloat("Move Speed", &cam.GetSpeedRef(), 0.5f, 20.0f, "%.1f");
	ImGui::SliderFloat("Rot Speed", &cam.GetRotationSpeedRef(), 0.01f, 0.5f, "%.3f");

	if (ImGui::Button("Reset Camera (0, 0, -3)"))
	{
		cam.SetLocation(FVector(0.0f, 0.0f, -3.0f));
		cam.SetRotation(FVector(0.0f, 0.0f, 0.0f));
	}
	
	FVector camFwd = cam.GetForward();
	ImGui::Text("Forward: (%.2f, %.2f, %.2f)", camFwd.x, camFwd.y, camFwd.z);
	
	ImGui::Separator();
	
	



	/////////////////////////////
	//////// SAVE & LOAD ////////
	/////////////////////////////
	
	// Save 버튼
	ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.8f, 1.0f), "[ Save & Load Scene ]");
	if (ImGui::Button("Save Scene"))
	{
		// "./SceneData/MyScene.Scene" 으로 저장됨
		SaveLoadManager::SaveScene("./SceneData/MyScene"); 
	}
	
	// Load 버튼
	if (ImGui::Button("Load Scene"))
	{
		// "./SceneData/MyScene.Scene" 에서 로드됨
		TArray<UObject*> loadedObj = SaveLoadManager::LoadScene("./SceneData/MyScene.Scene");
		
		// 기존 cube는 이미 삭제됐으므로 일단 무효화
		cube = nullptr;

		for (UObject* obj : loadedObj)
		{
			// Todo: 객체 여러 개 소환되면 객체 type (Sphere, Cube 별로 Load)
			// 현재는 객체가 하나라는 가정 하에, 혹은 여러 개 중 첫번째 것이 cube인 경우만 구현함.
			if (ACube* c = dynamic_cast<ACube*>(obj) )
			{
				cube = c;
				break;
			}
		}
	}
	
	ImGui::Separator();

	// cube 디버그 섹션
	ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "[ Cube Controls ]");
	if (cube)
	{
		string uid = std::to_string(cube->GetID());

		FVector loc = cube->GetLocation();
		if (ImGui::DragFloat3(("Cube Pos##" + uid).c_str(), &loc.x, 0.01f, -10.0f, 10.0f))
		{
			cube->SetLocation(loc);
		}

		FVector scale = cube->GetScale();
		if (ImGui::DragFloat3(("Cube Scale##" + uid).c_str(), &scale.x, 0.01f, 0.01f, 5.0f))
		{
			cube->SetScale(scale);
		}

		FVector rot = cube->GetRotation();
		bool bCubeChanged = false;
		if (ImGui::DragFloat(("Rotation X" + uid).c_str(), &rot.x, 0.01f, -3.14f, 3.14f))
		{
			bCubeChanged = true;
		}
		if (ImGui::DragFloat(("Rotation Y" + uid).c_str(), &rot.y, 0.01f, -3.14f, 3.14f))
		{
			bCubeChanged = true;
		}
		if (ImGui::DragFloat(("Rotation Z" + uid).c_str(), &rot.z, 0.01f, -3.14f, 3.14f))
		{
			bCubeChanged = true;
		}

		if (bCubeChanged)
		{
			cube->SetRotation(rot);
		}
	}

	// gizmo 디버그 섹션
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
