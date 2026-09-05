#pragma once

#include "UObject.h"
#include "CollisionManager.h"

//모든 UObject를 관리하는 클래스, Main 초기에 Get 호출
class ObjectManager
{
public:

	~ObjectManager()
	{
		DestroyAllObjects();
	}

	vector<UObject*> AllObjects;
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

	void DistroyAllActors()
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
	ObjectManager(const ObjectManager&) = delete;
	ObjectManager& operator=(const ObjectManager&) = delete;

private:
	ObjectManager(){}
};
