#pragma once

#include "UObject.h"
#include "CollisionManager.h"
#include "string_view"
#include "Mesh.h"
#include <algorithm>

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

		DestroyAllMeshes();
	}

	void DestroyAllMeshes()
	{
		for (auto& pair : AllMeshMap)
		{
			delete pair.second;
		}
		AllMeshMap.clear();
	}

	TArray<UObject*> AllObjects;

	void AddObject(UObject* Obj)
	{
		auto it = std::upper_bound(AllObjects.begin(), AllObjects.end(), Obj,
			[](const UObject* a, const UObject* b) {
				return a->GetRenderPriority() < b->GetRenderPriority();
			});
		AllObjects.insert(it, Obj);
	}
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
	TMap<uint32, ACollider*> ColliderMap;

	void AddCollider(ACollider* col)
	{
		if (col)
		{
			ColliderMap[col->GetID()] = col;
		}
	}

	// 콜라이더 맵에서만 제거
	void DestroyCollider(uint32 id)
	{
		ColliderMap.Remove(id);
	}

	void DestroyCollider(ACollider* col)
	{
		if (col)
		{
			ColliderMap.Remove(col->GetID());
		}
	}

	// 콜라이더 맵만 클리어
	void DestroyAllColliders()
	{
		ColliderMap.clear();
	}

	void Destroy(UObject* Target);
	void DestroyAllObjects();
	void DestroyAllActors();
	void DestroyAllActor() { DestroyAllActors(); }

	UObject* Find(uint32 ID)
	{
		for (UObject* Obj : AllObjects)
		{
			if (Obj->GetID() == ID)
				return Obj;
		}

		return nullptr;
	}

	// 객체 유효성 검사
	bool IsValidObject(const UObject* Target, uint32 UUID) const;

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

		Mesh* newMesh = new Mesh(vertices);
		AllMeshMap[name] = newMesh;
		return newMesh;
	}

	template <typename VertexType>
	Mesh* GetOrCreateMesh(const FString& name, const TArray<VertexType>& vertices)
	{
		auto it = AllMeshMap.find(name);
		if (it != AllMeshMap.end())
			return it->second;

		Mesh* newMesh = new Mesh(vertices);
		AllMeshMap[name] = newMesh;
		return newMesh;
	}

	template <typename VertexType>
	Mesh* GetOrCreateMesh(const FString& name, const std::vector<VertexType>& vertices)
	{
		auto it = AllMeshMap.find(name);
		if (it != AllMeshMap.end())
			return it->second;

		Mesh* newMesh = new Mesh(vertices);
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
