#include "pch.h"
#pragma once
#include "FOutputDevice.h"

enum class ELogVerbosity
{
    Log,
    Warning,
    Error
};

struct FLogMessage
{
    FString Message;
    FString Category;
    ELogVerbosity Verbosity;
};

class FOutputLog : public FOutputDevice
{
public:
    void Serialize(const FString& Message) override;
    const std::vector<FString>& GetLogs() const;
    void Clear();

private:
    std::vector<FString> Logs;
};