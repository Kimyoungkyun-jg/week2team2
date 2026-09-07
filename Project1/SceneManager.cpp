#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Renderer.h"


SceneManager& SceneManager::GetInstance()
{
    static SceneManager instance;
    return instance;
}

void SceneManager::Initialize()
{
	for (auto& it : m_scenes)
	{
		it.second->Initialize();
	}
}

void SceneManager::AddScene(const FString& name, Scene* scene)
{
	if (m_scenes.Contains(name))
		return;

	m_scenes[name] = scene;
}

void SceneManager::RequestChangeScene(const FString& name)
{
	if (m_hasPendingSceneChange) return;
	m_hasPendingSceneChange = true;
	m_pendingSceneName = name;
	if (m_currentScene)
	{
		m_currentScene->GetFadeOverlay().StartFadeOut(1.0f);
	}
}

void  SceneManager::ChangeScene(const FString& name) 
{
	if (m_currentScene != nullptr) 
	{
		m_currentScene->Exit();
	}
	m_currentScene = m_scenes[name];
	m_currentSceneName = name;
	if (m_currentScene != nullptr) 
	{
		m_currentScene->Enter();
	}
}

void SceneManager::Update(float deltaTime)
{
	//if (m_currentScene)
	//{
	//	m_currentScene->GetFadeOverlay().Update(deltaTime);
	//}

	//if (m_hasPendingSceneChange && (m_currentScene == nullptr || !m_currentScene->GetFadeOverlay().IsFading()))
	//{
	//	m_hasPendingSceneChange = false;
	//	string nextScene = m_pendingSceneName;
	//	ChangeScene(nextScene);
	//	if (m_currentScene)
	//	{
	//		m_currentScene->GetFadeOverlay().StartFadeIn(1.0f);
	//	}
	//}


	if (m_currentScene)
	{
		m_currentScene->Update(deltaTime);
	}
}

void SceneManager::Render()
{
	if (m_currentScene) m_currentScene->Render();
}
