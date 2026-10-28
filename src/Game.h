#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "SceneManager.h"  // 添加这行

class Game {
private:
    sf::RenderWindow window;
    sf::Vector2u baseResolution;
    float scale;
    sf::View gameView;
    bool running;
    
    SceneManager sceneManager;  // 添加场景管理器
    sf::Clock clock;            // 添加时钟用于计算deltaTime
    
    void processEvents();
    void update();
    void render();
    void updateScale();

public:
    Game();
    ~Game();
    
    void run();
    
    float getScale() const { return scale; }
    sf::Vector2u getBaseResolution() const { return baseResolution; }
};

#endif