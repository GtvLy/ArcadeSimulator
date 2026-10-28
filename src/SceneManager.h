#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"
#include <memory>
#include <map>

class SceneManager {
private:
    std::map<SceneType, std::unique_ptr<Scene>> scenes;
    Scene* currentScene;
    SceneType currentSceneType;
    
public:
    SceneManager();
    ~SceneManager();
    
    // 添加场景
    void addScene(SceneType type, std::unique_ptr<Scene> scene);
    
    // 切换场景
    void changeScene(SceneType type);
    
    // 更新当前场景
    void update(float deltaTime);
    
    // 渲染当前场景
    void render(sf::RenderWindow& window);
    
    // 处理输入
    void handleInput(const sf::Event& event, sf::RenderWindow& window);
    
    // 检查并处理场景切换请求
    void checkSceneChange();
    
    // 获取当前场景类型
    SceneType getCurrentSceneType() const { return currentSceneType; }
};

#endif