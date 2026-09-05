#pragma once

#include "Renderer.h"
#include "enums.h"
#include "FVector.h"

class UObject
{
public:
	UObject()
	{
		++UIDMax;
		UID = UIDMax;
	}
	virtual ~UObject() {}
	uint32 GetID() const { return UID; }


	virtual void Update(float deltatime);
	virtual void Render() {}

	virtual void Destroy();
	virtual void Tick(float deltaTime) {}
	
	void Active() { bIsActive = true; }
	void DeActive() { bIsActive = false; }
	bool const GetIsActive() { return bIsActive; }
private:
	inline static uint32 UIDMax = 0;
	uint32 UID = 0;
	bool bIsActive = true;
};

