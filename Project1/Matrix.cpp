#include "pch.h"
#include "Matrix.h"
#include "FVector.h"
#include <cmath>

    // 단위 행렬
    FMatrix FMatrix::Identity()
    {
        FMatrix result = {{ {1,0,0,0},
                            {0,1,0,0},
                            {0,0,1,0},
                            {0,0,0,1} }};    
        return result;
    }

    // (t.x, t.y, t.z) 만큼 이동
    FMatrix FMatrix::Translation(const FVector& t)
    {
        FMatrix result = Identity();
        result.M[3][0] = t.x;
        result.M[3][1] = t.y;
        result.M[3][2] = t.z;
        return result;
    }

    // (s.x, s.y, s.z) 배만큼 Scaling
    FMatrix FMatrix::Scale(const FVector& s)
    {
        FMatrix result = {{ {s.x, 0, 0, 0},
                            {0, s.y, 0, 0},
                            {0, 0, s.z, 0},
                            {0, 0, 0, 1} }};
        return result;
    };
    
    // 왼손 좌표계 기준 회전 행렬 (x축)
    FMatrix FMatrix::RotationX(float radians)
    {
        float s = sinf(radians);
        float c = cosf(radians);

        FMatrix result = {{ {1, 0, 0, 0},
                            {0, c, s,0},
                            {0, -s, c, 0},
                            {0, 0, 0, 1} }};
        return result;

    }

    // 왼손 좌표계 기준 회전 행렬 (y축)
    FMatrix FMatrix::RotationY(float radians)
    {
        float s = sinf(radians);
        float c = cosf(radians);
        
        FMatrix result = {{ {c, 0, -s, 0},
        {0, 1, 0, 0},
        {s, 0, c, 0},
        {0, 0, 0, 1} }};
        return result;
        
    }         
    
    // 왼손 좌표계 기준 회전 행렬 (z축)
    FMatrix FMatrix::RotationZ(float radians)
    {
        float s = sinf(radians);
        float c = cosf(radians);

        FMatrix result = {{ {c, s, 0, 0},
                            {-s, c, 0, 0},
                            {0, 0, 1, 0},
                            {0, 0, 0, 1} }};
        return result;

    }         

    // 카메라
    FMatrix FMatrix::LookAt(const FVector& eye, const FVector& target, const FVector up)
    {
        FVector zAxis = (target - eye).Normalized(); // 카메라의 Forward 방향
        FVector xAxis = (FVector::Cross3D(up, zAxis)).Normalized(); // 카메라의 Right 방향
        FVector yAxis = FVector::Cross3D(zAxis, xAxis); // 카메라의 Up 방향

        
        FMatrix result = {{ {xAxis.x, yAxis.x, zAxis.x, 0.0f},
                            {xAxis.y, yAxis.y, zAxis.y, 0.0f},
                            {xAxis.z, yAxis.z, zAxis.z, 0.0f},
                            {-(xAxis.DotProduct(eye)), -(yAxis.DotProduct(eye)), -(zAxis.DotProduct(eye)), 1.0f} }};

        return result;

    }

    // 투영 (DirectX 표준 Depth 0 ~ 1)
    FMatrix FMatrix::PerspectiveFov(float fovY, float aspectRatio, float nearZ, float farZ)
    {
        float yScale = 1.0f / tanf(fovY/2.0f);
        float xScale = yScale / aspectRatio ;

        FMatrix result = {};
        result.M[0][0] = xScale;
        result.M[1][1] = yScale;
        result.M[2][2] = farZ / (farZ - nearZ);
        result.M[2][3] = 1.0;                   // w = z 로 만들기 위함.
        result.M[3][2] = -nearZ * farZ / (farZ - nearZ);

        return result;
    }

    // 행렬곱
    FMatrix FMatrix::operator*(const FMatrix& other) const
    {
        FMatrix result = {};
        for (uint32 r = 0; r < 4; ++r)
            for (uint32 c = 0; c < 4; ++c)
                for (uint32 k = 0; k < 4; ++k)
                    result.M[r][c] += M[r][k] * other.M[k][c];
        return result;
    }
    
    // 전치행렬
    FMatrix FMatrix::Transpose() const
    {
        FMatrix result = {};
        for (uint32 r = 0; r < 4; ++r)
            for (uint32 c = 0; c < 4; ++c)
                result.M[c][r] = M[r][c];
        return result;
    }