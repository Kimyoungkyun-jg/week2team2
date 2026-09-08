#include "pch.h"
#include "FQuaternion.h"
#include "Matrix.h"
#include "Global.h"

//  - Up       : Y축 ( yaw )
//  - Right    : X축 ( pitch )
//  - Forward  : Z축 ( roll )

const FQuaternion FQuaternion::Identity = FQuaternion(0.0f, 0.0f, 0.0f, 1.0f);

// W가 1이면 항등 회전 (Identity) = 회전 0도
// W가 0이면 에러 발생하므로 주의
FQuaternion::FQuaternion(): x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

FQuaternion::FQuaternion(float X, float Y, float Z, float W)
    : x(X), y(Y), z(Z), w(W) {}

// 기본 연산
float FQuaternion::LengthSquared() const { return x*x + y*y + z*z + w*w; }
float FQuaternion::Length() const { return sqrtf(LengthSquared()); }
void FQuaternion::Normalize()
{
    float len = Length();

    // len이 0 이상일 경우만
    if (len > 1e-6f)
    {
        // 나눗셈 연산 비용 비싸므로 역수 곱함
        float invLen = 1.0f / len;
        x *= invLen;
        y *= invLen;
        z *= invLen;
        w *= invLen;
    }
    else
    {
        // Length = 0이면 항등 회전으로 대체 ( 오류 방지 )
        x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;
    }
}

FQuaternion FQuaternion::Normalized() const
{
    FQuaternion result = *this;
    result.Normalize();
    return result;
}

// x,y,z 축의 반대로 회전하는 쿼터니언 (원복)
FQuaternion FQuaternion::Conjugate() const { return FQuaternion(-x, -y, -z, w); }
FQuaternion FQuaternion::Inverse() const
{   
    // |q|^2 계산 
    float lenSq = LengthSquared();
    if (lenSq <= 1e-6f) return Identity;
    float invLenSq = 1 / lenSq;

    // q 켤레
    FQuaternion q = Conjugate();
    
    q.x *= invLenSq;
    q.y *= invLenSq;
    q.z *= invLenSq;
    q.w *= invLenSq;

    return q;
}

// 회전 합성 (this * rhs) EX. world = parent * local
FQuaternion FQuaternion::operator*(const FQuaternion& rhs) const
{
    return FQuaternion{
        w*rhs.x + x*rhs.w + y*rhs.z - z*rhs.y, // i 성분
        w*rhs.y - x*rhs.z + y*rhs.w + z*rhs.x, // j 성분
        w*rhs.z + x*rhs.y - y*rhs.x + z*rhs.w, // k 성분
        w*rhs.w - x*rhs.x - y*rhs.y - z*rhs.z  // w 성분
    };
}
FQuaternion& FQuaternion::operator*=(const FQuaternion& rhs)
{
    *this = (*this) * rhs;
    return *this;
}

// 벡터 회전
FVector FQuaternion::RotateVector(const FVector& v) const
{
    // v' = q * v * q^-1 (q가 정규화되어 있다면 Conjugate = Inverse)
    FQuaternion qv(v.x, v.y, v.z, 0.0f);
    FQuaternion result = (*this) * qv * Conjugate();
    return FVector(result.x, result.y, result.z);
}

// 행렬 변환 ( row-major )
FMatrix FQuaternion::ToMatrix() const
{
    float xx = x*x, xy = x*y, xz = x*z, xw = x*w;
    float yx = y*x, yy = y*y, yz = y*z, yw = y*w;
    float zx = z*x, zy = z*y, zz = z*z, zw = z*w;
    float wx = w*x, wy = w*y, wz = w*z, ww = w*w;

    FMatrix m;

    m.M[0][0] = 1.0f - 2.0f * (yy + zz);
    m.M[0][1] = 2.0f * (xy + wz);
    m.M[0][2] = 2.0f * (xz - wy);
    m.M[0][3] = 0.0f;

    m.M[1][0] = 2.0f * (xy - wz);
    m.M[1][1] = 1.0f - 2.0f * (xx + zz);
    m.M[1][2] = 2.0f * (yz + wx);
    m.M[1][3] = 0.0f;

    m.M[2][0] = 2.0f * (xz + wy);
    m.M[2][1] = 2.0f * (yz - wx);
    m.M[2][2] = 1.0f - 2.0f * (xx + yy);
    m.M[2][3] = 0.0f;

    m.M[3][0] = 0.0f;
    m.M[3][1] = 0.0f;
    m.M[3][2] = 0.0f;
    m.M[3][3] = 1.0f;

    return m;
}

