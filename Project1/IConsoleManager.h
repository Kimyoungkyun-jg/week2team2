#include "pch.h"
#pragma once

class IConsoleManager
{
public:
    using FConsoleCommand =
        std::function<void(const std::vector<FString>&)>;

public:
    virtual ~IConsoleManager() = default;

    static IConsoleManager& Get();

    virtual void RegisterConsoleCommand(
        const FString& Name,
        const FString& Help,
        FConsoleCommand Command) = 0;

    virtual bool ExecuteConsoleCommand(
        const FString& Name,
        const std::vector<FString>& Args) = 0;

    virtual std::vector<FString> GetCommandNames() const = 0;

    virtual FString GetHelp(const FString& Name) const = 0;
};

