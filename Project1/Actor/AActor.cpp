#include "AActor.h"
#include "AGizmo.h"
#include "Intersection.h"
#include "PickingManager.h"
#include "Renderer.h"
#include "pch.h"


AActor::AActor(const FLinearColor &inColor) : Color(inColor) {
  worldBuffer = new MatrixBuffer();
}

AActor::~AActor() {
  if (worldBuffer) {
    delete worldBuffer;
    worldBuffer = nullptr;
  }

  if (bOwnsMesh && mesh) {
    delete mesh;
    mesh = nullptr;
  }
}

void AActor::Destroy() {
  // 기즈모 선택 대상 해제
  if (AGizmo::MainGizmo && AGizmo::MainGizmo->GetTargetActor() == this) {
    AGizmo::MainGizmo->SetTargetActor(nullptr);
  }

  Super::Destroy();
}

bool AActor::bIsPicked(const FRay &worldRay, float &outDistance) {
  if (mesh) {
    return mesh->bIsPicked(worldRay, transform, outDistance);
  }
  return false;
}

void AActor::SetWorldBuffer() {
  worldBuffer->SetMat(transform.WorldMat);
  worldBuffer->SetVSBuffer(0);
}

bool AActor::IsSelected() const {
  return AGizmo::MainGizmo && this == AGizmo::MainGizmo->GetTargetActor();
}

void AActor::Render() {
  UObject::Render();

  SetWorldBuffer();

  if (mesh) {
    const bool bSelected = IsSelected();
    if (bSelected) {
      RENDERER.SetSelectedState();
    }

    mesh->SetColor(Color);
    mesh->Render();

    if (bSelected) {
      RENDERER.SetDefaultDepthState();
    }
  }
}

void AActor::Update(float Deltatime) { UObject::Update(Deltatime); }
