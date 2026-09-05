#include "pch.h"
#pragma once
#include "IConsoleCommandExecutor.h"

class FOutputLog;

class FConsoleCommandExecutor : public IConsoleCommandExecutor
{
public:
    FConsoleCommandExecutor(FOutputLog* InOutputLog);

    bool Exec(const FString& Input) override;

    void GetExecHistory(
        std::vector<FString>& OutHistory) const override;

    void GetSuggestedCompletions(
        const FString& Input,
        std::vector<FString>& OutSuggestions) const override;

private:
    FOutputLog* OutputLog;

    std::vector<FString> History;
};