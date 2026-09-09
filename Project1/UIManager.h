#pragma once
#include "Containers.h"

class IUIPanel;

class UIManager
{
public:    
	// pch.h 매크로 지정 완료 (IMGUI)
    static UIManager& GetInstance();

    void Initialize();                     // panel 생성
    void RenderAll();                      // 모든 panel render
    void Shutdown();                       // ~UIManager()에서 사용하는 panel clear 함수
    ~UIManager();   
    
private:
    void RegisterPanel(IUIPanel* panel);   // panel 등록
    TArray<IUIPanel*> Panels;

};