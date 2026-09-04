#pragma once

#include "FVector.h"

struct FConstants
{
	FVector Offset;
	float Rotation;	// radians
	FVector Scale;	// (width, height, depth)
	float AspectRatio;
};
