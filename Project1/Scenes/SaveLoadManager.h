#pragma once
#include "Containers.h"
#include <functional>
#include <unordered_map>

class UObject;
class AActor;

class SaveLoadManager
{
public:

static SaveLoadManager& GetInstance();
static void SaveScene(const FString& path);
static TArray<UObject*> LoadScene(const FString& path);

private:
// <location, ratation, scale> 을 인자로 받아서 AACtor*를 반환하는 공통타입 함수
using CreatorFunc = std::function<AActor*(FVector, FQuaternion, FVector)>;
static TMap<string, CreatorFunc>& GetActorCreatorRegistry();
static string EPrimitiveToStr(EPrimitive prim);

SaveLoadManager() = default;
~SaveLoadManager() = default;
};