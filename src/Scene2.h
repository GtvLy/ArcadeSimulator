#ifndef SCENE2_H
#define SCENE2_H

#include "Scene.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "Player.h"
#include "NPCManager.h"
#include "Button.h"  // 用于F键提示
#include "SpecialButton.h"
#include <SFML/Audio.hpp>

class Scene2 : public Scene {
private:
    // 背景图层
    sf::Sprite bgSprite;       // 背景
    sf::Sprite lightSprite;    // 灯光
    sf::Sprite treeSprite;     // 树木
    
    // 音乐
    sf::Music* bgMusic;
    
    // 摄像机
    Camera camera;
    
    // 玩家
    Player player;
    
    // 场景信息
    sf::Vector2f sceneSize;    // 场景大小（483x180）
    float spriteScale;         // 精灵缩放比例

    // NPC管理器
    NPCManager npcManager;

    // F键交互提示
    SpecialButton fKeyPrompt;
    bool showFKeyPrompt;  // 是否显示F键提示

    void checkInteractionZone();  // 检查是否在交互区域（X: 240-300）

    // 返回按钮
    Button returnButton;

    bool isReturning;

    bool isFirstEnter;  // 是否是第一次进入场景

public:
    Scene2();
    ~Scene2();
    
    void handleInput(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    
    void onEnter() override;
    void onExit() override;

private:
    void calculateSpriteScale();  // 计算精灵缩放比例
};

#endif