#pragma once

#include "AActor.h"
#include "Sphere.h"

enum class EGizmoAxis
{
	None,
	X,
	Y,
	Z,
	All
};

enum class EGizmoMode
{
	Translation,
	Rotation,
	Scale
};

class AGizmo : public AActor
{
	DECLARE_CLASS(AGizmo, AActor)

public:
	AGizmo();
	virtual ~AGizmo();

	virtual void Render() override;
	virtual void Update(float DeltaTime) override;

	// 조작할 타겟 액터 설정/조회
	void SetTargetActor(AActor* inTarget) { TargetActor = inTarget; }
	AActor* GetTargetActor() const { return TargetActor; }

	// 기즈모 모드 및 활성 축 설정
	void SetGizmoMode(EGizmoMode inMode) { Mode = inMode; }
	EGizmoMode GetGizmoMode() const { return Mode; }

	void SetSelectedAxis(EGizmoAxis inAxis) { SelectedAxis = inAxis; }
	EGizmoAxis GetSelectedAxis() const { return SelectedAxis; }

	// 버텍스 버퍼 수동 지정 지원
	void SetVB(void* vertexData, UINT vertexStride, UINT vertexCount);

	// 마우스 인터랙션 이벤트
	virtual void Pressed(FVector _Location) override;
	virtual void Released(FVector _Location) override;

private:
	// 각 축 렌더링 헬퍼 함수
	void RenderAxis(const FMatrix& localRotation, EGizmoAxis axisType);

private:
	AActor* TargetActor = nullptr;
	EGizmoMode Mode = EGizmoMode::Translation;
	EGizmoAxis SelectedAxis = EGizmoAxis::None;

	float GizmoScale = 1.0f;
};


