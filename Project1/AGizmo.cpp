#include "pch.h"
#include "AGizmo.h"
#include "Renderer.h"

AGizmo::AGizmo(EGizmoAxis Axis) //기즈모는 기본이 위를 바라보는 모양
{
	// 기본 화살표 정점으로 버텍스 버퍼 초기화
	InitVertexBuffer(arrow_vertices);
	SelectedAxis = Axis;
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
	RENDERER.PrepareShader(inputLayout);

	//버텍스 버퍼 바인딩
	vertexbuffer->IASet();

	switch (SelectedAxis)
	{
	case EGizmoAxis::None:
		break;
	case EGizmoAxis::X:
		transform.SetRotation({ 0.0f,0.0f, -DirectX::XM_PIDIV2 });
		Color = FLinearColor::Red;
		break;
	case EGizmoAxis::Y:
		Color = FLinearColor::Green;
		break;
	case EGizmoAxis::Z:
		transform.SetRotation({ DirectX::XM_PIDIV2 ,0.0f, 0.0f});
		Color = FLinearColor::Blue;
		break;
	case EGizmoAxis::All:
		break;
	default:
		break;
	}

	worldBuffer->SetMat(transform.WorldMat);
	worldBuffer->SetVSBuffer(0);

	RENDERER.SetCustomColor(Color);
	
	DC->Draw(numVertices, 0);
}

void AGizmo::RenderAxis(const FMatrix& localRotation, EGizmoAxis axisType)
{
	//FMatrix S = FMatrix::Scale(transform.Scale * GizmoScale);
	//FMatrix T = FMatrix::Translation(transform.Location);

	//FMatrix axisWorld = localRotation * S * T;

	//FLinearColor axisColor = FLinearColor::White;
	//if (axisType == EGizmoAxis::X) axisColor = FLinearColor::Red;
	//else if (axisType == EGizmoAxis::Y) axisColor = FLinearColor::Green;
	//else if (axisType == EGizmoAxis::Z) axisColor = FLinearColor::Blue;

	//worldBuffer->SetMat(axisWorld);
	//worldBuffer->SetVSBuffer(0);

	//REDERER.SetCustomColor(axisColor);

	//DC->Draw(numVertices, 0);
}

void AGizmo::Pressed(FVector _Location)
{
	// 마우스 클릭 시 피킹/축 선택 처리 로직
}

void AGizmo::Released(FVector _Location)
{
	SelectedAxis = EGizmoAxis::None;
}
