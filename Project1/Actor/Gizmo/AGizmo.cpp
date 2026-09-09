#include "pch.h"
#include "AGizmo.h"
#include "Renderer.h"
#include "PickingManager.h"
#include "ObjectManager.h"


AGizmoAxis::AGizmoAxis(EGizmoMode& mode, EGizmoAxis inAxis)
	: Axis(inAxis), mode(&mode)
{
	Primitive = EPrimitive::Gizmo;
	// ObjectManager를 통해 기즈모 화살표 메시 공유 및 LocalVertices 재활용
	SetMesh(OBJECT.GetOrCreateMesh("GizmoLocation", arrow_vertices));
	SetMesh(OBJECT.GetOrCreateMesh("GizmoRotate", rotate_ring_vertices));
	SetMesh(OBJECT.GetOrCreateMesh("GizmoScale", scale_axis_vertices));


	transform.SetScale({ 0.7f, 0.7f, 0.7f });
	transform.SetLocation({ 0.0f, 0.0f, 0.0f });

	switch (Axis)
	{
	case EGizmoAxis::X:
		Color = FLinearColor::Red;
		transform.SetRotationEuler({ 0.0f, 0.0f, -DirectX::XM_PIDIV2 });
		break;
	case EGizmoAxis::Y:
		Color = FLinearColor::Green;
		transform.SetRotationEuler({ 0.0f, 0.0f, 0.0f });
		break;
	case EGizmoAxis::Z:
		Color = FLinearColor::Blue;
		transform.SetRotationEuler({ DirectX::XM_PIDIV2, 0.0f, 0.0f });
		break;
	default:
		Color = FLinearColor::White;
		break;
	}

	srcColor = Color;
}

AGizmoAxis::~AGizmoAxis()
{
	// 공유 메시는 ObjectManager가 관리하며 worldBuffer는 AActor::~AActor()가 해제함
}

void AGizmoAxis::Update(float DeltaTime, const Transform& parentTransform)
{
	FVector camPos = CAMERA.GetLocation();
	FVector gizmoPos = parentTransform.Location;

	float dist = (gizmoPos - camPos).Length();
	float scaleFactor = dist * 0.15f;

	float baseThickness = (std::max)(0.7f * scaleFactor, 0.05f); //최소 0.05 이상
	float axisLength = baseThickness;

	if (mode && *mode == EGizmoMode::Scale && bSelected)
	{
		axisLength = baseThickness + currentDragDist;
		axisLength = (std::max)(axisLength, baseThickness * 0.1f);
	}

	transform.SetScale(FVector(baseThickness, axisLength, baseThickness));

	FMatrix S = FMatrix::Scale(transform.Scale);
	FMatrix R = transform.Rotation.ToMatrix();
	FMatrix localMat = S * R;

	if (bIsLocal)
	{
		// 부모 회전과 위치 적용
		FMatrix parentRot = parentTransform.Rotation.ToMatrix();
		FMatrix parentTrans = FMatrix::Translation(parentTransform.Location);
		transform.SetWorldMatrix(localMat * (parentRot * parentTrans));
	}
	else
	{
		// 부모 위치만 적용
		FMatrix parentTrans = FMatrix::Translation(parentTransform.Location);
		transform.SetWorldMatrix(localMat * parentTrans);
	}

	worldBuffer->SetMat(transform.WorldMat);

	AActor::Update(DeltaTime);

	if (bSelected || bHovered)
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
	if (!mesh)
		return;

	// 깊이 판정 비활성화 (물체에 가려지지 않고 항상 최상단 렌더링)
	RENDERER.SetGizmoDepthState();

	worldBuffer->SetVSBuffer(0);

	switch (*mode)
	{
	case EGizmoMode::Translation:
		SetMesh(OBJECT.GetOrCreateMesh("GizmoLocation", arrow_vertices));

		break;
	case EGizmoMode::Rotation:
		SetMesh(OBJECT.GetOrCreateMesh("GizmoRotate", rotate_ring_vertices));

		break;
	case EGizmoMode::Scale:
		SetMesh(OBJECT.GetOrCreateMesh("GizmoScale", scale_axis_vertices));

		break;
	default:
		break;
	}

	mesh->SetColor(Color);
	mesh->Render();

	// 기본 깊이 상태로 복원
	RENDERER.SetDefaultDepthState();
}


