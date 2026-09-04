#include "pch.h"
#include "DefaultScene.h"


DefaultScene::DefaultScene()
{
	circle = SpawnColider<ACircle>(FVector(0.5f, 0.5f, 0.5f), EPrimitive::Circle);

	SceneObjects.push_back(circle);
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
}
