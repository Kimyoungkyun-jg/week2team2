#include "pch.h"
#include "DefaultScene.h"
#include "Camera.h"
#include "Renderer.h"
#include "PickingManager.h"
#include "SaveLoadManager.h"
#include <random>


DefaultScene::DefaultScene()
{
	cube = FObjectFactory::SpawnColider<ACube>(FVector(0.0f, 0.0f, 0.0f), { 1.0f, 1.0f, 1.0f }, 1.0f, FLinearColor::Cyan);

	cube2 = FObjectFactory::SpawnColider<ACube>(FVector(5.0f, 0.0f, 0.0f), { 1.0f, 1.0f, 1.0f }, 1.0f, FLinearColor::Magenta);


	sphere = FObjectFactory::SpawnActor<ASphere>(
		FVector(-5.0f, 0.0f, 0.0f),
		FVector(1.0f, 1.0f, 1.0f),
		FLinearColor::Red
	);


	// World Map Axis 생성
	worldAxises = FObjectFactory::SpawnActor<AWorldAxises>();

	// Grid 생성
	grid = FObjectFactory::SpawnActor<AGrid>(EGridType::Triangle);

	// Gizmo 생성
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
	FQuaternion camRot = cam.GetRotation();
	if (ImGui::DragFloat3("Cam Rot", &camRot.x, 0.01f, -3.14f, 3.14f))
	{
		cam.SetRotation(camRot);
	}

	//카메라 속도 및 회전 조절
	ImGui::SliderFloat("Move Speed", &cam.GetSpeedRef(), 0.5f, 20.0f, "%.1f");
	ImGui::SliderFloat("Rot Speed", &cam.GetRotationSpeedRef(), 0.01f, 0.5f, "%.3f");

	if (ImGui::Button("Reset Camera"))
	{
		cam.SetLocation(FVector(3.336f, 3.282f, -4.715f));
		cam.SetRotation(FQuaternion::FromEuler(0.391f, -0.468f, 0.0f));
	}
	
	FVector camFwd = cam.GetForward();
	ImGui::Text("Forward: (%.2f, %.2f, %.2f)", camFwd.x, camFwd.y, camFwd.z);
	
	ImGui::Separator();
	
	/////////////////////////////
	//////// SAVE & LOAD ////////
	/////////////////////////////
	
	// Spawn 버튼
	ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.8f, 1.0f), "[ Spawn Primitives ]");
	
	// Select number
	static int spawnCount = 1;
	ImGui::InputInt("No. of Prim", &spawnCount);
	if (spawnCount < 0) spawnCount = 0;

	// Select Primitives
	static int selected_item = 0;
	const char* items[] = { "Sphere", "Cube", "Circle", "Rectangle", "Triangle" };
	ImGui::Combo("##Primitives", &selected_item, items, IM_ARRAYSIZE(items));

	// 난수 생성 및 범위 설정 -> spawn 위치 지정을 위해
	// 화면 안에 spawn 되도록 수정
	FVector camLocation = Camera::GetInstance().GetLocation();
	FVector camForward = Camera::GetInstance().GetForward();

	static std::mt19937 rng(std::random_device{}());
	static std::uniform_real_distribution<float> distSide(-3.0f, 3.0f);   // 좌우 범위
	static std::uniform_real_distribution<float> distUp(0.0f, 2.0f);      // 상하 범위 (Grid 위로 한정)

	float spawnDistance = 8.0f; // 카메라 앞으로 얼마나 떨어뜨릴지

	// 카메라의 오른쪽 벡터 (forward와 up의 외적)
	FVector worldUp(0.0f, 1.0f, 0.0f);
	FVector camRight = FVector::Cross3D(camForward, worldUp).Normalized();

	FVector spawnCenter = camLocation + camForward * spawnDistance;

	FVector randomLoc = spawnCenter + camRight * distSide(rng) + worldUp * distUp(rng);

	if (ImGui::Button("Spawn"))
	{
		for (int i=0; i<spawnCount; i++)
		{	
			switch(selected_item)
			{
				case 0 :
					FObjectFactory::SpawnColider<ASphere>(randomLoc, { 1.0f, 1.0f, 1.0f });
					break;
				case 1 :
					FObjectFactory::SpawnColider<ACube>(randomLoc, { 1.0f, 1.0f, 1.0f });
					break;
				case 2 :
					FObjectFactory::SpawnColider<ACircle>(randomLoc, { 1.0f, 1.0f, 1.0f });
					break;
				case 3 : 
					FObjectFactory::SpawnColider<ARectangle>(randomLoc, { 1.0f, 1.0f, 1.0f });
					break;
				case 4 :
					FObjectFactory::SpawnColider<ATriangle>(randomLoc, { 1.0f, 1.0f, 1.0f });
					break;
				default :
					break;
			}
		}
			
	}

	ImGui::Separator();

	// Collider만 삭제 (Grid, World Axis Gizmo 삭제되지 않도록)
	ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.8f, 1.0f), "[ Save & Load Scene ]");
	if (ImGui::Button("New Scene"))
	{
		ObjectManager::GetInstance().DestroyAllColliders();
		
	}
	
	// Save 버튼
	if (ImGui::Button("Save Scene"))
	{
		// "./SceneData/MyScene.Scene" 으로 저장됨
		SaveLoadManager::SaveScene("./SceneData/MyScene"); 
	}
	
	// Load 버튼
	if (ImGui::Button("Load Scene"))
	{
		// World Map Axis 생성
		worldAxises = FObjectFactory::SpawnActor<AWorldAxises>();

		// Grid 생성
		grid = FObjectFactory::SpawnActor<AGrid>(EGridType::Triangle);

		// Gizmo 생성
		gizmo = FObjectFactory::SpawnActor<AGizmo>();

		// "./SceneData/MyScene.Scene" 에서 로드됨
		TArray<UObject*> loadedObj = SaveLoadManager::LoadScene("./SceneData/MyScene.Scene");

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

		FQuaternion rot = cube->GetRotation();
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

		FQuaternion rot = gizmo->GetRotation();
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

	ImGui::Begin("Picking Primitive Property", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	// Picked Primitive Editor
	ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "[ Picking Controls ]");
	AActor* pickedActor = PickingManager::GetInstance().pickedObjcect;

	if (pickedActor)
	{
		AGizmoAxis* gizmo = dynamic_cast<AGizmoAxis*>(pickedActor);
		if (gizmo)
		{
			pickedActor = gizmo->GetTargetActor();
		}


		string uid = std::to_string(pickedActor->GetID());
		string cid = string(pickedActor->GetClass()->Name);
		// DEBUG
		// OutputDebugStringA(uid.c_str());
		ImGui::Text("UUID: %s", uid.c_str());
		ImGui::Text("ClassName: %s", cid.c_str());


		// Location Editor
		FVector loc = pickedActor->GetLocation();
		if (ImGui::DragFloat3(("Pos##" + uid).c_str(), &loc.x, 0.01f, -10.0f, 10.0f))
		{
			pickedActor->SetLocation(loc);
		}

		// Scale Editor
		FVector scale = pickedActor->GetScale();
		if (ImGui::DragFloat3(("Scale##" + uid).c_str(), &scale.x, 0.01f, 0.01f, 5.0f))
		{
			pickedActor->SetScale(scale);
		}

		// Rotation Editor
		FQuaternion rot = pickedActor->GetRotation();

		bool bPrimChanged = false;
		if (ImGui::DragFloat(("Rotation X" + uid).c_str(), &rot.x, 0.01f, -3.14f, 3.14f))
		{
			bPrimChanged = true;
		}
		if (ImGui::DragFloat(("Rotation Y" + uid).c_str(), &rot.y, 0.01f, -3.14f, 3.14f))
		{
			bPrimChanged = true;
		}
		if (ImGui::DragFloat(("Rotation Z" + uid).c_str(), &rot.z, 0.01f, -3.14f, 3.14f))
		{
			bPrimChanged = true;
		}
		// 수정했으면 Rotation 재설정
		if (bPrimChanged)
		{
			pickedActor->SetRotation(rot);
		}
	}

	ImGui::End();


}
