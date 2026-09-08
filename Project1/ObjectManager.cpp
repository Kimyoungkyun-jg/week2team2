#include "pch.h"
#include "ObjectManager.h"
#include "AGizmo.h"

// 객체 유효성 검사
bool ObjectManager::IsValidObject(const UObject* Target, uint32 UUID) const
{
	if (!Target || UUID == 0) return false;

	// 등록 객체 검사
	for (const UObject* Obj : AllObjects)
	{
		if (Obj == Target && Obj->GetID() == UUID)
			return true;
	}

	// 기즈모 축 검사
	if (AGizmo::MainGizmo)
	{
		for (const AGizmoAxis* Axis : AGizmo::MainGizmo->GetAxes())
		{
			if (Axis == Target && Axis->GetID() == UUID)
				return true;
		}
	}

	return false;
}
