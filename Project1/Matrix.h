#pragma once
#include "Vector.h"

struct FMatrix
{
    float M[4][4];

    // 단위 행렬
    static FMatrix Identity();

    // 변환 행렬
    static FMatrix Translation(const FVector& t);    // (x, y, z) 만큼 이동
    static FMatrix Scale(const FVector& s);          // (x, y, z) 배만큼 확대
    static FMatrix RotationX(float radians);         // x축을 radian 만큼 회전
    static FMatrix RotationY(float radians);         // y축을 radian 만큼 회전
    static FMatrix RotationZ(float radians);         // z축을 radian 만큼 회전

    // 카메라 / 투영
    static FMatrix LookAt(const FVector& eye, const FVector& target, const FVector up);     // (보는 방향 x, 타겟 방향 y, 위쪽 방향 z)
    static FMatrix PerspectiveFov(float fovY, float aspectRatio, float nearZ, float farZ);  // (시야각(FoV), 화면비, 거리 범위)

    // 행렬곱 / 전치행렬
    FMatrix operator* (const FMatrix& other) const; 
    FMatrix Transpose() const;

};


/*
struct FMatrix
{
	double M[4][4] = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} };
    
    public:
	FMatrix() = default;
    
    public:
	static FMatrix Identity;
};
*/