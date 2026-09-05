#pragma once
//#include "UScene.h"
class Scene;
class Renderer;

class SceneManager
{
public:
    static SceneManager& GetInstance();
    Scene* GetCurrentScene() { return m_currentScene; }
    string GetCurrentSceneName() { return m_currentSceneName; }
    void Initialize();

    void AddScene(const string& name, Scene* scene);
    void RequestChangeScene(const string& name);
    void ChangeScene(const string& name);
    void Update(float deltaTime);
    void Render();

private:
    SceneManager() = default;
    ~SceneManager() = default;

private:
    unordered_map<string, Scene*> m_scenes;
    Scene* m_currentScene = nullptr;
    string m_currentSceneName;

    bool m_hasPendingSceneChange = false;
    string m_pendingSceneName;
};

