#pragma once

#include "UObject.h"
#include "TemplateLibrary.h"
#include "Camera.h"
#include "Renderer.h"

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
        auto& objects = ObjectManager::GetInstance().AllObjects;
        for (size_t i = 0; i < objects.size(); ++i)
        {
            if (objects[i] && objects[i]->GetIsActive())
            {
                objects[i]->Update(deltaTime);
            }
        }
    }

    virtual void Render()
    {
        auto& objects = ObjectManager::GetInstance().AllObjects;
        for (size_t i = 0; i < objects.size(); ++i)
        {
            if (objects[i] && objects[i]->GetIsActive())
            {
                objects[i]->Render();
            }
        }
    }

    FFadeOverlay& GetFadeOverlay() { return m_fadeOverlay; }

protected:
    FFadeOverlay m_fadeOverlay;
};

