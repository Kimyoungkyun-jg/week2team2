#include "pch.h"
#include "FConsoleCommandExecutor.h"
#include "FOutputLog.h"

FConsoleCommandExecutor::FConsoleCommandExecutor(FOutputLog* InOutputLog)
    : OutputLog(InOutputLog)
{
}

bool FConsoleCommandExecutor::Exec(const FString& Input)
{
    if (Input.empty())
        return false;

    // 입력한 명령 기록
    History.push_back(Input);

    // clear 명령
    if (Input == "clear")
    {
        OutputLog->Clear();
        return true;
    }

    // help 명령
    if (Input == "help")
    {
        OutputLog->Serialize("clear - Clear Output Log");
        OutputLog->Serialize("help - Show Commands");
        return true;
    }

    // 없는 명령
    OutputLog->Serialize("Unknown Command: " + Input);

    return false;
}

void FConsoleCommandExecutor::GetExecHistory(
    std::vector<FString>& OutHistory) const
{
    OutHistory = History;
}

void FConsoleCommandExecutor::GetSuggestedCompletions(
    const FString& Input,
    std::vector<FString>& OutSuggestions) const
{
    OutSuggestions.clear();

    if ("clear"s.find(Input) == 0)
        OutSuggestions.push_back("clear");

    if ("help"s.find(Input) == 0)
        OutSuggestions.push_back("help");
}