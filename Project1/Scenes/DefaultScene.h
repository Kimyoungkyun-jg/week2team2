#pragma once
#include "Scene.h"
#include "UObject.h"

class DefaultScene : public Scene
{
public:
	DefaultScene();
	~DefaultScene();

	virtual void Initialize() override;
	virtual void Update(float deltatime) override;
	virtual void Render() override;

	ACircle* circle = nullptr;



	
};

