#include "pch.h"
#include "UIPanel.h"
#include "UIManager.h"

// "IMGUI" 매크로 등록 완료
UIManager& UIManager::GetInstance()
{
    static UIManager instance;
    return instance;
}

// panel 생성
void UIManager::Initialize()
{
    RegisterPanel(new UIPanel_Memory);
    RegisterPanel(new UIPanel_Camera);
    RegisterPanel(new UIPanel_Spawn);
    RegisterPanel(new UIPanel_SaveLoad);
    RegisterPanel(new UIPanel_Picking);
}

// panel 등록
void UIManager::RegisterPanel(IUIPanel* panel)
{
    Panels.push_back(std::move(panel));

}

// 모든 panel render
void UIManager::RenderAll()
{
    for (IUIPanel* panel : Panels)
    {
        if (panel->IsOpen()) panel->Render();
    }
}

// ~UIManager()에서 사용하는 panel clear 함수
void UIManager::Shutdown()
{
    for (IUIPanel* panel : Panels)
    {
        delete panel;
    }
    Panels.clear();
}

UIManager::~UIManager()
{
    Shutdown();
}