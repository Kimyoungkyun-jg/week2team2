#pragma once
#include "Containers.h"
#include <functional>
#include <unordered_map>

class UObject;
class AActor;
class SaveLoadManager
{
public:
    static void SaveScene(const FString& path);
    static TArray<UObject*> LoadScene(const FString& path);

private:
    // location, ratation, scale, type
    using CreatorFunc = std::function<AActor*(FVector, FVector, FVector, EPrimitive)>;
    
    static TMap<string, CreatorFunc>& GetActorCreatorRegistry();
};