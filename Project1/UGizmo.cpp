#include "pch.h"
#include "UGizmo.h"
#include "AActor.h"
#include "Renderer.h"

UGizmo::UGizmo(EGizmoAxis Axis, AActor* target)
{
	worldBuffer = new MatrixBuffer();
	InitVertexBuffer(arrow_vertices);
	SelectedAxis = Axis;
	SetTargetActor(target);
}

UGizmo::~UGizmo()
{
	delete worldBuffer;
	worldBuffer = nullptr;

	if (vertexbuffer)
	{
		delete vertexbuffer;
		vertexbuffer = nullptr;
	}
}

void UGizmo::InitVertexBuffer(const void* vertices, UINT stride, UINT inNumVertices, ID3D11InputLayout* inLayout)
{
	numVertices = inNumVertices;
	inputLayout = inLayout;

	if (vertexbuffer)
	{
		delete vertexbuffer;
		vertexbuffer = nullptr;
	}

	if (vertices && inNumVertices > 0)
	{
		vertexbuffer = new VertexBuffer(vertices, stride, inNumVertices);
	}
}

void UGizmo::Update(float DeltaTime)
{
	UObject::Update(DeltaTime);

	// 타겟 액터가 존재하면 기즈모 위치를 타겟 위치로 동기화
	if (TargetActor)
	{
		transform.SetLocation(TargetActor->GetLocation());
	}
}

void UGizmo::Render()
{
	UObject::Render();
	//버텍스 버퍼가 없으면 렌더링하지 않음
	if (vertexbuffer == nullptr || numVertices == 0)
		return;

	//깊이 판정 비활성화 (도형에 가려지지 않고 항상 최상단 렌더링)
	RENDERER.SetGizmoDepthState();

	//저장된 InputLayout으로 셰이더 및 레이아웃 바인딩
	RENDERER.PrepareShader(inputLayout);

	//버텍스 버퍼 바인딩
	vertexbuffer->IASet();

	switch (SelectedAxis)
	{
	case EGizmoAxis::None:
		break;
	case EGizmoAxis::X:
		transform.SetRotation({ 0.0f, 0.0f, -DirectX::XM_PIDIV2 });
		Color = FLinearColor::Red;
		break;
	case EGizmoAxis::Y:
		Color = FLinearColor::Green;
		break;
	case EGizmoAxis::Z:
		transform.SetRotation({ DirectX::XM_PIDIV2, 0.0f, 0.0f });
		Color = FLinearColor::Blue;
		break;
	default:
		break;
	}

	worldBuffer->SetMat(transform.WorldMat);
	worldBuffer->SetVSBuffer(0);

	RENDERER.SetCustomColor(Color);
	
	DC->Draw(numVertices, 0);

	// 2. 기본 깊이 상태로 복원
	RENDERER.SetDefaultDepthState();
}

void UGizmo::Pressed(FVector _Location)
{
	// 마우스 클릭 시 피킹/축 선택 처리 로직
}

void UGizmo::Released(FVector _Location)
{
	SelectedAxis = EGizmoAxis::None;
}
