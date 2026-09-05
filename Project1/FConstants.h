#pragma once

#include "Matrix.h"

struct FFrameConstants {
	DirectX::XMMATRIX VP;
};

struct FConstants
{
	DirectX::XMMATRIX World;
	FMatrix WVP;
};
