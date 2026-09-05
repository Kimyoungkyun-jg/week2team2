#include "pch.h"
#include "UObject.h"
#include "ObjectManager.h"

void UObject::Destroy()
{
	ObjectManager::GetInstance().Destroy(this);
}

void UObject::Update(float deltatime)
{
	if (!bIsActive) return;
}
