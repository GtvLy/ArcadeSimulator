#ifndef GAME2_H
#define GAME2_H

#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include "Button.h"
#include "GameBase.h"
#include <vector>
#include <cmath>

// 赛车结构
struct Car {
    float x, y;           // 位置
    float z;              // 深度（用于3D效果）
    float speed;          // 速度
    float targetX;        // 目标X位置（AI用）
    sf::Color color;      // 颜色
    bool isPlayer;        // 是否是玩家
    float wobble;         // 摇晃效果
    float enginePulse;    // 引擎脉冲
};

// 赛道装饰物
struct RoadDecor {
    float y;              // Y位置
    float z;              // 深度
    int type;             // 类型（树、路标等）
    float offset;         // 偏移
};

// 粒子（尾气、火花等）
struct Game2Particle {  // 改名
    float x, y, z;
    float vx, vy;
    float life;
    sf::Color color;
    bool active;
};

class Game2 : public GameBase {
private:
    // 游戏状态
    bool gameOver;
    bool gamePaused;
    bool raceStarted;
    int score;
    float raceTime;
    
    // 玩家赛车
    Car playerCar;
    
    // NPC赛车
    static const int MAX_NPC_CARS = 5;
    Car npcCars[MAX_NPC_CARS];
    
    // 赛道
    float roadScroll;
    float roadSpeed;
    int roadSegments;
    
    // 赛道装饰
    static const int MAX_DECOR = 20;
    RoadDecor decorations[MAX_DECOR];
    
    // 粒子系统
    static const int MAX_PARTICLES = 100;
    Game2Particle particles[MAX_PARTICLES];
    
    // UI
    sf::Font gameFont;
    bool fontLoaded;
    Button returnButton;
    bool returnButtonInitialized;
    
    // 游戏计时
    sf::Clock gameClock;
    sf::Clock animClock;
    
    // 倒计时
    float countdownTimer;
    
    // 排名
    int playerRank;
    
    // 初始化函数
    void initCars();
    void initDecorations();
    
    // 更新函数
    void updateNPCCars(float deltaTime);
    void updateDecorations(float deltaTime);
    void updateParticles(float deltaTime);
    void checkCollisions();
    void calculateRanking();
    
    // 渲染函数
    void renderRoad(sf::RenderWindow& window);
    void renderCar(sf::RenderWindow& window, const Car& car, float scale);
    void renderDecorations(sf::RenderWindow& window);
    void renderParticles(sf::RenderWindow& window);
    void renderUI(sf::RenderWindow& window);
    void renderCountdown(sf::RenderWindow& window);
    
    // 辅助函数
    void createParticles(float x, float y, float z, sf::Color color, int count);
    float getPerspectiveScale(float z);
    sf::Color getDarkenedColor(sf::Color color, float factor);

public:
    Game2();
    ~Game2();
    
    void init() override;
    void handleInput(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    
    bool isGameRunning() const override { return !gameOver && !gamePaused; }
    bool isGameOver() const override { return gameOver; }
    int getScore() const override { return score; }
    void reset() override;
};

#endif