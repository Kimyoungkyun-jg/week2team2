#pragma once

#include "FVector.h"

struct FConstants
{
	FVector Offset;
	float Rotation;	// radians
	FVector Scale;	// (width, height, depth)
	float AspectRatio;

	DirectX::XMFLOAT4X4 WVP;
};
