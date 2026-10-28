#ifndef TESTSPRITESCENE_H
#define TESTSPRITESCENE_H

#include "Scene.h"
#include "SpriteSheet.h"

class TestSpriteScene : public Scene {
private:
    SpriteSheet testAnim;
    sf::RectangleShape background;
    bool isFlipped;
    
public:
    TestSpriteScene();
    ~TestSpriteScene();
    
    void handleInput(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    
    void onEnter() override;
    void onExit() override;
};

#endif