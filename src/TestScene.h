#ifndef TESTSCENE_H
#define TESTSCENE_H

#include "Scene.h"
#include "ResourceManager.h"
#include "Button.h"  // 添加这行

class TestScene : public Scene {
private:
    sf::Sprite background;
    Button testButton;  // 添加测试按钮
    
public:
    TestScene();
    ~TestScene();
    
    void handleInput(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    
    void onEnter() override;
    void onExit() override;
};

#endif