#pragma once

#include "AActor.h"
#include "FVertexSimple.h"
#include "Sphere.h"

struct FRay;

// AActor를 상속받아 bIsPicked(정점 피킹) 및 트랜스폼/버퍼를 그대로 활용하는 기즈모 축 액터
class AGizmoAxis : public AActor
{
	DECLARE_CLASS(AGizmoAxis, AActor)

public:
	AGizmoAxis(EGizmoAxis inAxis = EGizmoAxis::Y);
	virtual ~AGizmoAxis();

	void Update(float DeltaTime, const Transform& parentTransform);
	virtual void Render() override;

	EGizmoAxis GetAxis() const { return Axis; }
	void Picked(); // 피킹되었을 때의 처리

	EPrimitive Primitive = EPrimitive::None;


private:
	EGizmoAxis Axis = EGizmoAxis::None;
};

// 씬에 단 하나 생성되어 피킹된 액터에 부착되는 통합 기즈모 액터
class AGizmo : public AActor
{
	DECLARE_CLASS(AGizmo, AActor)

public:
	// 씬에 존재하는 유일한 기즈모 인스턴스 (어디서든 즉시 접근 가능)
	static inline AGizmo* MainGizmo = nullptr;

	AGizmo();
		
	virtual ~AGizmo();

	virtual void Update(float DeltaTime) override;
	virtual void Render() override;

	// 타겟 액터 설정 (피킹된 액터 연결)
	void SetTargetActor(AActor* inTarget);
	AActor* GetTargetActor() const { return TargetActor; }

	void SetGizmoMode(EGizmoMode inMode) { Mode = inMode; }
	EGizmoMode GetGizmoMode() const { return Mode; }

	void SetSelectedAxis(EGizmoAxis inAxis) { SelectedAxis = inAxis; }
	EGizmoAxis GetSelectedAxis() const { return SelectedAxis; }

	// 광선(Ray)과 기즈모 축들 간의 피킹 검사: 가장 가까이 클릭된 축 반환
	EGizmoAxis PickAxis(const FRay& ray, float& outDist);

	virtual void Pressed(FVector _Location) override;
	virtual void Released(FVector _Location) override;

	// 3개의 기즈모 축 객체 목록 반환
	const TArray<AGizmoAxis*>& GetAxes() const { return Axes; }

private:
	AActor* TargetActor = nullptr;
	EGizmoMode Mode = EGizmoMode::Translation;
	EGizmoAxis SelectedAxis = EGizmoAxis::None;

	// 3개의 기즈모 축 액터 (X, Y, Z)
	TArray<AGizmoAxis*> Axes;
};

inline FLinearColor Highlighting(const FLinearColor& color) {
	float t = 0.5f;

	FLinearColor result = color;

	result.r = color.r + (1.0f - color.r) * t;
	result.g = color.g + (1.0f - color.g) * t;
	result.b = color.b + (1.0f - color.b) * t;
	result.a = color.a;
	return result;
}