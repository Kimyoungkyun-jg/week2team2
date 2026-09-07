#include "pch.h"
#include "Intersection.h"

bool RayIntersectTriangle(const FVector& rayOrigin, const FVector& rayDirection, const FVector& V0, const FVector& V1, const FVector& V2, float & dist)
{
	// 삼각형 벡터
	FVector E1 = V1 - V0;
	FVector E2 = V2 - V0;
	
	// D : Ray 방향
	FVector D = rayDirection;
	FVector T = rayOrigin - V0;

	FVector P = FVector::Cross3D(D, E2);
	float det = E1.DotProduct(P);
	FVector Q = FVector::Cross3D(T, E1);

	if (fabs(det) < 0.000001f) {
		// ray와 삼각형이 평행하다! 
		return false;
	}


	float u = T.DotProduct(P) / det;
	if (u < 0 || u > 1) return false;

	float v = D.DotProduct(Q) / det;
	if (v < 0 || u + v > 1) return false;

	float t = E2.DotProduct(Q) / det;
	if (t < 0.0f) return false;

	dist = t;
	return true;
}
