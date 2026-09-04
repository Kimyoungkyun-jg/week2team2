#pragma once

#include "FVector.h"
#include "UObject.h"
#include "ObjectManager.h"
#include "CollisionManager.h"
#include "enums.h"


template<class T>
inline T* NewObject()
{
	static_assert(std::is_base_of_v<UObject, T>);

	T* Obj = new T;
	ObjectManager::GetInstance().AllObjects.push_back(Obj);

	return static_cast<T*>(Obj);
}

template<class T>
inline T* SpawnActor(FVector Location, EPrimitive Primitive, FVector Scale = { 0.1, 0.1, 1 })
{
	static_assert(std::is_base_of_v<AActor, T>);

	AActor* SpawnedActor = NewObject<T>();

	// 크기
	float minRadius = 0.05f;
	float maxRadius = 0.10f;
	SpawnedActor->SetScale(Scale);

	// 위치
	SpawnedActor->SetLocation(Location);

	// 모양
	SpawnedActor->SetPrimitive(Primitive);
	return static_cast<T*>(SpawnedActor);
}

template<class T>
inline T* SpawnColider(FVector Location, EPrimitive Primitive, FVector Scale = { 1, 1, 1 }, float Mass = 1)
{
	static_assert(std::is_base_of_v<ACollider, T>);
	ACollider* Colider = SpawnActor<T>(Location, Primitive, Scale);

	Colider->SetMass(Mass);
	CollisionManager::GetInstance().AddColider(Colider);

	return static_cast<T*>(Colider);
}

inline bool TraceSphere(FVector Location, float Radius, std::vector<ACollider*>& Result)
{
	bool bFound = false;
	std::vector<ACollider*> Colliders = CollisionManager::GetInstance().colliders;
	for (ACollider* c : Colliders)
	{
		if ((c->GetLocation() - Location).LengthSquared() <= Radius * Radius)
		{
			Result.push_back(c);
			bFound = true;
		}
	}

	return bFound;
}