#pragma once
#include <cmath>

//  - Up       : Y축 ( yaw )
//  - Right    : X축 ( pitch )
//  - Forward  : Z축 ( roll )

struct FVector;
struct FMatrix;
struct FQuaternion
{
    float x, y, z, w;

    // W가 1이면 항등 회전 (Identity) = 회전 0도
    // W가 0이면 에러 발생하므로 주의
    FQuaternion();
    FQuaternion(float X, float Y, float Z, float W);

    // 기본 연산
    float Length() const;
    float LengthSquared() const;
    void Normalize();
    FQuaternion Normalized() const;

    // x,y,z 축의 반대로 회전하는 쿼터니언 (원복)
    FQuaternion Conjugate() const;
    FQuaternion Inverse() const;
    
    // 회전 합성
    FQuaternion operator*(const FQuaternion& rhs) const;
    FQuaternion& operator*=(const FQuaternion& rhs);
    
    // 벡터 회전
    FVector RotateVector(const FVector& v) const;

    // 행렬 변환 ( row-major )
    FMatrix ToMatrix() const;

    static const FQuaternion Identity;

    static FQuaternion FromAxisAngle(const FVector& axis, float angleRad);
    
    // pitch, yaw, roll 단위 : radian
    static FQuaternion FromEuler(float pitch, float yaw, float roll);

    // 쿼터니언 -> 오일러 (디버그, UI 출력용)
    static FVector ToEuler(const FQuaternion& q);

    // 보간
    static FQuaternion Slerp(const FQuaternion& a, FQuaternion b, float t);

    // 정규화 근사 보간
    static FQuaternion Lerp(const FQuaternion& a, const FQuaternion& b, float t);

};