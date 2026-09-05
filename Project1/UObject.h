#pragma once

#include "Renderer.h"
#include "enums.h"
#include "FVector.h"
#include "App.h"
#include "UEngineStatics.h"

class UObject
{
public:
	UObject() : UUID(UEngineStatics::GetUUID())
	{
	}
	virtual ~UObject() {}
	uint32 GetID() const { return UUID; }

	void* operator new(size_t size)
	{
		if (App::Instance)
		{
			App::Instance->TotalAllocationBytes += size;  // 할당 요청된 바이트 누적
			App::Instance->TotalAllocationCount++;        // 할당 횟수 1 증가

			// 실제 OS 힙 할당 수행
			void* ptr = malloc(size);
			if (!ptr) throw std::bad_alloc();
			return ptr;
		}
		else
			throw std::runtime_error("App::Instance is not initialized!");
	}

	void operator delete(void* ptr, size_t size) noexcept
	{
		if (App::Instance)
		{
			App::Instance->TotalAllocationBytes -= size;  // 할당 요청된 바이트 누적
			App::Instance->TotalAllocationCount--;        // 할당 횟수 1 증가

			free(ptr);
		}
		else
			throw std::runtime_error("App::Instance is not initialized!");


	}


	virtual void Update(float deltatime);
	virtual void Render() {}

	virtual void Destroy();
	virtual void Tick(float deltaTime) {}
	
	void Active() { bIsActive = true; }
	void DeActive() { bIsActive = false; }
	bool const GetIsActive() { return bIsActive; }
private:
	uint32 UUID = 0;
	bool bIsActive = true;
};

