#include "pch.h"
#include "UObject.h"
#include "ObjectManager.h"

void* UObject::operator new(size_t size)
{
	OBJECT.TotalAllocationBytes += static_cast<uint32>(size);
	OBJECT.TotalAllocationCount++;

	void* ptr = malloc(size);
	if (!ptr) throw std::bad_alloc();
	return ptr;
}

void UObject::operator delete(void* ptr, size_t size) noexcept
{
	OBJECT.TotalAllocationBytes -= static_cast<uint32>(size);
	OBJECT.TotalAllocationCount--;

	free(ptr);
}

void UObject::Destroy()
{
	ObjectManager::GetInstance().Destroy(this);
}

void UObject::Update(float deltatime)
{
	if (!bIsActive) return;
}
