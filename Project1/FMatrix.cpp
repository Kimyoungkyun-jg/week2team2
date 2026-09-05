#include "pch.h"
#include "FMatrix.h"
#include <cmath>

FMatrix::FMatrix() {

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {

            if (i == j) M[i][j] = 1;
            else M[i][j] = 0;
        }
    }
    
}

FMatrix FMatrix::Identity() {

    return FMatrix();
}

FMatrix FMatrix::Translation(const FVector& Location)
{
	FMatrix Result = Identity();

	Result.M[3][0] = Location.x;
	Result.M[3][1] = Location.y;
	Result.M[3][2] = Location.z;

	return Result;
}

FMatrix FMatrix::Scale(const FVector& Scale)
{
	FMatrix Result = Identity();

	Result.M[0][0] = Scale.x;
	Result.M[1][1] = Scale.y;
	Result.M[2][2] = Scale.z;

	return Result;
}

FMatrix FMatrix::RotationX(float Radian)
{
	FMatrix Result = Identity();

	float C = cosf(Radian);
	float S = sinf(Radian);

	Result.M[1][1] = C;
	Result.M[1][2] = S;
	Result.M[2][1] = -S;
	Result.M[2][2] = C;

	return Result;
}

FMatrix FMatrix::RotationY(float Radian)
{
	FMatrix Result = Identity();

	float C = cosf(Radian);
	float S = sinf(Radian);

	Result.M[0][0] = C;
	Result.M[0][2] = -S;
	Result.M[2][0] = S;
	Result.M[2][2] = C;

	return Result;
}

FMatrix FMatrix::RotationZ(float Radian)
{
	FMatrix Result = Identity();

	float C = cosf(Radian);
	float S = sinf(Radian);

	Result.M[0][0] = C;
	Result.M[0][1] = S;
	Result.M[1][0] = -S;
	Result.M[1][1] = C;

	return Result;
}

FMatrix FMatrix::Perspective(
	float FovY,
	float AspectRatio,
	float NearZ,
	float FarZ)
{
	FMatrix Result;

	// Identity로 초기화되어 있으므로 전부 0으로 초기화
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Result.M[i][j] = 0.0f;
		}
	}

	float YScale = 1.0f / tanf(FovY * 0.5f);
	float XScale = YScale / AspectRatio;

	Result.M[0][0] = XScale;
	Result.M[1][1] = YScale;

	Result.M[2][2] = FarZ / (FarZ - NearZ);
	Result.M[2][3] = 1.0f;

	Result.M[3][2] =
		(-NearZ * FarZ) / (FarZ - NearZ);

	return Result;
}

FMatrix FMatrix::LookAt(
	const FVector& Eye,
	const FVector& Target,
	const FVector& Up)
{
	// Forward
	FVector ZAxis(
		Target.x - Eye.x,
		Target.y - Eye.y,
		Target.z - Eye.z
	);

	float ZLength = sqrtf(
		ZAxis.x * ZAxis.x +
		ZAxis.y * ZAxis.y +
		ZAxis.z * ZAxis.z
	);

	ZAxis.x /= ZLength;
	ZAxis.y /= ZLength;
	ZAxis.z /= ZLength;

	// Right = Up x Forward
	FVector XAxis(
		Up.y * ZAxis.z - Up.z * ZAxis.y,
		Up.z * ZAxis.x - Up.x * ZAxis.z,
		Up.x * ZAxis.y - Up.y * ZAxis.x
	);

	float XLength = sqrtf(
		XAxis.x * XAxis.x +
		XAxis.y * XAxis.y +
		XAxis.z * XAxis.z
	);

	XAxis.x /= XLength;
	XAxis.y /= XLength;
	XAxis.z /= XLength;

	// Camera Up = Forward x Right
	FVector YAxis(
		ZAxis.y * XAxis.z - ZAxis.z * XAxis.y,
		ZAxis.z * XAxis.x - ZAxis.x * XAxis.z,
		ZAxis.x * XAxis.y - ZAxis.y * XAxis.x
	);

	FMatrix Result = Identity();

	Result.M[0][0] = XAxis.x;
	Result.M[0][1] = YAxis.x;
	Result.M[0][2] = ZAxis.x;

	Result.M[1][0] = XAxis.y;
	Result.M[1][1] = YAxis.y;
	Result.M[1][2] = ZAxis.y;

	Result.M[2][0] = XAxis.z;
	Result.M[2][1] = YAxis.z;
	Result.M[2][2] = ZAxis.z;

	Result.M[3][0] = -(
		Eye.x * XAxis.x +
		Eye.y * XAxis.y +
		Eye.z * XAxis.z
		);

	Result.M[3][1] = -(
		Eye.x * YAxis.x +
		Eye.y * YAxis.y +
		Eye.z * YAxis.z
		);

	Result.M[3][2] = -(
		Eye.x * ZAxis.x +
		Eye.y * ZAxis.y +
		Eye.z * ZAxis.z
		);

	return Result;
}

FMatrix FMatrix::operator*(const FMatrix& Other) const
{
	FMatrix Result;

	for (int Row = 0; Row < 4; Row++)
	{
		for (int Col = 0; Col < 4; Col++)
		{
			Result.M[Row][Col] = 0.0f;

			for (int K = 0; K < 4; K++)
			{
				Result.M[Row][Col] +=
					M[Row][K] * Other.M[K][Col];
			}
		}
	}

	return Result;
}