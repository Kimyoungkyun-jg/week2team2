#include "pch.h"
#include "PickingManager.h"
#include "Camera.h"

FRay PickingManager::ScreenToWorldRay(float mouseX, float mouseY, float screenW, float screenH) const
{
	// NDC -> View 
	float ndcX = 2.0f * mouseX / screenW - 1.0f;
	float ndcY = -2.0f * mouseY / screenH + 1.0f;

	FMatrix proj = Camera::GetInstance().GetProjectionMatrix(screenW / screenH);

	float viewX = ndcX / proj.M[0][0];
	float viewY = ndcY / proj.M[1][1];

	FMatrix view = Camera::GetInstance().GetViewMatrix();
	FMatrix invView = view.InverseAffine();

	// View -> World
	FVector viewDirection(viewX, viewY, 1.0f);
	FVector worldDirection = TransformDirection(viewDirection, invView);
	worldDirection.Normalize();

	FVector worldOrigin = Camera::GetInstance().GetLocation();

	return FRay{ worldOrigin, worldDirection };
}

AActor* PickingManager::Pick(const FRay& ray) const
{
	AActor* closest = nullptr;
	float closestDist = FLT_MAX;

	for (auto object : ObjectManager::GetInstance().AllObjects) {
		AActor * actor = dynamic_cast<AActor*> (object);
		if (actor == nullptr) continue;

		if (actor->GetPrimitive() == EPrimitive::Cube) {
			float dist;
			if (RayIntersectBox(ray, actor->GetLocation(), actor->GetScale(), dist)) {
				if (dist < closestDist) {
					closestDist = dist;
					closest = actor;
				}
			}
		}
	}
	return closest;
}

bool PickingManager::RayIntersectBox(const FRay& ray, const FVector& center, const FVector& scale, float& outDistance) const
{
	FVector minBox = center - scale * 0.5f;
	FVector maxBox = center + scale * 0.5f;

	float tMin = -FLT_MAX, tMax = FLT_MAX;

	if (fabsf(ray.Direction.x) > 1e-6f) {
		float t1 = (minBox.x - ray.Origin.x) / ray.Direction.x;
		float t2 = (maxBox.x - ray.Origin.x) / ray.Direction.x;
		if (t1 > t2) swap(t1, t2);
		tMin = max(tMin, t1);
		tMax = min(tMax, t2);
	}
	if (fabsf(ray.Direction.y) > 1e-6f) {
		float t1 = (minBox.y - ray.Origin.y) / ray.Direction.y;
		float t2 = (maxBox.y - ray.Origin.y) / ray.Direction.y;
		if (t1 > t2) swap(t1, t2);
		tMin = max(tMin, t1);
		tMax = min(tMax, t2);
	}
	if (fabsf(ray.Direction.z) > 1e-6f) {
		float t1 = (minBox.z - ray.Origin.z) / ray.Direction.z;
		float t2 = (maxBox.z - ray.Origin.z) / ray.Direction.z;
		if (t1 > t2) swap(t1, t2);
		tMin = max(tMin, t1);
		tMax = min(tMax, t2);
	}

	if (tMin > tMax || tMax < 0) {
		return false;
	}

	outDistance = tMin;
	return true;
}
