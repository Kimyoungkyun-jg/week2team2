#pragma once
#include <string>
#include "UObject.h"
#include "Containers.h"

class SaveLoadManager
{
public:
    void SaveScene(const FString& path);
    TArray<UObject*> LoadScene(const FString& path);
};