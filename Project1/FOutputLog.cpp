#include "pch.h"
#include "FOutputLog.h"

void FOutputLog::Serialize(const std::string& Message)
{
    Logs.push_back(Message);
}

const std::vector<std::string>& FOutputLog::GetLogs() const
{
    return Logs;
}

void FOutputLog::Clear()
{
    Logs.clear();
}