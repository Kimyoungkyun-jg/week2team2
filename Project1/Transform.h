#pragma once

#include "FVector.h"
#include <DirectXMath.h>

class Transform
{
public:
	Transform()
		: Location(0.0f, 0.0f, 0.0f)
		, Rotation(0.0f, 0.0f, 0.0f)
		, Scale(1.0f, 1.0f, 1.0f)
		, WorldMatrix(DirectX::XMMatrixIdentity())
	{
	}

	Transform(const FVector& InLocation, const FVector& InRotation = FVector(0.0f, 0.0f, 0.0f), const FVector& InScale = FVector(1.0f, 1.0f, 1.0f))
		: Location(InLocation)
		, Rotation(InRotation)
		, Scale(InScale)
		, WorldMatrix(DirectX::XMMatrixIdentity())
	{
		UpdateWorldMatrix();
	}

	void UpdateWorldMatrix()
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(Location.x, Location.y, Location.z);

		WorldMatrix = S * R * T;
	}

	void SetLocation(const FVector& InLocation) { Location = InLocation; }
	void SetRotation(const FVector& InRotation) { Rotation = InRotation; }
	void SetScale(const FVector& InScale) { Scale = InScale; }

	const FVector& GetLocation() const { return Location; }
	const FVector& GetRotation() const { return Rotation; }
	const FVector& GetScale() const { return Scale; }

	void SetWorldMatrix(const DirectX::XMMATRIX& InWorldMatrix) { WorldMatrix = InWorldMatrix; }
	const DirectX::XMMATRIX& GetWorldMatrix() const { return WorldMatrix; }


	FVector Forward() const //현재 상태에서 앞
	{
		DirectX::XMVECTOR v = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), WorldMatrix));
		DirectX::XMFLOAT3 f3;
		DirectX::XMStoreFloat3(&f3, v);
		return FVector(f3.x, f3.y, f3.z);
	}

	FVector Up() const //현재 상태에서 위
	{
		DirectX::XMVECTOR v = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), WorldMatrix));
		DirectX::XMFLOAT3 f3;
		DirectX::XMStoreFloat3(&f3, v);
		return FVector(f3.x, f3.y, f3.z);
	}

	FVector Right() const//현재 상태에서 오른쪽
	{
		DirectX::XMVECTOR v = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), WorldMatrix));
		DirectX::XMFLOAT3 f3;
		DirectX::XMStoreFloat3(&f3, v);
		return FVector(f3.x, f3.y, f3.z);
	}


public:
	FVector Location;
	FVector Rotation;
	FVector Scale;
	DirectX::XMMATRIX WorldMatrix;

	
};
