#pragma once

#include "ACollider.h"

class USphere : public ACollider
{
	DECLARE_CLASS(USphere, ACollider)

public:
	USphere()
	{
		Primitive = EPrimitive::Sphere;
	}
};

