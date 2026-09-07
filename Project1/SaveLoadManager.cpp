#include "pch.h"
#include "SaveLoadManager.h"
#include "ObjectManager.h"
#include "Containers.h"
#include "AActor.h"
#include "USphere.h"
#include "UEngineStatics.h"
#include <fstream> // file input stream
#include <filesystem>

// Version 상수 처리 
constexpr int CURRENT_SCENE_VERSION = 1;

// 1. vs - 솔루션탐색기 - 프로젝트 우클릭 - NuGet 패키지 관리
// 2. nlohmann.json 검색 후 설치
// 추후 json.hpp 파일을 다운로드 후 ThirdParty 폴더에 업로드해 놓을 예정 (설치 불필요하도록)
/////////////////////////
/////// 반영 완료! ///////
/////////////////////////

#include <nlohmann/json.hpp>

// 알파벳 순서가 아닌 input 순서로 push하기 위함
using json = nlohmann::ordered_json;

// 타입 이름 String으로 받아서 Spawn
TMap<string, SaveLoadManager::CreatorFunc>& SaveLoadManager::GetActorCreatorRegistry()
{
    static TMap<string, CreatorFunc> registry;

    // 처음 호출 시에만 ACube, ASphere 등록
    if (registry.empty())
    {
        // "Cube" -> 상자 생성
        registry["Cube"] = [](FVector loc, FVector rot, FVector sc) -> AActor *
        {
            AActor* actor = FObjectFactory::SpawnColider<ACube>(loc, sc);
            actor->SetRotation(rot);
            return actor;
        };
        
        // "Sphere" -> 구 생성
        registry["Sphere"] = [](FVector loc, FVector rot, FVector sc) -> AActor *
        {
            AActor* actor = FObjectFactory::SpawnColider<ASphere>(loc, sc);
            actor->SetRotation(rot);
            return actor;
        };

        // // "Circle" -> 원 생성
        // registry["Circle"] = [](FVector loc, FVector rot, FVector sc) -> AActor *
        // {
        //     AActor* actor = FObjectFactory::SpawnColider<ACircle>(loc, sc);
        //     actor->SetRotation(rot);
        //     return actor;
        // };

        // // "Rectangle" -> 사각형 생성
        // registry["Rectangle"] = [](FVector loc, FVector rot, FVector sc) -> AActor *
        // {
        //     AActor* actor = FObjectFactory::SpawnColider<ARec>(loc, sc);
        //     actor->SetRotation(rot);
        //     return actor;
        // };

        // // "Triangle" -> 삼각형 생성
        // registry["Triangle"] = [](FVector loc, FVector rot, FVector sc) -> AActor *
        // {
        //     AActor* actor = FObjectFactory::SpawnColider<ATri>(loc, sc);
        //     actor->SetRotation(rot);
        //     return actor;
        // };
    }
    
    return registry;
}

// EPrimitive (0, 1 ...) -> Str (Sphere, Cube ...)
string SaveLoadManager::EPrimitiveToStr(EPrimitive prim)
{
    switch (prim)
    {
        case EPrimitive::Cube : return "Cube";
        case EPrimitive::Sphere : return "Sphere";
        case EPrimitive::Circle : return "Circle";
        case EPrimitive::Rectangle : return "Rectangle";
        case EPrimitive::Triangle : return "Triangle";
        case EPrimitive::Gizmo : return "Gizmo";
        default : return "None";
    }
}

////////////////////////////
/////////// SAVE ///////////
////////////////////////////

