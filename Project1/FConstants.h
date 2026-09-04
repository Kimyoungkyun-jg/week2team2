#pragma once

#include "FVector.h"
#include "FMatrix.h"

struct FConstants
{
	FVector Offset;
	float Rotation;	// radians
	FVector Scale;	// (width, height, depth)

	DirectX::XMFLOAT4X4 WVP;

	FMatrix Model;
	FMatrix View;
	FMatrix Projection;

	float FovY;
	float AspectRatio;
	float NearZ;
	float FarZ;

};
