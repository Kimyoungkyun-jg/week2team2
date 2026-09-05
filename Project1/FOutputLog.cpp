#include "pch.h"
#include "FOutputLog.h"

void FOutputLog::Serialize(const FString& Message)
{
    Logs.push_back(Message);
}

const std::vector<FString>& FOutputLog::GetLogs() const
{
    return Logs;
}

void FOutputLog::Clear()
{
    Logs.clear();
}