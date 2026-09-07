#include "pch.h"
#include "AGizmo.h"
#include "Renderer.h"
#include "PickingManager.h"


AGizmoAxis::AGizmoAxis(EGizmoAxis inAxis)
	: Axis(inAxis)
{
	// AActor의 템플릿 InitVertexBuffer 호출 -> GPU 버퍼 생성 및 LocalVertices 자동 저장!
	InitVertexBuffer(arrow_vertices);

	switch (Axis)
	{
	case EGizmoAxis::X:
		Color = FLinearColor::Red;
		break;
	case EGizmoAxis::Y:
		Color = FLinearColor::Green;
		break;
	case EGizmoAxis::Z:
		Color = FLinearColor::Blue;
		break;
	default:
		Color = FLinearColor::White;
		break;
	}
}

AGizmoAxis::~AGizmoAxis()
{
	//vertexbuffer와 worldBuffer는 부모인 AActor::~AActor()가 안전하게 해제함
}

void AGizmoAxis::Update(float DeltaTime, const Transform& parentTransform)
{
	transform.Location = parentTransform.Location;
	transform.Scale = parentTransform.Scale;

	switch (Axis)
	{
	case EGizmoAxis::X:
		transform.SetRotation({ 0.0f, 0.0f, -DirectX::XM_PIDIV2 });
		break;
	case EGizmoAxis::Y:
		transform.SetRotation({ 0.0f, 0.0f, 0.0f });
		break;
	case EGizmoAxis::Z:
		transform.SetRotation({ DirectX::XM_PIDIV2, 0.0f, 0.0f });
		break;
	default:
		break;
	}

	AActor::Update(DeltaTime);
}

void AGizmoAxis::Render()
{
	if (vertexbuffer == nullptr || numVertices == 0)
		return;

	// 깊이 판정 비활성화 (물체에 가려지지 않고 항상 최상단 렌더링)
	RENDERER.SetGizmoDepthState();

	RENDERER.PrepareShader(inputLayout);
	vertexbuffer->IASet();

	worldBuffer->SetMat(transform.WorldMat);
	worldBuffer->SetVSBuffer(0);

	RENDERER.SetCustomColor(Color);
	DC->Draw(numVertices, 0);

	// 기본 깊이 상태로 복원
	RENDERER.SetDefaultDepthState();
}

void AGizmoAxis::Picked()
{
	// 기즈모 축이 클릭(피킹)되었을 때의 처리
}



AGizmo::AGizmo()
{
	MainGizmo = this;
	Primitive = EPrimitive::Gizmo;

	// 3개의 기즈모 축 액터 생성 (X, Y, Z)
	Axes.push_back(new AGizmoAxis(EGizmoAxis::X));
	Axes.push_back(new AGizmoAxis(EGizmoAxis::Y));
	Axes.push_back(new AGizmoAxis(EGizmoAxis::Z));
}

AGizmo::~AGizmo()
{
	if (MainGizmo == this)
	{
		MainGizmo = nullptr;
	}

	for (auto* axis : Axes)
	{
		delete axis;
	}
	Axes.clear();
}

void AGizmo::SetTargetActor(AActor* inTarget)
{
	TargetActor = inTarget;
	if (TargetActor)
	{
		transform.SetLocation(TargetActor->GetLocation());
		transform.Scale = TargetActor->GetScale() * 0.7f;
	}
}

void AGizmo::Update(float DeltaTime)
{
	AActor::Update(DeltaTime);

	// 피킹된 타겟 액터가 있을 때만 위치 동기화 및 3개 축 업데이트
	if (TargetActor)
	{
		transform.SetLocation(TargetActor->GetLocation());
		for (auto* axis : Axes)
		{
			axis->Update(DeltaTime, transform);
		}
	}
}

void AGizmo::Render()
{
	//타겟 액터 없을 시 스킵
	if (!TargetActor)
		return;

	//기즈모 축 렌더링
	for (auto* axis : Axes)
	{
		axis->Render();
	}
}

EGizmoAxis AGizmo::PickAxis(const FRay& ray, float& outDist)
{
	if (!TargetActor)
		return EGizmoAxis::None;

	EGizmoAxis hitAxis = EGizmoAxis::None;
	float closestDist = FLT_MAX;

	//축 피킹 검사
	for (auto* axis : Axes)
	{
		float dist = 0.0f;
		if (axis->bIsPicked(ray, dist))
		{
			if (dist > 0.0f && dist < closestDist)
			{
				closestDist = dist;
				hitAxis = axis->GetAxis();
			}
		}
	}

	if (hitAxis != EGizmoAxis::None)
	{
		outDist = closestDist;
		SelectedAxis = hitAxis;
	}

	return hitAxis;
}

void AGizmo::Pressed(FVector _Location)
{
	// 마우스 클릭 피킹 시 선택된 축 조작 로직
}

void AGizmo::Released(FVector _Location)
{
	SelectedAxis = EGizmoAxis::None;
}
