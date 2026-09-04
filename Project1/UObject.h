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
	int GetID() const { return UID; }


	virtual void Update(float deltatime);
	virtual void Render() {}

	virtual void Destroy();
	virtual void Tick(float deltaTime) {}
	
	void Active() { bIsActive = true; }
	void DeActive() { bIsActive = false; }
	bool const GetIsActive() { return bIsActive; }
private:
	inline static int UIDMax = 0;
	int UID = 0;
	bool bIsActive = true;
};

