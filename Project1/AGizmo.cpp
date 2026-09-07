#include "pch.h"
#include "AGizmo.h"
#include "Renderer.h"
#include "PickingManager.h"


AGizmoAxis::AGizmoAxis(EGizmoAxis inAxis)
	: Axis(inAxis)
{
	// AActor의 템플릿 InitVertexBuffer 호출 -> GPU 버퍼 생성 및 LocalVertices 자동 저장!
	InitVertexBuffer(arrow_vertices);
	Primitive = EPrimitive::Gizmo;

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

	srcColor = Color;
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

	//마우스 호버 또는 피킹 선택 시 하이라이트 처리
	FRay ray = PICK.ScreenToWorldRay();
	if (bSelected || bIsPicked(ray))
	{
		HighlightAxe();
	}
	else
	{
		SetColor(srcColor);
	}
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
	if (!Targettransform) return;

	//축 방향 결정
	switch (Axis)
	{
	case EGizmoAxis::X:
		currentAxisDir = TransformDirection({ 1,0,0 }, Targettransform->WorldMat).Normalized();
		break;
	case EGizmoAxis::Y:
		currentAxisDir = TransformDirection({ 0,1,0 }, Targettransform->WorldMat).Normalized();
		break;
	case EGizmoAxis::Z:
		currentAxisDir = TransformDirection({ 0,0,1 }, Targettransform->WorldMat).Normalized();
		break;
	default:
		currentAxisDir = FVector(0, 0, 0);
		break;
	}

	//평면 법선 벡터 계산
	FVector cameraDir = CAMERA.GetForward();
	FVector A = FVector::Cross3D(currentAxisDir, cameraDir).Normalized();
	planeNormal = FVector::Cross3D(A, currentAxisDir).Normalized();

	//드래그 시작 지점 및 타겟 초기 위치 저장
	FRay ray = PICK.ScreenToWorldRay();

	float denom = planeNormal.Dot(ray.Direction);
	if (fabsf(denom) > 1e-6f)
	{
		float t = (Targettransform->GetLocation() - ray.Origin).Dot(planeNormal) / denom;
		dragStartPoint = ray.Origin + ray.Direction * t;
		dragStartActorLocation = Targettransform->GetLocation();
	}


	//피킹 선택 상태 활성화
	bSelected = true;
	HighlightAxe();
}

void AGizmoAxis::Pressed()
{
	if (!Targettransform) return;

	FRay ray = PICK.ScreenToWorldRay();
	float denom = planeNormal.Dot(ray.Direction);
	if (fabsf(denom) > 1e-6f)
	{
		float t = (dragStartPoint - ray.Origin).Dot(planeNormal) / denom;
		FVector currentHitPoint = ray.Origin + ray.Direction * t;

		//이동량 계산 및 축 투영
		FVector delta = currentHitPoint - dragStartPoint;
		float moveDist = delta.Dot(currentAxisDir);

		//타겟 위치 갱신 및 월드 행렬 업데이트
		Targettransform->SetLocation(dragStartActorLocation + currentAxisDir * moveDist);
	}

}

void AGizmoAxis::Released()
{
	//피킹 선택 상태 해제
	bSelected = false;
	SetColor(srcColor);
}

void AGizmoAxis::HighlightAxe()
{
	SetColor(Highlighting(srcColor));
}






AGizmo::AGizmo()
{
	//메인 기즈모 인스턴스 등록
	MainGizmo = this;
	Primitive = EPrimitive::Gizmo;

	//기즈모 축 액터 생성
	Axes.push_back(new AGizmoAxis(EGizmoAxis::X));
	Axes.push_back(new AGizmoAxis(EGizmoAxis::Y));
	Axes.push_back(new AGizmoAxis(EGizmoAxis::Z));
}

AGizmo::~AGizmo()
{
	//메인 기즈모 인스턴스 해제
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
		for (auto& it : Axes)
		{
			it->SetTargetActor(&inTarget->GetTransform());
		}

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