// 데이터 저장 - 직렬화(객체 -> json)
void SaveLoadManager::SaveScene(const FString& path)
{
    json sceneJson;

    sceneJson["Version"] = CURRENT_SCENE_VERSION;
    sceneJson["NextUUID"] = UEngineStatics::PeekUUID() - 1;
    
    // DEBUG
    // OutputDebugStringA(("Current working dir: " + std::filesystem::current_path().string() + "\n").c_str());
    
    json objectsJson = json::object(); // key, value 형식으로 저장하기 위함
    
    int index = 0;

    for (UObject* obj : ObjectManager::GetInstance().AllObjects)
    {
        AActor* actor = dynamic_cast<AActor*>(obj);
        if (!actor) continue;
        
        FVector location = actor->GetLocation();    // location 저장
        FVector rotation = actor->GetRotation();    // rotation 저장
        FVector scale = actor->GetScale();          // scale 저장
        EPrimitive type = actor->GetPrimitive();    // type 저장
        // if (type == EPrimitive::Gizmo) continue; // Gizmo면 pass
        
        json objJson;
        // objJson["UUID"]     = actor->GetID();
        objJson["Location"] = { location.x, location.y, location.z }; // {x,y,z}-> [x,y,z] 형태로 저장됨
        objJson["Rotation"] = { rotation.x, rotation.y, rotation.z };
        objJson["Scale"]    = { scale.x, scale.y, scale.z };
        // objJson["Class"]    = string(actor->GetObjClassName()); // ACube, ASphere ...
        objJson["Type"]     = EPrimitiveToStr(type);           // Sphere -> "Sphere", Cube -> "Cube"

        objectsJson[std::to_string(index)] = objJson; // 0 -> "0", 1 -> "1" ...
        ++index;
    }
    
    sceneJson["Primitives"] = objectsJson;
    
    std::ofstream file(path + ".Scene"); // 파일 경로
    
    if (!file.is_open())
    {
        assert(false && "Failed to Save objects!\n");
        return;
    }
    
    file << sceneJson.dump(4); // json 객체 -> string으로 변환 (4칸 들여쓰기)
    file.close();
}

////////////////////////////
/////////// LOAD ///////////
////////////////////////////

// 데이터 로드 - 역직렬화(json -> 객체)
TArray<UObject*> SaveLoadManager::LoadScene(const FString& path)
{
    TArray<UObject*> loadedObjects;

    // DEBUG
    // OutputDebugStringA(("Current working dir: " + std::filesystem::current_path().string() + "\n").c_str());

    std::ifstream file(path);

    if (!file.is_open())
    {
        assert(false && "Failed to Load objects!\n");
        OutputDebugStringA(("Failed to open: " + path + "\n").c_str());  // 추가

        return loadedObjects; // {} 빈 배열 return
    }

    json sceneJson;

    // Parsing Check
    try
    {
        file >> sceneJson; // Load
    }
    catch(const std::exception& e)
    {
        OutputDebugStringA(("Parse failed : " + string(e.what())).c_str());
        return loadedObjects; // {} 빈 배열 return

    }

    // 기존 Scene에 있던 Objects Clear
    ObjectManager::GetInstance().DestroyAllActors();

    // Format Version Check
    int version = sceneJson["Version"].get<int>();
    if (version != CURRENT_SCENE_VERSION)
        OutputDebugStringA("Scene Version mismatch!");

    // 함수 Load 및 람다 등록
    auto& registry = GetActorCreatorRegistry();

    for (json objJson : sceneJson["Primitives"]){

        string Class     = objJson["Type"];  // Cube, Sphere ...

        auto it = registry.find(Class);

        // 등록되지 않은 AActor면 패스 (EX. Gizmo ...)
        if(it == registry.end())
            continue;

        // auto uuid       = objJson["UUID"]; 
        auto location   = objJson["Location"];
        auto rotation   = objJson["Rotation"];
        auto scale      = objJson["Scale"];
        
        // 명시적 형변환 (float) 하여 x, y, z 값 가져오기
        FVector loc(location[0].get<float>(), location[1].get<float>(), location[2].get<float>());
        FVector rat(rotation[0].get<float>(), rotation[1].get<float>(), rotation[2].get<float>());
        FVector sc(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>());
        
        // EPrimitive prim = static_cast<EPrimitive>(objJson["Type"].get<int>());

        AActor* actor = it->second(loc, rat, sc);
        loadedObjects.push_back(actor);

    }

    return loadedObjects;

}