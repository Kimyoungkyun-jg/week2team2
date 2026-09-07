#pragma once
#include "Scene.h"
#include "ACollider.h"
#include "AGizmo.h"
#include "AWorldAxises.h"
#include "PickingManager.h"
#include "AGrid.h"

class DefaultScene : public Scene
{
public:
	DefaultScene();
	~DefaultScene();

	virtual void Initialize() override;
	virtual void Update(float deltatime) override;
	virtual void Render() override;

	ACube* cube = nullptr;
	ACube* cube2 = nullptr;
	ASphere* sphere = nullptr;
	AGizmo* gizmo = nullptr;

	AWorldAxises* worldAxises;

	FRay ray;

	AGrid* grid;
};

