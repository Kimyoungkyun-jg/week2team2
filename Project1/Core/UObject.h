#pragma once

#include "FVector.h"
#include "GameManager.h"
#include "UEngineStatics.h"
#include "enums.h"
#include <cstdint>
#include <string_view>
#include <typeinfo>

#define DECLARE_CLASS(ClassName, SuperName)                                    \
public:                                                                        \
  using ThisClass = ClassName;                                                 \
  using Super = SuperName;                                                     \
  static constexpr string_view StaticClassName() { return #ClassName; }        \
  static constexpr string_view StaticSuperClassName() { return #SuperName; }   \
  virtual string_view GetObjClassName() const override { return #ClassName; }  \
  virtual string_view GetSuperClassName() const override { return #SuperName; }

struct ClassInfo // 클래스 정보를 담는 메타 데이터 클래스
{
  ClassInfo(string_view name, uint32 cID, const ClassInfo *superClass = nullptr)
      : Name(name), CID(cID), SuperClass(superClass) {}

  string_view Name;
  uint32 CID = 0;
  const ClassInfo *SuperClass = nullptr;

  bool IsA(const ClassInfo *Targetclass) const {
    for (const ClassInfo *Cur = this; Cur != nullptr; Cur = Cur->SuperClass) {
      if (Cur == Targetclass) {
        return true;
      }
    }
    return false;
  }
};

class UObject {
public:
  // 고유 식별자 기본 발급
  UObject() : UUID(UEngineStatics::GetUUID()) {}
  virtual ~UObject() {}

  uint32 GetID() const { return UUID; }
  void SetUUID(uint32 inUUID) { UUID = inUUID; }

  virtual string_view GetObjClassName() const {
    return "UObject";
  } // 자식들이 매크로를 통해 오버라이딩해야됨
  virtual string_view GetSuperClassName() const { return "None"; }

  void SetClassInfo(const ClassInfo *inClassInfo) { classinfo = inClassInfo; }

  const ClassInfo *GetClass() const { return classinfo; }

  // 객체를 인자로 받는 IsA (예: objA->IsA(objB))
  bool IsA(const UObject *TargetObject) const {
    if (!TargetObject || !TargetObject->GetClass() || !classinfo)
      return false;
    return classinfo->IsA(TargetObject->GetClass());
  }

  // ClassInfo를 직접 받는 IsA@
  bool IsA(const ClassInfo *TargetClass) const {
    return classinfo ? classinfo->IsA(TargetClass) : false;
  }

  // 메모리 추적용 new, delete 오버로딩
  void *operator new(size_t size);
  void operator delete(void *ptr, size_t size) noexcept;

  virtual void Update(float deltatime);
  virtual void Render() {}
  virtual int GetRenderPriority() const { return 0; }

  virtual void Destroy();
  virtual void Tick(float deltaTime) {}

  void Active() { bIsActive = true; }
  void DeActive() { bIsActive = false; }
  bool const GetIsActive() { return bIsActive; }

private:
  uint32 UUID = 0;
  bool bIsActive = true;
  const ClassInfo *classinfo; // 포인터 1개만
};
