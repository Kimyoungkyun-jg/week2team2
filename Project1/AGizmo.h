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
	AGizmoAxis(EGizmoMode& mode, EGizmoAxis inAxis = EGizmoAxis::Y);
	virtual ~AGizmoAxis();

	void Update(float DeltaTime, const Transform& parentTransform);
	virtual void Render() override;

	EGizmoAxis GetAxis() const { return Axis; }
	void Picked(); // 피킹되었을 때의 처리


	virtual void Pressed() override;
	virtual void Released() override;

	void SetTargetActor(AActor* inTarget)
	{
		TargetActor = inTarget;
		if (TargetActor)
		{
			transform.SetParent(&TargetActor->GetTransform(), false);
		}
		else
		{
			transform.SetParent(nullptr, false);
		}
	}
	AActor* GetTargetActor() const { return TargetActor; }


	void HighlightAxe();

	void SetHovered(bool inHovered)
	{
		bHovered = inHovered;
		if (bSelected || bHovered)
		{
			HighlightAxe();
		}
		else
		{
			SetColor(srcColor);
		}
	}
	bool GetHovered() const { return bHovered; }

	void SetIsLocal(bool inIsLocal) { bIsLocal = inIsLocal; }
	bool GetIsLocal() const { return bIsLocal; }

	FLinearColor srcColor;

private:
	EGizmoAxis Axis = EGizmoAxis::None;
	AActor* TargetActor = nullptr;
	FVector planeNormal; 
	FVector currentAxisDir;
	FVector dragStartPoint;
	FVector dragStartActorLocation;
	FVector dragStartActorRotation;
	FVector dragStartActorScale;
	bool bSelected = false;
	bool bHovered = false;
	bool bIsLocal = true;
	float currentDragDist = 0.0f;

	EGizmoMode* mode;
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

	void SetGizmoMode(EGizmoMode inMode) { GizMode = inMode; }
	EGizmoMode GetGizmoMode() const { return GizMode; }
	void ChangeGizmoMode();

	void SetSelectedAxis(EGizmoAxis inAxis) { SelectedAxis = inAxis; }
	EGizmoAxis GetSelectedAxis() const { return SelectedAxis; }

	//광선(Ray)과 기즈모 축들 간의 피킹 검사: 가장 가까이 클릭된 축 반환
	EGizmoAxis PickAxis(const FRay& ray, float& outDist);


	// 3개의 기즈모 축 객체 목록 반환
	const TArray<AGizmoAxis*>& GetAxes() const { return Axes; }

	bool GetIsLocal() const { return bIsLocal; }
	void SetIsLocal(bool inIsLocal) { bIsLocal = inIsLocal; }

private:
	AActor* TargetActor = nullptr;
	EGizmoMode GizMode = EGizmoMode::Translation;
	EGizmoAxis SelectedAxis = EGizmoAxis::None;
	bool bIsLocal = true;

	// 3개의 기즈모 축 액터 (X, Y, Z)
	TArray<AGizmoAxis*> Axes;



};

inline FLinearColor Highlighting(const FLinearColor& color) {
	float t = 0.5f;

	FLinearColor result = color;

	//백화 방지 색상 상한선 제한
	result.r = std::clamp(color.r + (1.0f - color.r) * t, 0.0f, 0.85f);
	result.g = std::clamp(color.g + (1.0f - color.g) * t, 0.0f, 0.85f);
	result.b = std::clamp(color.b + (1.0f - color.b) * t, 0.0f, 0.85f);
	result.a = color.a;
	return result;
}