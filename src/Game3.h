#ifndef GAME3_H
#define GAME3_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "ResourceManager.h"
#include "Button.h"
#include "GameBase.h"
#include <vector>
#include <cmath>

// 玩家结构
struct Player {
    float x, y;
    float vx, vy;
    float width, height;
    bool onGround;
    bool jumping;
    int health;
    bool invincible;
    float invincibleTimer;
    int coins;
    bool isDead;
    float animTime;
    int animFrame;
};

// 敌人结构
struct Game3Enemy {
    float x, y;
    float vx;
    float width, height;
    bool active;
    bool moving;
    int type; // 0=蘑菇怪, 1=乌龟
    float animTime;
};

// 砖块/平台结构
struct Block {
    float x, y;
    float width, height;
    int type; // 0=普通砖, 1=问号砖, 2=隐藏砖, 3=管道
    bool broken;
    bool hasItem;
    float bounceOffset;
};

// 金币结构
struct Coin {
    float x, y;
    float vy;
    bool collected;
    float animTime;
};

// 道具结构
struct Item {
    float x, y;
    float vy;
    int type; // 0=蘑菇, 1=火焰花
    bool active;
    bool spawning;
    float spawnTimer;
};

// 粒子结构
struct Game3Particle {
    float x, y;
    float vx, vy;
    float life;
    sf::Color color;
    bool active;
    float size;
};

class Game3 : public GameBase {
private:
    // 游戏状态
    bool gameOver;
    bool gamePaused;
    bool levelComplete;
    int score;
    float gameTime;
    
    // 玩家
    Player player;
    
    // 关卡元素
    std::vector<Block> blocks;
    std::vector<Game3Enemy> enemies;
    std::vector<Coin> coins;
    std::vector<Item> items;
    
    // 粒子系统
    static const int MAX_PARTICLES = 150;
    Game3Particle particles[MAX_PARTICLES];
    
    // 摄像机
    float cameraX;
    
    // UI
    sf::Font gameFont;
    bool fontLoaded;
    Button returnButton;
    bool returnButtonInitialized;
    
    // 音效生成器（程序化音效）
    std::vector<sf::Int16> generateJumpSound();
    std::vector<sf::Int16> generateCoinSound();
    std::vector<sf::Int16> generateHitSound();
    std::vector<sf::Int16> generatePowerUpSound();
    void playProceduralSound(const std::vector<sf::Int16>& samples);
    
    sf::SoundBuffer jumpSoundBuffer;
    sf::SoundBuffer coinSoundBuffer;
    sf::SoundBuffer hitSoundBuffer;
    sf::SoundBuffer powerUpSoundBuffer;
    sf::Sound gameSound;
    
    // 游戏计时
    sf::Clock gameClock;
    
    // 关卡生成
    void generateLevel();
    void generateBlock(float x, float y, int type, bool hasItem = false);
    void generateEnemy(float x, float y, int type);
    void generateCoin(float x, float y);
    
    // 更新函数
    void updatePlayer(float deltaTime);
    void updateEnemies(float deltaTime);
    void updateCoins(float deltaTime);
    void updateItems(float deltaTime);
    void updateParticles(float deltaTime);
    void updateCamera();
    
    // 碰撞检测
    void checkPlayerBlockCollision();
    void checkPlayerEnemyCollision();
    void checkPlayerCoinCollision();
    void checkPlayerItemCollision();
    bool checkCollision(float x1, float y1, float w1, float h1,
                       float x2, float y2, float w2, float h2);
    
    // 渲染函数
    void renderBackground(sf::RenderWindow& window);
    void renderBlocks(sf::RenderWindow& window);
    void renderPlayer(sf::RenderWindow& window);
    void renderEnemies(sf::RenderWindow& window);
    void renderCoins(sf::RenderWindow& window);
    void renderItems(sf::RenderWindow& window);
    void renderParticles(sf::RenderWindow& window);
    void renderUI(sf::RenderWindow& window);
    
    // 辅助函数
    void createParticles(float x, float y, sf::Color color, int count);
    void hitBlock(Block& block);
    void collectCoin(Coin& coin);
    void killEnemy(Game3Enemy& enemy);
    void damagePlayer();

public:
    Game3();
    ~Game3();
    
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