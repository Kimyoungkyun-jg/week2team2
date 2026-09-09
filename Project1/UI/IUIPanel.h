#pragma once

class IUIPanel
{
public:
    virtual ~IUIPanel() = default;
    virtual void Render() = 0;

    bool IsOpen() const { return bIsOpen; };
    void SetOpen(bool bOpen) { bIsOpen = bOpen; }

protected:
    bool bIsOpen = true; // 창 open 여부 (default : 열림)
};