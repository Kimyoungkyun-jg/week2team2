#pragma once

struct FRay {
	FVector Origin;
	FVector Direction;
};


class PickingManager
{
public:
	static PickingManager& GetInstance() {
		static PickingManager instance;
		return instance;
	}

	PickingManager(const PickingManager&) = delete;
	PickingManager& operator=(const PickingManager&) = delete;

	FRay ScreenToWorldRay(float mouseX, float mouseY, float screenW, float screenH) const;
	FRay ScreenToWorldRay() const;

	AActor* Pick();

	void Pressed();
	void Released();


	void Update();


	AActor* pickedObjcect;
private:
	PickingManager() {};
};

