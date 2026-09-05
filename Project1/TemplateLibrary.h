#pragma once

#include "FVector.h"
#include "UObject.h"
#include "AActor.h"
#include "ACollider.h"
#include "ObjectManager.h"
#include "CollisionManager.h"
#include "enums.h"

template<typename, typename = void>
struct has_super : std::false_type {}; //부모 없으면 컴파일 타임에 false 반환

template<typename T>
struct has_super<T, std::void_t<typename T::Super>> : std::true_type {};  //부모 있으면 컴파일 타임에 false 반환

class FObjectFactory
{
public:
	template<typename T>
	static inline string_view GetObjClassName()
	{
		string_view name = typeid(T).name();
		if (name.rfind("class ", 0) == 0)
			name.remove_prefix(6); //"class UMesh" 이런 식으로 나와서 
		else if (name.rfind("struct ", 0) == 0)
			name.remove_prefix(7);

		return name;
	}

	template<typename T>
	static inline ClassInfo* GetStaticClassInfo()
	{
		string_view name = GetObjClassName<T>();
		auto& map = ObjectManager::GetInstance().AllClassInfoMap;

		auto it = map.find(name); //맵에서 찾으면 바로 리턴
		if (it != map.end())
			return it->second;

		const ClassInfo* superInfo = nullptr;
		if constexpr (has_super<T>::value) 
		{
			superInfo = GetStaticClassInfo<typename T::Super>();
		}

		uint32 newID = UEngineStatics::GetUCID();
		ClassInfo* newInfo = new ClassInfo(name, newID, superInfo);
		map[name] = newInfo;
		return newInfo;
	}

	template<class T>
	static inline T* NewObject()
	{
		static_assert(is_base_of_v<UObject, T>);

		T* Obj = new T;
		ObjectManager::GetInstance().AllObjects.push_back(Obj);

		ClassInfo* info = GetStaticClassInfo<T>();
		Obj->SetClassInfo(info);

		return static_cast<T*>(Obj);
	}

	template<class T>
	static inline T* SpawnActor(FVector Location, FVector Scale = { 0.1, 0.1, 1 })
	{
		static_assert(is_base_of_v<AActor, T>);

		AActor* SpawnedActor = NewObject<T>();

		// 크기
		float minRadius = 0.05f;
		float maxRadius = 0.10f;
		SpawnedActor->SetScale(Scale);

		// 위치
		SpawnedActor->SetLocation(Location);

		// 모양
		return static_cast<T*>(SpawnedActor);
	}

	template<class T>
	static inline T* SpawnColider(FVector Location, FVector Scale = { 1, 1, 1 }, float Mass = 1)
	{
		static_assert(is_base_of_v<ACollider, T>);
		ACollider* Colider = SpawnActor<T>(Location, Scale);

		Colider->SetMass(Mass);
		CollisionManager::GetInstance().AddColider(Colider);

		return static_cast<T*>(Colider);
	}

	static inline bool TraceSphere(FVector Location, float Radius, TArray<ACollider*>& Result)
	{
		bool bFound = false;
		TArray<ACollider*> Colliders = CollisionManager::GetInstance().colliders;
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
};

template<typename To, typename From>
inline To* Cast(From* Src) // 언리얼 스타일 고속 다운캐스팅
{
	if (Src && Src->IsA(FObjectFactory::GetStaticClassInfo<To>()))
	{
		return static_cast<To*>(Src);
	}

	return nullptr;
}

