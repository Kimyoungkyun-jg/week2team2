#include "pch.h"
#include "DefaultScene.h"


DefaultScene::DefaultScene()
{
	cube = SpawnColider<ACube>(FVector(0.0f, 0.0f, 0.2f), EPrimitive::Cube, {0.2f,0.2f,0.2f});

	SceneObjects.push_back(cube);
}

DefaultScene::~DefaultScene()
{

	SceneObjects.clear();
}


void DefaultScene::Initialize()
{
}

void DefaultScene::Update(float deltatime)
{
	if (SceneObjects.size() > 0)
	{
		for (auto& it : SceneObjects)
		{
			it->Update(deltatime);
		}
	}

}

void DefaultScene::Render()
{
	if (SceneObjects.size() > 0)
	{
		for (auto& it : SceneObjects)
		{
			it->Render();
		}
	}

	ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_FirstUseEver);
	ImGui::Begin("cube Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	if (cube)
	{
		FVector loc = cube->GetLocation();
		if (ImGui::DragFloat3("Location", &loc.x, 0.01f, -2.0f, 2.0f))
		{
			cube->SetLocation(loc);
		}

		FVector scale = cube->GetScale();
		if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.01f, 5.0f))
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
