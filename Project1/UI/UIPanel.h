#pragma once
#include "IUIPanel.h"
#include <string>

// FPS
class UIPanel_FPS : public IUIPanel
{
public:
    void Render() override;
};

// 메모리 스탯창
class UIPanel_Memory : public IUIPanel
{
public:
    void Render() override;
};

// 카메라 디버깅 창
class UIPanel_Camera : public IUIPanel
{
public:
    void Render() override;
};

// 스폰 창
class UIPanel_Spawn : public IUIPanel
{
public:
    void Render() override;
};

// 세이브/로드창
class UIPanel_SaveLoad : public IUIPanel
{
public:
    void Render() override;
};

// picking actor 속성창
class UIPanel_Picking : public IUIPanel
{
public:
    void Render() override;
private:
    float s_euler[3] = { 0.0f, 0.0f, 0.0f };    // euler 회전각 x, y, z
    string s_lastActorID;                       // 마지막으로 선택된 actor ID
    bool s_idEditing = false;                   // Imgui에서 수정되었는지?
};