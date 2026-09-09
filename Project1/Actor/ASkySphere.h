#pragma once
#include "AActor.h"
#include "Sphere.h"
#include "Camera.h"
#include "Renderer.h"

class ASkySphere : public AActor
{
	DECLARE_CLASS(ASkySphere, AActor);

public:
	ASkySphere()
	{
		// 오브젝트 매니저를 통해 스카이스피어 메시 생성 및 캐시
		SetMesh(OBJECT.GetOrCreateMesh("SkySphere", skysphere_vertices));

		mesh->SetTexture(L"Resources/Textures/Sky.jpg");

		// 카메라를 둘러싸는 기본 크기 설정
		SetScale(FVector(500.0f, 500.0f, 500.0f));
	}

	virtual ~ASkySphere() = default;

	virtual void Update(float deltaTime) override
	{
		AActor::Update(deltaTime);

	}

	virtual void Render() override
	{
		if (!mesh) return;

		// 렌더링 직전 카메라 위치 동기화 및 월드 행렬 갱신
		SetLocation(CAMERA.GetLocation());
		SetWorldBuffer();

		// 텍스처 바인딩
		if (mesh->GetTexture())
		{
			RENDERER.SetTexture(mesh->GetTexture());
		}

		// 스카이스피어 전용 셰이더 및 파이프라인 설정
		RENDERER.PrepareSkyShader(mesh->GetInputLayout());
		RENDERER.SetSkyDepthState();

		// 버텍스 버퍼 바인딩 및 렌더링
		if (mesh->GetVertexBuffer())
		{
			mesh->GetVertexBuffer()->IASet();
			RENDERER.GetDeviceContext()->Draw(mesh->GetNumVertices(), 0);
		}

		// 기본 상태 복원
		RENDERER.SetDefaultDepthState();
		RENDERER.SetTexture(nullptr);
	}

	// 하늘 객체는 마우스 피킹 대상에서 제외
	virtual bool bIsPicked(const FRay& ray, float& outDistance) override
	{
		return false;
	}

};
