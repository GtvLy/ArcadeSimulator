#ifndef SCENE1_H
#define SCENE1_H

#include "Scene.h"
#include "ResourceManager.h"
#include "Button.h"
#include <SFML/Audio.hpp>

class Scene1 : public Scene {
private:
    sf::Sprite background;
    sf::Music* bgMusic;  // 背景音乐
    
    // 主菜单按钮
    Button startButton;
    Button helpButton;
    
    // 帮助页面
    bool showingHelp;  // 是否显示帮助页面
    sf::Sprite helpBackground;
    Button returnButton;
    
public:
    Scene1();
    ~Scene1();
    
    void handleInput(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    
    void onEnter() override;
    void onExit() override;
    
private:
    void showHelpPage();     // 显示帮助页面
    void hideHelpPage();     // 隐藏帮助页面
    void startGame();        // 开始游戏
};

#endif