#pragma once

#include "UObject.h"
#include "Transform.h"
#include "VertexBuffer.h"
#include "GlobalBuffer.h"
#include "FVertexSimple.h"
#include "Sphere.h"

class AActor;

class UGizmo : public UObject
{
	DECLARE_CLASS(UGizmo, UObject)

public:
	UGizmo(EGizmoAxis Axis = EGizmoAxis::Y, AActor* target = nullptr);
	virtual ~UGizmo();

	virtual void Render() override;
	virtual void Update(float DeltaTime) override;

	// 트랜스폼 및 위치/회전/크기 제어
	Transform& GetTransform() { return transform; }
	const Transform& GetTransform() const { return transform; }
	void SetLocation(const FVector& loc) { transform.SetLocation(loc); }
	const FVector& GetLocation() const { return transform.GetLocation(); }
	void SetRotation(const FVector& rot) { transform.SetRotation(rot); }
	const FVector& GetRotation() const { return transform.GetRotation(); }
	void SetScale(const FVector& scale) { transform.SetScale(scale); }
	const FVector& GetScale() const { return transform.GetScale(); }

	// 조작할 타겟 액터 설정/조회
	void SetTargetActor(AActor* inTarget) { TargetActor = inTarget; }
	AActor* GetTargetActor() const { return TargetActor; }

	// 기즈모 모드 및 활성 축 설정
	void SetGizmoMode(EGizmoMode inMode) { Mode = inMode; }
	EGizmoMode GetGizmoMode() const { return Mode; }

	void SetSelectedAxis(EGizmoAxis inAxis) { SelectedAxis = inAxis; }
	EGizmoAxis GetSelectedAxis() const { return SelectedAxis; }

	// 버텍스 버퍼 초기화 함수
	void InitVertexBuffer(const void* vertices, UINT stride, UINT inNumVertices, ID3D11InputLayout* inLayout = nullptr);
	template <typename VertexType, size_t N>
	void InitVertexBuffer(const VertexType (&vertices)[N])
	{
		InitVertexBuffer(vertices, sizeof(VertexType), static_cast<UINT>(N), RENDERER.GetInputLayout<VertexType>());
	}

	void SetVB(void* vertexData, UINT vertexStride, UINT vertexCount);

	// 마우스 인터랙션 이벤트
	virtual void Pressed(FVector _Location);
	virtual void Released(FVector _Location);

private:
	void RenderAxis(const FMatrix& localRotation, EGizmoAxis axisType);

public:
	Transform transform;
	MatrixBuffer* worldBuffer = nullptr;
	VertexBuffer* vertexbuffer = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	FLinearColor Color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UINT numVertices = 0;

private:
	AActor* TargetActor = nullptr;
	EGizmoMode Mode = EGizmoMode::Translation;
	EGizmoAxis SelectedAxis = EGizmoAxis::None;
	float GizmoScale = 1.0f;
};
