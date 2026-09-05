#include "pch.h"
#include "SaveLoadManager.h"
#include "ObjectManager.h"
#include "Containers.h"
#include "AActor.h"
#include "USphere.h"
#include <fstream>

// 1. vs - 솔루션탐색기 - 프로젝트 우클릭 - NuGet 패키지 관리
// 2. nlohmann.json 검색 후 설치
// 추후 json.hpp 파일을 다운로드 후 ThirdParty 폴더에 업로드해 놓을 예정 (설치 불필요하도록)
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 타입 이름

// 데이터 저장 - 직렬화(객체 -> json)
void SaveLoadManager::SaveScene(const FString& path)
{
    json sceneJson;
    json objectsJson = json::array();

    for (UObject* obj : ObjectManager::GetInstance().AllObjects)
    {
        AActor* actor = dynamic_cast<AActor*>(obj);
        if (!actor) continue;
        
        FVector location = actor->GetLocation();    // location 저장
        FVector rotation = actor->GetRotation();    // rotation 저장
        FVector scale = actor->GetScale();          // scale 저장
        EPrimitive type = actor->GetPrimitive();    // type 저장

        json objJson;
        // objJson["UUID"]     = actor->UUID;
        objJson["Location"] = { location.x, location.y, location.z };
        objJson["Rotation"] = { rotation.x, rotation.y, rotation.z };
        objJson["Scale"]    = { scale.x, scale.y, scale.z };
        objJson["Type"]     = string(actor->GetObjClassName());

        objectsJson.push_back(objJson);
    }

    sceneJson["objects"] = objectsJson;

    std::ofstream file(path + ".Scene"); // 파일 경로
    file << sceneJson.dump(4);
    file.close();
}

// 데이터 로드 - 역직렬화(json -> 객체)
TArray<UObject*> SaveLoadManager::LoadScene(const FString& path)
{

}