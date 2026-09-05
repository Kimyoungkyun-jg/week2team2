#pragma once
#include "FVector.h"

struct FMatrix
{
	float M[4][4];

	FMatrix();

	// 기본
	static FMatrix Identity();

	// 이동
	static FMatrix Translation(const FVector& Location);

	// 크기
	static FMatrix Scale(const FVector& Scale);

	// 회전
	static FMatrix RotationX(float Radian);
	static FMatrix RotationY(float Radian);
	static FMatrix RotationZ(float Radian);


	static FMatrix Perspective(
		float FovY,
		float AspectRatio,
		float NearZ,
		float FarZ
	);

	static FMatrix LookAt(
		const FVector& Eye,
		const FVector& Target,
		const FVector& Up
	);
	
	FMatrix operator*(const FMatrix& Other) const;
};
