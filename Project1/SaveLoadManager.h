#pragma once
#include <string>
#include <UObject.h>

struct ObjData
{
    int version;
    int next_uuid;
    UObject obj;
}


class SaveLoadManager
{
public:
    void SaveScene(const FString& filePath);
    void LoadScene(const FString& filePath);

private:
    std::string filePath;


};