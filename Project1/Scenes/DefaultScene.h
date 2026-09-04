#pragma once
#include "Scene.h"
#include "ACollider.h"

class DefaultScene : public Scene
{
public:
	DefaultScene();
	~DefaultScene();

	virtual void Initialize() override;
	virtual void Update(float deltatime) override;
	virtual void Render() override;

	ACube* cube = nullptr;



	
};

