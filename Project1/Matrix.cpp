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

        
        FMatrix result = {{ {xAxis.x, yAxis.x, zAxis.x, 0},
                            {xAxis.y, yAxis.y, zAxis.y, 0},
                            {xAxis.z, yAxis.z, zAxis.z, 0},
                            {-(xAxis.DotProduct(eye)), -(yAxis.DotProduct(eye)), -(zAxis.DotProduct(eye)), 1} }};

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

    //////////////////////////////////////
    //////////// SRT 성분 추출 ////////////
    /////////////////////////////////////
    
    // Scalse 성분 추출
    FVector FMatrix::ExtractScale() const
    {
        // 각 행 벡터의 길이 = Scale 성분 (why? 회전행렬은 길이를 보존하므로)
        float sx = sqrtf(M[0][0]*M[0][0] + M[0][1]*M[0][1] + M[0][2]*M[0][2]);
        float sy = sqrtf(M[1][0]*M[1][0] + M[1][1]*M[1][1] + M[1][2]*M[1][2]);
        float sz = sqrtf(M[2][0]*M[2][0] + M[2][1]*M[2][1] + M[2][2]*M[2][2]);
        
        return FVector(sx, sy, sz);
    }
    
    // Rotation 성분 추출
    FMatrix FMatrix::ExtractRotation() const
    {
        FVector scale = ExtractScale();
        FMatrix result = Identity();
        
        for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
        {
            if (i==0) result.M[i][j] = M[i][j] / scale.x; // 첫번째 행
            if (i==1) result.M[i][j] = M[i][j] / scale.y; // 두번째 행
            if (i==2) result.M[i][j] = M[i][j] / scale.z; // 세번째 행
        }
        return result;
    }

    // Translation 성분 추출
    FVector FMatrix::ExtractTranslation() const
    {
        return FVector(M[3][0], M[3][1], M[3][2]);
    }

    //////////////////////////////////////
    //////////// 역행렬 계산 //////////////
    /////////////////////////////////////

    // 역행렬 (Normal) - 어떤 행렬이든 계산 가능 (But 연산량 주의) 
    FMatrix FMatrix::Inverse() const
    {
        const float* m = &M[0][0];  // 포인터로 접근 row-major 이므로 1행 -> 2행 -> ...
        float inv[16];              // 4x4

        inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15]
            + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
    
        inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15]
            - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
    
        inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15]
            + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
    
        inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14]
            - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
        
        // 행렬식 계산
        float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
        
        // Singular Matrix (역행렬 없음)
        if (fabsf(det) < 1e-6f)
        {
            assert(false, "Warn: Inverse Mtx. does not exists!");
            return Identity();  // 단위 행렬 반환
        }
        
        // Second Column
        inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15]
        - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
        
        inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15]
        + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
        
        inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15]
        - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
        
        inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14]
        + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
        
        // Third Column
        inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15]
        + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
        
        inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15]
        - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
        
        inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15]
        + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
        
        inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14]
        - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
        
        // Forth Column
        inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11]
        - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
        
        inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11]
        + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
        
        inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11]
        - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
        
        inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10]
        + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];
        
        FMatrix result = {};
        
        // 역행렬 = 1/행렬식 * 여인수 행렬
        float invDet = 1.0f / det;
        float* out = &result.M[0][0];
        for (int i = 0; i < 16; ++i)
            out[i] = inv[i] * invDet;
        
        return result;
    }

    // 역행렬 (SRT 행렬 전용)
    FMatrix FMatrix::InverseAffine() const
    {
        // SRT 성분 추출
        FVector scale = ExtractScale();
        FMatrix rotation = ExtractRotation();
        FVector translation = ExtractTranslation();

        // R^-1
        FMatrix InversedR = rotation.Transpose();

        // S^-1
        for (int i=0; i<3; i++)
            for (int j=0; j<3; j++)
            {
                if (j==0) InversedR.M[i][j] /= scale.x; // 0번째 열
                if (j==1) InversedR.M[i][j] /= scale.y; // 1번째 열
                if (j==2) InversedR.M[i][j] /= scale.z; // 2번째 열
            }
        // InversedR = 회전 + 스케일의 역행렬
        FMatrix result = InversedR;

        // T^-1
        result.M[3][0] = -(translation.x*InversedR.M[0][0] + translation.y*InversedR.M[1][0] + translation.z*InversedR.M[2][0]);
        result.M[3][1] = -(translation.x*InversedR.M[0][1] + translation.y*InversedR.M[1][1] + translation.z*InversedR.M[2][1]);
        result.M[3][2] = -(translation.x*InversedR.M[0][2] + translation.y*InversedR.M[1][2] + translation.z*InversedR.M[2][2]);

        return result;
    }