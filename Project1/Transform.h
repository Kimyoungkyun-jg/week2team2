#pragma once

#include "FVector.h"
#include "Matrix.h"

class Transform
{
public:
	Transform()
		: Location(0.0f, 0.0f, 0.0f)
		, Rotation(0.0f, 0.0f, 0.0f)
		, Scale(1.0f, 1.0f, 1.0f)
		, WorldMat(FMatrix::Identity())
	{
		UpdateWorldMatrix();
	}

	Transform(const FVector& InLocation, const FVector& InRotation = FVector(0.0f, 0.0f, 0.0f), const FVector& InScale = FVector(1.0f, 1.0f, 1.0f))
		: Location(InLocation)
		, Rotation(InRotation)
		, Scale(InScale)
		, WorldMat(FMatrix::Identity())
	{
		UpdateWorldMatrix();
	}

	void UpdateWorldMatrix()
	{
		FMatrix S = FMatrix::Scale(Scale);
		FMatrix R = FMatrix::RotationZ(Rotation.z) * FMatrix::RotationX(Rotation.x) * FMatrix::RotationY(Rotation.y);
		FMatrix T = FMatrix::Translation(Location);

		WorldMat = S * R * T;
	}

	void SetLocation(const FVector& InLocation) { Location = InLocation; UpdateWorldMatrix(); }
	void SetRotation(const FVector& InRotation) { Rotation = InRotation; UpdateWorldMatrix(); }
	void SetScale(const FVector& InScale) { Scale = InScale; UpdateWorldMatrix(); }

	const FVector& GetLocation() const { return Location; }
	const FVector& GetRotation() const { return Rotation; }
	const FVector& GetScale() const { return Scale; }

	void SetWorldMatrix(const FMatrix& InWorldMatrix) { WorldMat = InWorldMatrix; }
	const FMatrix& GetWorldMatrix() const { return WorldMat; }


	FVector Forward() const //현재 상태에서 앞 (+Z)
	{
		FVector v(WorldMat.M[2][0], WorldMat.M[2][1], WorldMat.M[2][2]);
		v.Normalize();
		return v;
	}

	FVector Up() const //현재 상태에서 위 (+Y)
	{
		FVector v(WorldMat.M[1][0], WorldMat.M[1][1], WorldMat.M[1][2]);
		v.Normalize();
		return v;
	}

	FVector Right() const//현재 상태에서 오른쪽 (+X)
	{
		FVector v(WorldMat.M[0][0], WorldMat.M[0][1], WorldMat.M[0][2]);
		v.Normalize();
		return v;
	}


public:
	FVector Location;
	FVector Rotation;
	FVector Scale;
	FMatrix WorldMat;
};
