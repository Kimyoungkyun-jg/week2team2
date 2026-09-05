#include "pch.h"
#include "SOutputLog.h"
#include "FOutputLog.h"

SOutputLog::SOutputLog(FOutputLog* InOutputLog) : OutputLog(InOutputLog)
{
}

void SOutputLog::Render()
{
    ImGui::SetNextWindowSize(ImVec2(750, 400), ImGuiCond_FirstUseEver);

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

    if (!ImGui::Begin("Output Log"))
    {
        ImGui::End();
        return;
    }

    // 상단 버튼
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
        std::string AllLogs;

        for (const std::string& Log : OutputLog->GetLogs())
        {
            AllLogs += Log;
            AllLogs += '\n';
        }

        ImGui::SetClipboardText(AllLogs.c_str());
    }

    ImGui::Separator();

    // 필터
    static ImGuiTextFilter Filter;

    ImGui::Button("Options");
    ImGui::SameLine();

    Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 200.0f);

    ImGui::Separator();

    // 로그 출력 영역
    ImGui::BeginChild(
        "ScrollingRegion",
        ImVec2(0, -35),
        false,
        ImGuiWindowFlags_HorizontalScrollbar
    );

    if (OutputLog)
    {
        for (const std::string& Log : OutputLog->GetLogs())
        {
            if (Filter.PassFilter(Log.c_str()))
            {
                ImGui::TextUnformatted(Log.c_str());
            }
        }
    }

    if (ImGui::InputText(
        "##Input",
        InputBuffer,
        IM_ARRAYSIZE(InputBuffer),
        ImGuiInputTextFlags_EnterReturnsTrue))
    {
        if (InputBuffer[0] != '\0')
        {
            OutputLog->Serialize(InputBuffer);
            InputBuffer[0] = '\0';
        }
    }

    ImGui::EndChild();

    ImGui::Separator();

    // 하단 입력창
    ImGui::SetNextItemWidth(-50.0f);
    ImGui::InputText("##Input", InputBuffer, IM_ARRAYSIZE(InputBuffer));

    ImGui::SameLine();
    ImGui::Text("Input");

    ImGui::End();
}