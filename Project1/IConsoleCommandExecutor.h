#include "pch.h"
#pragma once

class IConsoleCommandExecutor
{
public:
    virtual ~IConsoleCommandExecutor() = default;

    virtual bool Exec(const FString& Input) = 0;

    virtual void GetExecHistory(
        std::vector<FString>& OutHistory) const = 0;

    virtual void GetSuggestedCompletions(
        const FString& Input,
        std::vector<FString>& OutSuggestions) const = 0;
};