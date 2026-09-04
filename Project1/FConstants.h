#pragma once

#include "FVector.h"

struct FConstants
{
	DirectX::XMMATRIX worldmat;
	DirectX::XMMATRIX viewmat;
	DirectX::XMMATRIX projmat;

	float AspectRatio;
	float Pad[3];
};
