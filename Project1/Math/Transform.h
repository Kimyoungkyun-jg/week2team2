#pragma once

#include "FVector.h"
#include "FMatrix.h"
#include "FQuaternion.h"

class Transform
{
public:
	Transform()
		: Location(0.0f, 0.0f, 0.0f)
		, Rotation()
		, Scale(1.0f, 1.0f, 1.0f)
		, WorldMat(FMatrix::Identity())
	{
		UpdateWorldMatrix();
	}
	
	// 쿼터니언을 입력으로 받는 생성자
	Transform(const FVector& InLocation, const FQuaternion& InRotation, const FVector& InScale = FVector(1.0f, 1.0f, 1.0f))
		: Location(InLocation)
		, Rotation(InRotation)
		, Scale(InScale)
		, WorldMat(FMatrix::Identity())
	{
		UpdateWorldMatrix();
	}

	// 오일러를 입력으로 받는 생성자
	Transform(const FVector& InLocation, const FVector& InRotation = FVector(0.0f, 0.0f, 0.0f), const FVector& InScale = FVector(1.0f, 1.0f, 1.0f))
		: Location(InLocation)
		, Rotation(FQuaternion::FromEuler(InRotation.x, InRotation.y, InRotation.z))
		, Scale(InScale)
		, WorldMat(FMatrix::Identity())
	{
		UpdateWorldMatrix();
	}

	void UpdateWorldMatrix()
	{
		FMatrix S = FMatrix::Scale(Scale);
		FMatrix R = Rotation.ToMatrix();
		FMatrix T = FMatrix::Translation(Location);

		WorldMat = S * R * T;

		if (Parent)
		{
			if (bInheritScale)
			{
				WorldMat = WorldMat * Parent->WorldMat;
			}
			else
			{
				FMatrix parentRot = Parent->Rotation.ToMatrix();
				FMatrix parentTrans = FMatrix::Translation(Parent->Location);

				WorldMat = WorldMat * (parentRot * parentTrans);
			}
		}
	}

	void SetParent(const Transform* InParent, bool inInheritScale = false)
	{
		Parent = InParent;
		bInheritScale = inInheritScale;
		UpdateWorldMatrix();
	}

	const Transform* GetParent() const { return Parent; }

	void SetLocation(const FVector& InLocation) { Location = InLocation; UpdateWorldMatrix(); }
	void SetRotationEuler(const FVector& InRotation)
	{
		Rotation = FQuaternion::FromEuler(InRotation.x, InRotation.y, InRotation.z);
		UpdateWorldMatrix();
	}
	void SetRotation(const FQuaternion& InRotation) { Rotation = InRotation; UpdateWorldMatrix(); }
	void SetScale(const FVector& InScale) { Scale = InScale; UpdateWorldMatrix(); }

	const FVector& GetLocation() const { return Location; }
	const FQuaternion& GetRotation() const { return Rotation; }
	FVector GetRotationEuler() const { return FQuaternion::ToEuler(Rotation); }
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
	// FVector Rotation;	// 오일러
	FQuaternion Rotation; 	// 쿼터니언
	FVector Scale;
	FMatrix WorldMat;

	const Transform* Parent = nullptr;
	bool bInheritScale = false;
};