void AGizmoAxis::Picked()
{
	if (!TargetActor) return;

	//축 방향 결정
	FVector localDir(0.0f, 0.0f, 0.0f);
	switch (Axis)
	{
	case EGizmoAxis::X: localDir = FVector(1.0f, 0.0f, 0.0f); break;
	case EGizmoAxis::Y: localDir = FVector(0.0f, 1.0f, 0.0f); break;
	case EGizmoAxis::Z: localDir = FVector(0.0f, 0.0f, 1.0f); break;
	default: break;
	}

	//로컬 모드이면 타겟의 회전을 반영하고, 월드 모드이면 월드 정방향 축 사용
	if (bIsLocal)
	{
		currentAxisDir = TransformDirection(localDir, TargetActor->GetTransform().WorldMat).Normalized();
	}
	else
	{
		currentAxisDir = localDir;
	}


	switch (*mode)
	{
	case EGizmoMode::Translation:

		break;
	case EGizmoMode::Rotation:




		break;
	case EGizmoMode::Scale:
		break;
	default:
		break;
	}

	//평면 법선 벡터 계산
	if (mode && *mode == EGizmoMode::Rotation)
	{
		planeNormal = currentAxisDir;
	}
	else
	{
		FVector cameraDir = CAMERA.GetForward();
		FVector A = FVector::Cross3D(currentAxisDir, cameraDir).Normalized();
		planeNormal = FVector::Cross3D(A, currentAxisDir).Normalized();
	}


	//드래그 시작 지점 및 타겟 초기 위치/회전/스케일 저장
	FRay ray = PICK.ScreenToWorldRay();

	float denom = planeNormal.Dot(ray.Direction);
	if (fabsf(denom) > 1e-6f)
	{
		float t = (TargetActor->GetLocation() - ray.Origin).Dot(planeNormal) / denom;
		dragStartPoint = ray.Origin + ray.Direction * t;
		dragStartActorLocation = TargetActor->GetLocation();
		dragStartActorRotation = TargetActor->GetRotation();
		dragStartActorScale = TargetActor->GetScale();
	}


	//피킹 선택 상태 활성화
	currentDragDist = 0.0f;
	bSelected = true;
	HighlightAxe();
}

void AGizmoAxis::Pressed()
{
	if (!TargetActor) return;

	FRay ray = PICK.ScreenToWorldRay();
	float denom = planeNormal.Dot(ray.Direction);
	if (fabsf(denom) > 1e-6f)
	{
		float t = (dragStartPoint - ray.Origin).Dot(planeNormal) / denom;
		FVector currentHitPoint = ray.Origin + ray.Direction * t;

		if (mode && *mode == EGizmoMode::Rotation) //기즈모가 Rotation일때
		{
			FVector A = dragStartPoint - TargetActor->GetLocation();
			FVector B = currentHitPoint - TargetActor->GetLocation();

			// 단위 벡터 정규화
			FVector vA = A.Normalized();
			FVector vB = B.Normalized();

			// 각도(라디안) 계산
			float cosAlpha = std::clamp(vA.Dot(vB), -1.0f, 1.0f);
			float alpha = acosf(cosAlpha);

			// 외적을 통해 회전축 방향과 일치 여부(시계/반시계) 판별
			FVector cross = FVector::Cross3D(vA, vB);
			if (cross.Dot(currentAxisDir) < 0.0f)
			{
				alpha = -alpha;
			}
			
			// currentAxisDir 기준으로 델타 회전 만들어서 곱함
			FQuaternion deltaRot = FQuaternion::FromAxisAngle(currentAxisDir, alpha);
			// 축에 맞게 회전값 적용
			FQuaternion newRot = (deltaRot *dragStartActorRotation).Normalized();
			
			// 이미 currentAxisDir을 계산해두었기 때문에 확인하지 않아도 괜찮음.
			// switch (Axis)
			// {
			// case EGizmoAxis::X: newRot.x += alpha; break;
			// case EGizmoAxis::Y: newRot.y += alpha; break;
			// case EGizmoAxis::Z: newRot.z += alpha; break;
			// default: break;
			// }

			TargetActor->SetRotation(newRot);
		}
		else if (mode && *mode == EGizmoMode::Scale) //Scale일때
		{
			// 축 방향 드래그 변위 계산
			FVector delta = currentHitPoint - dragStartPoint;
			float moveDist = delta.Dot(currentAxisDir);

			FVector newScale = dragStartActorScale;
			switch (Axis)
			{
			case EGizmoAxis::X: newScale.x += moveDist; break;
			case EGizmoAxis::Y: newScale.y += moveDist; break;
			case EGizmoAxis::Z: newScale.z += moveDist; break;
			default: break;
			}

			// 최소 크기 제한 (음수 또는 0 방지)
			newScale.x = (std::max)(newScale.x, 0.05f);
			newScale.y = (std::max)(newScale.y, 0.05f);
			newScale.z = (std::max)(newScale.z, 0.05f);

			TargetActor->SetScale(newScale);
			currentDragDist = moveDist;
		}
		else //이동일때
		{
			//이동량 계산 및 축 투영 (Translation)
			FVector delta = currentHitPoint - dragStartPoint;
			float moveDist = delta.Dot(currentAxisDir);

			//타겟 위치 갱신 및 월드 행렬 업데이트
			TargetActor->SetLocation(dragStartActorLocation + currentAxisDir * moveDist);
		}
	}

}

