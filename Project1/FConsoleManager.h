#pragma once

#include "IConsoleManager.h"

class FConsoleManager : public IConsoleManager
{
public:
    static FConsoleManager& GetInstance();

    void RegisterConsoleCommand(
        const FString& Name,
        const FString& Help,
        FConsoleCommand Command) override;

    bool ExecuteConsoleCommand(
        const FString& Name,
        const std::vector<FString>& Args) override;

    std::vector<FString> GetCommandNames() const override;

    FString GetHelp(const FString& Name) const override;

private:
    struct FCommandInfo
    {
        FString Help;
        FConsoleCommand Command;
    };

    std::unordered_map<FString, FCommandInfo> Commands;
};