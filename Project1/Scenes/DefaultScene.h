#pragma once
#include "Scene.h"
#include "ACollider.h"
#include "AGizmo.h"
#include "AWorldAxises.h"
#include "PickingManager.h"
#include "AGrid.h"
#include "ASkySphere.h"

#include "UGrid.h"

class DefaultScene : public Scene
{
public:
	DefaultScene();
	~DefaultScene();

	virtual void Initialize() override;
	virtual void Update(float deltatime) override;
	virtual void Render() override;

	
	AGizmo* gizmo = nullptr;

	AWorldAxises* worldAxises;


	AGrid* grid;
	UGrid Ugrid;

	ASkySphere* skysphere;

};

