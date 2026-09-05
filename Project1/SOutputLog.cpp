#include "pch.h"
#include "SOutputLog.h"
#include "FOutputLog.h"
#include "IConsoleCommandExecutor.h"

SOutputLog::SOutputLog(
    FOutputLog* InOutputLog,
    IConsoleCommandExecutor* InCommandExecutor)
    : OutputLog(InOutputLog)
    , CommandExecutor(InCommandExecutor)
{
}

void SOutputLog::Render()
{
    RenderConsoleWindow();

    ImGui::Begin("Output Log");
    
    RenderToolbar();
    RenderFilter();
    RenderInput();

    ImGui::End();
}

void SOutputLog::RenderConsoleWindow() {

    ImGuiIO& io = ImGui::GetIO();

    const float ConsoleHeight = 300.0f;

    ImGui::SetNextWindowPos(
        ImVec2(0.0f, io.DisplaySize.y - ConsoleHeight),
        ImGuiCond_Always
    );

    ImGui::SetNextWindowSize(
        ImVec2(io.DisplaySize.x, ConsoleHeight),
        ImGuiCond_Always
    );
}

void SOutputLog::RenderToolbar() {

    if (ImGui::Button("Add Debug Text"))
    {
        OutputLog->Serialize("Debug Text");
    }

    ImGui::SameLine();

    if (ImGui::Button("Add Debug Error"))
    {
        OutputLog->Serialize("[Error] Debug Error");
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear"))
    {
        OutputLog->Clear();
    }

    ImGui::SameLine();

    if (ImGui::Button("Copy"))
    {
        FString AllLogs;

        for (const FString& Log : OutputLog->GetLogs())
        {
            AllLogs += Log;
            AllLogs += '\n';
        }

        ImGui::SetClipboardText(AllLogs.c_str());
    }

    ImGui::Separator();

}
void SOutputLog::RenderFilter() {
  
    static ImGuiTextFilter Filter;

    //ImGui::BeginPopup("OpenPopUp");

    ImGui::Text("Options");

    //ImGui::EndPopup();

    ImGui::SameLine();

    Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 200.0f);

    ImGui::Separator();

    // 로그 출력 영역
    ImGui::BeginChild(
        "ScrollingRegion",
        ImVec2(0, -15),
        false,
        ImGuiWindowFlags_HorizontalScrollbar
    );

    if (OutputLog)
    {
        for (const FString& Log : OutputLog->GetLogs())
        {
            if (Filter.PassFilter(Log.c_str()))
            {
                ImGui::TextUnformatted(Log.c_str());
            }
        }
    }

    ImGui::EndChild();

    ImGui::Separator();
}

void SOutputLog::RenderInput() {
 
    ImGui::SetNextItemWidth(-100.0f);
    if (ImGui::InputText(
        "##Input",
        InputBuffer,
        IM_ARRAYSIZE(InputBuffer),
        ImGuiInputTextFlags_EnterReturnsTrue))
    {
        if (InputBuffer[0] != '\0')
            CommandExecutor->Exec(InputBuffer);

            // 입력창 비우기
            InputBuffer[0] = '\0';
        }
    ImGui::SameLine();
    ImGui::Text("Input");
}