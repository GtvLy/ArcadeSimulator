#ifndef GAME4_H
#define GAME4_H

#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include "Button.h"
#include "GameBase.h"
#include <string>
#include <sstream>
#include <iomanip>

class Game4 : public GameBase  {
private:
    static const int MAP_WIDTH = 16;
    static const int MAP_HEIGHT = 16;
    static const int NUM_RAYS = 80;  // 减少光线数量以适应320x180
    
    const float FOV = 3.14159f / 3.0f;
    const float MAX_DEPTH = 20.0f;
    
    // 迷宫地图 (1=墙, 0=空地, 2=终点)
    int worldMap[16][16];
    
    struct Player {
        float x, y;
        float angle;
        float moveSpeed;
        float rotSpeed;
    };
    
    Player player;
    
    sf::Font gameFont;
    bool fontLoaded;
    
    sf::Clock gameClock;
    sf::Clock animationClock;
    float completionTime;
    bool gameWon;
    bool gamePaused;
    
    Button returnButton;
    bool returnButtonInitialized;
    
    std::string formatTime(float seconds);
    sf::Color getWallColor(int x, int y, bool isVertical, float distance);
    void castRays(sf::RenderWindow& window);
    void drawMinimap(sf::RenderWindow& window);
    bool checkCollision(float x, float y);

public:
    Game4();
    ~Game4();
    
    void init();
    void handleInput(const sf::Event& event, sf::RenderWindow& window);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
    bool isGameRunning() const { return !gameWon && !gamePaused; }
    bool isGameOver() const { return gameWon; }
    int getScore() const { return static_cast<int>(completionTime * 100); }
    void reset();
};

#endif