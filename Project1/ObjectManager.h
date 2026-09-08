#pragma once

#include "UObject.h"
#include "CollisionManager.h"
#include "string_view"
#include "Mesh.h"

//모든 UObject를 관리하는 클래스, Main 초기에 Get 호출
class ObjectManager
{
public:

	~ObjectManager()
	{
		DestroyAllObjects();
		for (auto& pair : AllClassInfoMap)
		{
			delete pair.second;
		}
		AllClassInfoMap.clear();

		for (auto& pair : AllMeshMap)
		{
			delete pair.second;
		}
		AllMeshMap.clear();
	}

	TArray<UObject*> AllObjects;
	TMap<string_view, ClassInfo*> AllClassInfoMap;

	ClassInfo* GetOrCreateClassInfo(string_view name, const ClassInfo* superClass = nullptr)
	{
		auto it = AllClassInfoMap.find(name);
		if (it != AllClassInfoMap.end())
			return it->second;

		uint32 newID = UEngineStatics::GetUCID();
		ClassInfo* newInfo = new ClassInfo(name, newID, superClass);
		AllClassInfoMap[name] = newInfo;
		return newInfo;
	}
	void Destroy(UObject* Target)
	{
		for (int32 i = static_cast<int32>(AllObjects.size()) - 1; i >= 0; --i)
		{
			if (AllObjects[i] == Target)
			{
				if (ACollider* Collider = dynamic_cast<ACollider*>(Target))
				{
					CollisionManager::GetInstance().DeleteColider(Collider->GetID());
				}

				UObject* temp = AllObjects[i];
				swap(AllObjects[i], AllObjects.back());
				AllObjects.pop_back();
				
				delete(temp);


				break;
			}
		}
	}

	void DestroyAllObjects()
	{
		for (int32 i = static_cast<int32>(AllObjects.size()) - 1; i >= 0; --i)
		{
			delete(AllObjects[i]);
		}

		AllObjects.clear();
	}

	void DestroyAllActors()
	{
		for (int32 i = static_cast<int32>(AllObjects.size()) - 1; i >= 0; --i)
		{
			if (AActor* Actor = dynamic_cast<AActor*>(AllObjects[i]))
			{
				CollisionManager::GetInstance().DeleteColider(Actor->GetID());
			}

			UObject* temp = AllObjects[i];
			swap(AllObjects[i], AllObjects.back());
			AllObjects.pop_back();
			delete(temp);
		}
	}

	UObject* Find(uint32 ID)
	{
		for (UObject* Obj : AllObjects)
		{
			if (Obj->GetID() == ID)
				return Obj;
		}

		return nullptr;
	}

	static ObjectManager& GetInstance()
	{
		static ObjectManager Manager;
		return Manager;
	}
	
	
	

	TMap<FString, Mesh*> AllMeshMap; 
	//같은 메쉬는 저장해서 사용

	Mesh* GetMesh(const FString& name)
	{
		auto it = AllMeshMap.find(name);
		if (it != AllMeshMap.end())
			return it->second;
		return nullptr;
	}

	template <typename VertexType, size_t N>
	Mesh* GetOrCreateMesh(const FString& name, const VertexType(&vertices)[N])
	{
		auto it = AllMeshMap.find(name);
		if (it != AllMeshMap.end())
			return it->second;

		Mesh* newMesh = new Mesh();
		newMesh->InitVertexBuffer(vertices);
		AllMeshMap[name] = newMesh;
		return newMesh;
	}

	template <typename VertexType>
	Mesh* GetOrCreateMesh(const FString& name, const TArray<VertexType>& vertices)
	{
		auto it = AllMeshMap.find(name);
		if (it != AllMeshMap.end())
			return it->second;

		Mesh* newMesh = new Mesh();
		newMesh->InitVertexBuffer(vertices);
		AllMeshMap[name] = newMesh;
		return newMesh;
	}

	Mesh* RegisterMesh(const FString& name, Mesh* inMesh)
	{
		if (AllMeshMap.find(name) != AllMeshMap.end())
		{
			delete AllMeshMap[name];
		}
		AllMeshMap[name] = inMesh;
		return inMesh;
	}
	
	ObjectManager(const ObjectManager&) = delete;
	ObjectManager& operator=(const ObjectManager&) = delete;




	//메모리 관리
	uint32 TotalAllocationBytes = 0;
	uint32 TotalAllocationCount = 0;

private:
	ObjectManager(){}



};
