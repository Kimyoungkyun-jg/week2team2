#include "pch.h"
#include "SaveLoadManager.h"
#include "ObjectManager.h"
#include "Containers.h"

void SaveLoadManager::SaveScene(const FString& path)
{
    ObjectManager::GetInstance().GetOrCreateClassInfo();
}

TArray<UObject*> SaveLoadManager::LoadScene(const FString& path)
{

}