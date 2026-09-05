#pragma once
#include <string>
#include <UObject.h>
#include <Containers.h>

class SaveLoadManager
{
public:
    void SaveScene(const TArray<UObject*>& objects, const FString& path);
    TArray<UObject*> LoadScene(const FString& path);
};