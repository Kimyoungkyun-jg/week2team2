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


	virtual void Pressed() override;
	virtual void Released() override;

	void SetTargetActor(Transform* targettf)
	{
		Targettransform = targettf;
	}
private:
	EGizmoAxis Axis = EGizmoAxis::None;
	Transform* Targettransform = nullptr;
	FVector planeNormal; 
	FVector currentAxisDir;
	FVector dragStartPoint;
	FVector dragStartActorLocation;
};

// 씬에 단 하나 생성되어 피킹된 액터에 부착되는 통합 기즈모 액터
class AGizmo : public AActor
{
	DECLARE_CLASS(AGizmo, AActor)

public:
	//씬의 메인 기즈모 인스턴스
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

	//광선(Ray)과 기즈모 축들 간의 피킹 검사: 가장 가까이 클릭된 축 반환
	EGizmoAxis PickAxis(const FRay& ray, float& outDist);


	// 3개의 기즈모 축 객체 목록 반환
	const TArray<AGizmoAxis*>& GetAxes() const { return Axes; }

private:
	AActor* TargetActor = nullptr;
	EGizmoMode Mode = EGizmoMode::Translation;
	EGizmoAxis SelectedAxis = EGizmoAxis::None;

	// 3개의 기즈모 축 액터 (X, Y, Z)
	TArray<AGizmoAxis*> Axes;
};
