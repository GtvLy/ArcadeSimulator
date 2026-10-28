#include "SceneManager.h"
#include <iostream>

SceneManager::SceneManager() 
    : currentScene(nullptr)
    , currentSceneType(SceneType::None)
{
    std::cout << "SceneManager initialized" << std::endl;
}

SceneManager::~SceneManager() {
    std::cout << "SceneManager destroyed" << std::endl;
}

void SceneManager::addScene(SceneType type, std::unique_ptr<Scene> scene) {
    scenes[type] = std::move(scene);
    std::cout << "Scene added: " << static_cast<int>(type) << std::endl;
}

void SceneManager::changeScene(SceneType type) {
    // 如果场景不存在，报错
    if (scenes.find(type) == scenes.end()) {
        std::cerr << "Scene not found: " << static_cast<int>(type) << std::endl;
        return;
    }
    
    // 退出当前场景
    if (currentScene != nullptr) {
        currentScene->onExit();
        currentScene->resetSceneChangeFlag();  // 重置标志
        std::cout << "Exiting scene: " << static_cast<int>(currentSceneType) << std::endl;
    }
    
    // 切换到新场景
    currentSceneType = type;
    currentScene = scenes[type].get();
    currentScene->resetSceneChangeFlag();  // 重置标志
    currentScene->onEnter();
    
    std::cout << "Entered scene: " << static_cast<int>(currentSceneType) << std::endl;
}

void SceneManager::update(float deltaTime) {
    if (currentScene != nullptr && currentScene->isSceneActive()) {
        std::cout << "[DEBUG] SceneManager updating scene: " << static_cast<int>(currentSceneType) << std::endl;
        currentScene->update(deltaTime);
        checkSceneChange();
    } else {
        std::cout << "[DEBUG] Scene not active or null!" << std::endl;
    }
}

void SceneManager::render(sf::RenderWindow& window) {
    if (currentScene != nullptr && currentScene->isSceneActive()) {
        currentScene->render(window);
    }
}

void SceneManager::handleInput(const sf::Event& event, sf::RenderWindow& window) {
    if (currentScene != nullptr && currentScene->isSceneActive()) {
        currentScene->handleInput(event, window);
    }
}

void SceneManager::checkSceneChange() {
    if (currentScene != nullptr && currentScene->needsSceneChange()) {
        SceneType nextScene = currentScene->getNextScene();
        changeScene(nextScene);
    }
}