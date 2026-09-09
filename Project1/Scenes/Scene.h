#pragma once

#include "UObject.h"
#include "TemplateLibrary.h"
#include "Camera.h"
#include "Renderer.h"
#include "PickingManager.h"
#include "AGizmo.h"

struct FFadeOverlay
{
    float alpha = 0.0f;
    bool bFading = false;
    void StartFadeOut(float duration = 1.0f) { bFading = false; }
    void StartFadeIn(float duration = 1.0f) { bFading = false; }
    void Update(float deltaTime) {}
    bool IsFading() const { return bFading; }
};

class Scene
{   

public:
    virtual ~Scene() = default;
    virtual void Initialize() {}
    virtual void Enter() {}
    virtual void Exit() {}
    
    virtual void Update(float deltaTime)
    {
        auto& objects = OBJECT.AllObjects;
        for (size_t i = 0; i < objects.size(); ++i)
        {
            if (objects[i] && objects[i]->GetIsActive())
            {
                objects[i]->Update(deltaTime);
            }
        }

        PICK.Update();//pickmanager
    }

    virtual void Render()
    {
        auto& objects = OBJECT.AllObjects;
        for (size_t i = 0; i < objects.size(); ++i)
        {
            if (objects[i] && objects[i]->GetIsActive())
            {
                // 기즈모는 모든 오브젝트와 아웃라인 렌더링 후 맨 마지막에 렌더링
                if (objects[i] == AGizmo::MainGizmo)
                    continue;

                objects[i]->Render();
            }
        }

        if (AGizmo::MainGizmo) {
            AActor* selected = AGizmo::MainGizmo->GetTargetActor();
            if (selected) {
                RENDERER.DrawOutline(selected);
            }
            // 기즈모를 항상 최상단에 렌더링
            AGizmo::MainGizmo->Render();
        }

        // 기본 깊이 상태 복원
        RENDERER.SetDefaultDepthState();
    }

    FFadeOverlay& GetFadeOverlay() { return m_fadeOverlay; }

protected:
    FFadeOverlay m_fadeOverlay;
};