void AGizmoAxis::Released()
{
	//피킹 선택 상태 해제
	bSelected = false;
	currentDragDist = 0.0f;
	if (*mode == EGizmoMode::Scale)
	{
		transform.SetScale(FVector(0.7f, 0.7f, 0.7f));
		transform.UpdateWorldMatrix();
	}

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
	Axes.push_back(new AGizmoAxis(GizMode, EGizmoAxis::X));
	Axes.push_back(new AGizmoAxis(GizMode, EGizmoAxis::Y));
	Axes.push_back(new AGizmoAxis(GizMode, EGizmoAxis::Z));
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
			it->SetTargetActor(inTarget);
			it->SetIsLocal(bIsLocal);
		}

		transform.SetLocation(TargetActor->GetLocation());
		transform.Scale = TargetActor->GetScale() * 0.7f;
	}
	else
	{
		for (auto& it : Axes)
		{
			it->SetTargetActor(nullptr);
		}
	}
}

void AGizmo::Update(float DeltaTime)
{
	AActor::Update(DeltaTime);

	//키보드 L 누르면 bIsLocal 토글
	if (KEY_DOWN(ImGuiKey_L))
	{
		bIsLocal = !bIsLocal;
		for (auto* axis : Axes)
		{
			axis->SetIsLocal(bIsLocal);
		}
	}

	//키보드 1, 2, 3 누르면 기즈모 모드 전환 (1: Translation, 2: Rotation, 3: Scale;
	if (KEY_DOWN(ImGuiKey_Space)) ChangeGizmoMode();

	// 피킹된 타겟 액터가 있을 때만 위치 동기화 및 3개 축 업데이트
	if (TargetActor)
	{
		transform.SetLocation(TargetActor->GetLocation());
		for (auto* axis : Axes)
		{
			axis->Update(DeltaTime, TargetActor->transform);
		}

		// 마우스 호버 시 가장 가까운 축 1개만 하이라이트
		FRay ray = PICK.ScreenToWorldRay();
		AGizmoAxis* closestAxis = nullptr;
		float closestDist = FLT_MAX;

		for (auto* axis : Axes)
		{
			axis->SetHovered(false);
			float dist = 0.0f;
			if (axis->bIsPicked(ray, dist))
			{
				if (dist > 0.0f && dist < closestDist)
				{
					closestDist = dist;
					closestAxis = axis;
				}
			}
		}

		if (closestAxis && !MOUSE_PRESS(0))
		{
			closestAxis->SetHovered(true);
		}
	}
	else
	{
		for (auto* axis : Axes)
		{
			axis->SetHovered(false);
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

void AGizmo::ChangeGizmoMode()
{
	int nextMode = (static_cast<int>(GizMode) + 1) % 3;
	SetGizmoMode(static_cast<EGizmoMode>(nextMode));
}
