#include "ObjectManager.h"
#include "AGizmo.h"
#include "TemplateLibrary.h"
#include "pch.h"


// 객체 유효성 검사
bool ObjectManager::IsValidObject(const UObject *Target, uint32 UUID) const {
  if (!Target || UUID == 0)
    return false;

  // 등록 객체 검사
  for (const UObject *Obj : AllObjects) {
    if (Obj == Target && Obj->GetID() == UUID)
      return true;
  }

  // 기즈모 축 검사
  if (AGizmo::MainGizmo) {
    for (const AGizmoAxis *Axis : AGizmo::MainGizmo->GetAxes()) {
      if (Axis == Target && Axis->GetID() == UUID)
        return true;
    }
  }

  return false;
}

void ObjectManager::Destroy(UObject *Target) {
  for (int32 i = static_cast<int32>(AllObjects.size()) - 1; i >= 0; --i) {
    if (AllObjects[i] == Target) {
      if (ACollider *Collider = Cast<ACollider>(Target)) {
        DestroyCollider(Collider->GetID());
      }

      UObject *temp = AllObjects[i];
      swap(AllObjects[i], AllObjects.back());
      AllObjects.pop_back();
      delete temp;
      break;
    }
  }
}

void ObjectManager::DestroyAllObjects() {
  for (int32 i = static_cast<int32>(AllObjects.size()) - 1; i >= 0; --i) {
    delete AllObjects[i];
  }
  AllObjects.clear();
  ColliderMap.clear();
}

void ObjectManager::DestroyAllActors() {
  for (int32 i = static_cast<int32>(AllObjects.size()) - 1; i >= 0; --i) {
    // 커스텀 캐스트 사용
    if (AActor *Actor = Cast<AActor>(AllObjects[i])) {
      Actor->Destroy();
    }
  }
}
