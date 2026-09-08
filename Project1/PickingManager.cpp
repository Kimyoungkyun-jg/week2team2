#include "pch.h"
#include "PickingManager.h"
#include "Camera.h"
#include "TemplateLibrary.h"
#include "AGizmo.h"
#include "AWorldAxises.h"

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

FRay PickingManager::ScreenToWorldRay() const
{
	return ScreenToWorldRay(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y,
		RENDERER.ViewportInfo.Width, RENDERER.ViewportInfo.Height);
}

AActor* PickingManager::Pick()
{
	FRay ray = PICK.ScreenToWorldRay(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y,
		RENDERER.ViewportInfo.Width, RENDERER.ViewportInfo.Height);

	//기즈모 축 피킹 우선 검사
	if (AGizmo::MainGizmo && AGizmo::MainGizmo->GetTargetActor())
	{
		AGizmoAxis* closestAxis = nullptr;
		float closestDist = FLT_MAX;

		for (AGizmoAxis* axis : AGizmo::MainGizmo->GetAxes())
		{
			float axisDist = 0.0f;
			if (axis->bIsPicked(ray, axisDist))
			{
				if (axisDist > 0.0f && axisDist < closestDist)
				{
					closestDist = axisDist;
					closestAxis = axis;
				}
			}
		}

		if (closestAxis)
		{
			closestAxis->Picked();
			pickedObjcect = closestAxis;
			return closestAxis;
		}
	}

	//일반 액터 피킹 검사
	AActor* closest = nullptr;
	float closestDist = FLT_MAX;

	for (auto object : ObjectManager::GetInstance().AllObjects) {
		AActor* actor = Cast<AActor>(object);
		if (actor == nullptr || Cast<AGizmo>(actor) || Cast<AWorldAxises>(actor)) continue;

		float dist = 0.0f;
		if (actor->bIsPicked(ray, dist) && dist < closestDist)
		{
			closestDist = dist;
			closest = actor;
		}
	}

	if (AGizmo::MainGizmo)
	{
		AGizmo::MainGizmo->SetTargetActor(closest);
	}

	pickedObjcect = closest;

	return closest;
}


void PickingManager::Pressed()
{
	
}

void PickingManager::Update()
{
	if (MOUSE_CLICK(0))
	{
		Pick();
	}
	else if (MOUSE_PRESS(0))
	{
		if (pickedObjcect)
		{
			pickedObjcect->Pressed();
		}
	}
	else if (MOUSE_UP(0))
	{
		if (pickedObjcect)
		{
			pickedObjcect->Released();
		}
	}
	
}
