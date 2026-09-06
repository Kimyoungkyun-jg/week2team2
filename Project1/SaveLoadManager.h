#pragma once
#include "Containers.h"
#include <functional>
#include <unordered_map>

class UObject;
class AActor;
class SaveLoadManager
{
public:
    // utility class 이므로 생성자 금지
    SaveLoadManager() = delete;
    ~SaveLoadManager() = delete;

    static void SaveScene(const FString& path);
    static TArray<UObject*> LoadScene(const FString& path);

private:
    // <location, ratation, scale, type> 을 인자로 받아서 AACtor*를 반환하는 공통타입 함수
    using CreatorFunc = std::function<AActor*(FVector, FVector, FVector, EPrimitive)>;
    static TMap<string, CreatorFunc>& GetActorCreatorRegistry();

};