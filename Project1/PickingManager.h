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

	UObject* Pick(const FRay& ray) const;

	bool RayIntersectBox(const FRay& ray, const FVector& center, const FVector& scale, float& outDistance) const;

private:
	PickingManager() {};
};

