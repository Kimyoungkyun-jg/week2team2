#pragma once
#include "FOutputDevice.h"
#include <string>
#include <vector>

class FOutputLog : public FOutputDevice
{
public:
    void Serialize(const std::string& Message) override;
    const std::vector<std::string>& GetLogs() const;
    void Clear();;

private:
    std::vector<std::string> Logs;
};