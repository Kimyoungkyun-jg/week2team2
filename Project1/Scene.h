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
    
    virtual void Update(float deltaTime) {}
    virtual void Render() {
        Renderer::GetInstance().SetViewMatrix(Camera::GetInstance().GetViewMatrix());
        Renderer::GetInstance().SetProjMatrix(Camera::GetInstance().GetProjectionMatrix(Renderer::GetInstance().GetAspectRatio()));
    };

    FFadeOverlay& GetFadeOverlay() { return m_fadeOverlay; }

protected:
    FFadeOverlay m_fadeOverlay;
  
    // 마우스 클릭 - 버튼 대응
    //void HandleClick(float mouseX, float mouseY);


    std::vector<UObject*> SceneObjects;

};

