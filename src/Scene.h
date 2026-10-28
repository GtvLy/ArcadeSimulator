#ifndef SCENE_H
#define SCENE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

// 场景类型枚举
enum class SceneType {
    None,
    Scene1,  // 主菜单
    Scene2,  // 游戏场景
    Scene3   // 街机选择
};

// 场景基类（抽象类）
class Scene {
protected:
    bool isActive;
    SceneType nextScene;  // 下一个要切换的场景
    bool shouldChangeScene;  // 是否需要切换场景
    bool isChangingScene;

public:
    Scene() : isActive(true), nextScene(SceneType::None), shouldChangeScene(false) {}
    virtual ~Scene() {}
    
    // 纯虚函数，子类必须实现
    virtual void handleInput(const sf::Event& event, sf::RenderWindow& window) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    
    // 场景生命周期
    virtual void onEnter() {}   // 进入场景时调用
    virtual void onExit() {}    // 离开场景时调用
    
    // 场景控制
    bool isSceneActive() const { return isActive; }
    void setActive(bool active) { isActive = active; }
    
    // 场景切换
    bool needsSceneChange() const { return shouldChangeScene; }
    SceneType getNextScene() const { return nextScene; }
    
void requestSceneChange(SceneType scene) {
    if (!isChangingScene) {
        nextScene = scene;
        shouldChangeScene = true;
        isChangingScene = true;
        // 不要设置 isActive = false !!!
        std::cout << "Scene change requested to: " << static_cast<int>(scene) << std::endl;
    }
}
    
    void resetSceneChangeFlag() {
        isChangingScene = false;
        shouldChangeScene = false;
    }
};

#endif