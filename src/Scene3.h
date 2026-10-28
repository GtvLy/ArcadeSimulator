#ifndef SCENE3_H
#define SCENE3_H

#include "Scene.h"
#include "ResourceManager.h"
#include "Button.h"
#include <SFML/Audio.hpp>
#include <vector>
#include "GameBase.h"

class Scene3 : public Scene {
private:
    sf::Sprite background;
    
    // 6个游戏按钮
    std::vector<Button> gameButtons;
    
    // 返回按钮
    Button returnButton;

    std::unique_ptr<GameBase> currentGame;  // 改为基类指针
    bool playingGame;

public:
    Scene3();
    ~Scene3();
    
    void handleInput(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    
    void onEnter() override;
    void onExit() override;

private:
    void selectGame(int gameIndex);  // 选择游戏
};

#endif