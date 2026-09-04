#pragma once
//#include "UScene.h"
class Scene;
class Renderer;

class SceneManager
{
public:
    static SceneManager& GetInstance();
    Scene* GetCurrentScene() { return m_currentScene; }
    std::string GetCurrentSceneName() { return m_currentSceneName; }
    void Initialize();

    void AddScene(const std::string& name, Scene* scene);
    void RequestChangeScene(const std::string& name);
    void ChangeScene(const std::string& name);
    void Update(float deltaTime);
    void Render();

private:
    SceneManager() = default;
    ~SceneManager() = default;

private:
    std::unordered_map<std::string, Scene*> m_scenes;
    Scene* m_currentScene = nullptr;
    std::string m_currentSceneName;

    bool m_hasPendingSceneChange = false;
    std::string m_pendingSceneName;
};

