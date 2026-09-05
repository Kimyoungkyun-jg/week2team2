#pragma once
//#include "UScene.h"
class Scene;
class Renderer;

class SceneManager
{
public:
    static SceneManager& GetInstance();
    Scene* GetCurrentScene() { return m_currentScene; }
    FString GetCurrentSceneName() { return m_currentSceneName; }
    void Initialize();

    void AddScene(const FString& name, Scene* scene);
    void RequestChangeScene(const FString& name);
    void ChangeScene(const FString& name);
    void Update(float deltaTime);
    void Render();

private:
    SceneManager() = default;
    ~SceneManager() = default;

private:
    TMap<FString, Scene*> m_scenes;
    Scene* m_currentScene = nullptr;
    FString m_currentSceneName;

    bool m_hasPendingSceneChange = false;
    FString m_pendingSceneName;
};

