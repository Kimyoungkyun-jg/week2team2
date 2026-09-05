#pragma once
#include "FOutputLog.h"

class SOutputLog
{
public:
    SOutputLog(FOutputLog* InOutputLog);

    void Render();

private:
    FOutputLog* OutputLog;
    char InputBuffer[256] = "";
};