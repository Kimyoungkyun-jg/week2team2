#include "pch.h"
#include <string>
#pragma once

class FOutputDevice
{
public:
    virtual ~FOutputDevice() = default;

    virtual void Serialize(const std::string& Message) = 0;
};