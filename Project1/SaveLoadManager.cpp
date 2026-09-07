#include "pch.h"
#include "SaveLoadManager.h"
#include "ObjectManager.h"
#include "Containers.h"
#include "AActor.h"
#include "USphere.h"
#include "UEngineStatics.h"
#include <fstream> // file input stream
#include <filesystem>


// 1. vs - 솔루션탐색기 - 프로젝트 우클릭 - NuGet 패키지 관리
// 2. nlohmann.json 검색 후 설치
// 추후 json.hpp 파일을 다운로드 후 ThirdParty 폴더에 업로드해 놓을 예정 (설치 불필요하도록)
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 타입 이름 String으로 받아서 Spawn
TMap<string, SaveLoadManager::CreatorFunc>& SaveLoadManager::GetActorCreatorRegistry()
{
    static TMap<string, CreatorFunc> registry;

    // 처음 호출 시에만 ACube, ASphere 등록
    if (registry.empty())
    {
        // "ACube" -> 상자 생성
        registry["ACube"] = [](FVector loc, FVector rot, FVector sc, EPrimitive prim) -> AActor *
        {
            AActor* actor = FObjectFactory::SpawnColider<ACube>(loc, sc);
            actor->SetRotation(rot);
            return actor;
        };
        
        // "ASphere" -> 구 생성
        registry["ASphere"] = [](FVector loc, FVector rot, FVector sc, EPrimitive prim) -> AActor *
        {
            AActor* actor = FObjectFactory::SpawnColider<ASphere>(loc, sc);
            actor->SetRotation(rot);
            return actor;
        };
    }
    
    return registry;
}


// 데이터 저장 - 직렬화(객체 -> json)
void SaveLoadManager::SaveScene(const FString& path)
{
    json sceneJson;
    json objectsJson = json::array(); // push_back으로 데이터 추가하기 위함

    // DEBUG
    OutputDebugStringA(("Current working dir: " + std::filesystem::current_path().string() + "\n").c_str());
    
    sceneJson["Version"] = 1;
    sceneJson["NextUUID"] = UEngineStatics::GetUUID();

    for (UObject* obj : ObjectManager::GetInstance().AllObjects)
    {
        AActor* actor = dynamic_cast<AActor*>(obj);
        if (!actor) continue;
        
        FVector location = actor->GetLocation();    // location 저장
        FVector rotation = actor->GetRotation();    // rotation 저장
        FVector scale = actor->GetScale();          // scale 저장
        EPrimitive type = actor->GetPrimitive();    // type 저장

        json objJson;
        objJson["UUID"]     = actor->GetID();
        objJson["Location"] = { location.x, location.y, location.z }; // {x,y,z}-> [x,y,z] 형태로 저장됨
        objJson["Rotation"] = { rotation.x, rotation.y, rotation.z };
        objJson["Scale"]    = { scale.x, scale.y, scale.z };
        objJson["Class"]    = string(actor->GetObjClassName()); // ACube, ASphere ...
        objJson["Type"]     = static_cast<int>(type);           // Sphere(0), Cube(1), None(2)


        objectsJson.push_back(objJson);
    }

    sceneJson["objects"] = objectsJson;

    std::ofstream file(path + ".Scene"); // 파일 경로

    if (!file.is_open())
    {
        assert(false && "Failed to Save objects!\n");
        return;
    }

    file << sceneJson.dump(4); // json 객체 -> string으로 변환 (4칸 들여쓰기)
    file.close();
}

// 데이터 로드 - 역직렬화(json -> 객체)
TArray<UObject*> SaveLoadManager::LoadScene(const FString& path)
{
    TArray<UObject*> loadedObjects;

    // DEBUG
    OutputDebugStringA(("Current working dir: " + std::filesystem::current_path().string() + "\n").c_str());

    std::ifstream file(path);

    if (!file.is_open())
    {
        assert(false && "Failed to Load objects!\n");
        OutputDebugStringA(("Failed to open: " + path + "\n").c_str());  // 추가

        return loadedObjects; // {} 빈 배열 return
    }

    json sceneJson;
    file >> sceneJson; // Load

    // 함수 Load 및 람다 등록
    auto& registry = GetActorCreatorRegistry();

    for (json objJson : sceneJson["objects"]){

        string Class     = objJson["Class"];  // ACube, ASphere ...
        auto it = registry.find(Class);

        // 등록되지 않은 AActor면 패스 (EX. Gizmo ...)
        if(it == registry.end())
            continue;

        auto uuid       = objJson["UUID"]; 
        auto location   = objJson["Location"];
        auto rotation   = objJson["Rotation"];
        auto scale      = objJson["Scale"];
        
        // 명시적 형변환 (float) 하여 x, y, z 값 가져오기
        FVector loc(location[0].get<float>(), location[1].get<float>(), location[2].get<float>());
        FVector rat(rotation[0].get<float>(), rotation[1].get<float>(), rotation[2].get<float>());
        FVector sc(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>());
        
        EPrimitive prim = static_cast<EPrimitive>(objJson["Type"].get<int>());

        AActor* actor = it->second(loc, rat, sc, prim);
        loadedObjects.push_back(actor);

    }

    return loadedObjects;

}