#include "pch.h"
#include "PickingManager.h"
#include "Camera.h";

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

AActor* PickingManager::Pick(const FRay& ray, const FVector& center, float radius, float& outDistance) const
{
	return nullptr;
}

bool* PickingManager::RayIntersectsSphere(const FRay& ray, const FVector& center, float radius, float& outDistance) const
{
	return nullptr;
}
