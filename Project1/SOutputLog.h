#pragma once

class FOutputLog;
class IConsoleCommandExecutor;

class SOutputLog
{
public:
    SOutputLog(
        FOutputLog* InOutputLog,
        IConsoleCommandExecutor* InCommandExecutor
    );

    void Render();
    void RenderConsoleWindow();
    void RenderToolbar();
    void RenderFilter();
    void RenderInput();

private:
    FOutputLog* OutputLog;
    IConsoleCommandExecutor* CommandExecutor;

    char InputBuffer[256] = "";
};