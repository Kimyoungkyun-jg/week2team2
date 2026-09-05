#include "pch.h"
#include "AGizmo.h"
#include "Renderer.h"

AGizmo::AGizmo()
{
	// 기본 화살표 정점으로 버텍스 버퍼 초기화
	InitVertexBuffer(arrow_vertices);
}

AGizmo::~AGizmo()
{
}


void AGizmo::Update(float DeltaTime)
{
	AActor::Update(DeltaTime);

	// 타겟 액터가 존재하면 기즈모 위치를 타겟 위치로 동기화
	if (TargetActor)
	{
		transform.SetLocation(TargetActor->GetLocation());
	}
}

void AGizmo::Render()
{
	UObject::Render();
	// 버텍스 버퍼가 없으면 렌더링하지 않음
	if (vertexbuffer == nullptr || numVertices == 0)
		return;

	// 저장된 InputLayout으로 셰이더 및 레이아웃 바인딩
	REDERER.PrepareShader(inputLayout);

	//버텍스 버퍼 바인딩
	vertexbuffer->IASet();

	//Y축 (기본 위쪽 방향 +Y)
	FMatrix YRot = FMatrix::Identity();
	RenderAxis(YRot, EGizmoAxis::Y);

	// 2. X축 (+X 우측 방향: Z축 기준 -90도 회전)
	FMatrix XRot = FMatrix::RotationZ(-DirectX::XM_PIDIV2);
	RenderAxis(XRot, EGizmoAxis::X);

	// 3. Z축 (+Z 전방 방향: X축 기준 +90도 회전)
	FMatrix ZRot = FMatrix::RotationX(DirectX::XM_PIDIV2);
	RenderAxis(ZRot, EGizmoAxis::Z);
}

void AGizmo::RenderAxis(const FMatrix& localRotation, EGizmoAxis axisType)
{
	FMatrix S = FMatrix::Scale(transform.Scale * GizmoScale);
	FMatrix T = FMatrix::Translation(transform.Location);

	FMatrix axisWorld = localRotation * S * T;

	worldBuffer->SetMat(axisWorld);
	worldBuffer->SetVSBuffer(0);

	DC->Draw(numVertices, 0);
}

void AGizmo::Pressed(FVector _Location)
{
	// 마우스 클릭 시 피킹/축 선택 처리 로직
}

void AGizmo::Released(FVector _Location)
{
	SelectedAxis = EGizmoAxis::None;
}