FQuaternion FQuaternion::FromAxisAngle(const FVector& axis, float angleRad)
{
    FVector n = axis.Normalized(); // 방향만 가져오기
    float half = angleRad * 0.5f;  // 두번의 쿼터니언 곱을 해주므로 미리 반각
    float s = sinf(half);
    return FQuaternion(n.x * s, n.y *s, n.z *s, cosf(half));
}

// pitch, yaw, roll 단위 : radian
FQuaternion FQuaternion::FromEuler(float pitch, float yaw, float roll)
{
    float cp = cosf(pitch*0.5), sp = sinf(pitch*0.5);
    float cy = cosf(yaw*0.5), sy = sinf(yaw*0.5);
    float cr = cosf(roll*0.5), sr = sinf(roll*0.5);
    
    FQuaternion qYaw(0.0f, sy, 0.0f, cy);
    FQuaternion qPitch(sp, 0.0f, 0.0f, cp);
    FQuaternion qRoll(0.0f, 0.0f, sr, cr);
    
    // yaw(좌우) -> pitch(상하) -> roll(갸우뚱)
    return (qYaw * qPitch * qRoll).Normalized();
}

// 쿼터니언 -> 오일러 (디버그, UI 출력용)
FVector FQuaternion::ToEuler(const FQuaternion& q)
{
    float pitch, yaw, roll;

    // Pitch
    float sinPitch = 2.0f * ( q.w * q.x - q.y * q.z);
    // 짐벌락 경계 90도
    if (fabsf(sinPitch) >= 1.0f)
    {
        pitch = copysignf(Global::PI / 2.0f, sinPitch);
        roll = 0.0f;
        yaw = atan2f(-2.0f * (q.x * q.z - q.w * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z));
    }
    else
    {
        pitch = asinf(sinPitch);
    } 

    // Yaw
    float sinYawCosPitch = 2.0f * (q.w * q.y + q.x * q.z);
    float cosYawCosPitch = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    yaw = atan2f(sinYawCosPitch, cosYawCosPitch);

    // Roll
    float sinRollCosPitch = 2.0f * (q.w * q.z + q.x * q.y);
    float cosRollCosPitch = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
    roll = atan2f(sinRollCosPitch, cosRollCosPitch);

    return FVector(pitch, yaw, roll);
}

// 보간
FQuaternion FQuaternion::Slerp(const FQuaternion& a, FQuaternion b, float t)
{
    float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;

    // 두 쿼터니언이 반대 반구에 있으면 부호를 뒤집어 최단 경로로 보간
    if (dot < 0.0f) 
    {
        b.x = -b.x; b.y = -b.y; b.z = -b.z; b.w = -b.w;
        dot = -dot;
    }

    const float THRESHOLD = 0.9995f;
    // 두 점이 매우 가까우면 곡선과 직선의 경로가 차이가 크지 않으므로
    // 성능 최적화를 위해 LERP 사용
    if (dot > THRESHOLD)
    {
        FQuaternion result(
                a.x + t * (b.x - a.x),
                a.y + t * (b.y - a.y),
                a.z + t * (b.z - a.z),
                a.w + t * (b.w - a.w)
        );
        result.Normalize();
        return result;
    }

    float theta0 = acosf(dot);
    float theta = theta0 * t;
    float sinTheta0 = sinf(theta0);
    float sinTheta = sinf(theta);

    float s0 = cosf(theta) - dot * sinTheta / sinTheta0;
    float s1 = sinTheta / sinTheta0;
 
    return FQuaternion(
        s0 * a.x + s1 * b.x,
        s0 * a.y + s1 * b.y,
        s0 * a.z + s1 * b.z,
        s0 * a.w + s1 * b.w
    );
}

// 정규화 근사 보간
FQuaternion FQuaternion::Lerp(const FQuaternion& a, const FQuaternion& b, float t)
{
            FQuaternion result(
                a.x + t * (b.x - a.x),
                a.y + t * (b.y - a.y),
                a.z + t * (b.z - a.z),
                a.w + t * (b.w - a.w)
        );
        result.Normalize();
        return result;
}
