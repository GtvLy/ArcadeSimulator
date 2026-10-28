#ifndef GAME5_H
#define GAME5_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "ResourceManager.h"
#include "Button.h"
#include "GameBase.h"

// 粒子结构
struct Particle {
    float x, y;
    float vx, vy;
    float life;
    float size;
    sf::Color color;
    bool active;
};

// 蛇节点结构
struct SnakeNode {
    int x, y;
};

// 食物结构
struct Food {
    int x, y;
    bool active;
    float pulseTimer;
};

// 敌人结构
struct Enemy {
    float x, y;
    float vx, vy;
    float angle;
    bool active;
    int type;
    float stateTimer;
    float glowPulse;
};

// 障碍物结构
struct Obstacle {
    int x, y;
    int type;
    float rotation;
    bool active;
    float pulseTimer;
};

// 道具结构
struct PowerUp {
    int x, y;
    int type;
    float spawnTimer;
    bool active;
    float rotationAngle;
};

// 音效管理器
class SnakeSoundManager {
private:
    sf::SoundBuffer eatBuffer, powerUpBuffer, hitBuffer, gameOverBuffer, moveBuffer;
    sf::Sound eatSound, powerUpSound, hitSound, gameOverSound, moveSound;
    bool soundEnabled;
    float volume;
    
    void generateEatSound();
    void generatePowerUpSound();
    void generateHitSound();
    void generateGameOverSound();
    void generateMoveSound();

public:
    SnakeSoundManager();
    
    void playEat();
    void playPowerUp();
    void playHit();
    void playGameOver();
    void playMove();
    void setVolume(float vol);
    void toggleSound();
    bool isSoundEnabled() const { return soundEnabled; }
    float getVolume() const { return volume; }
};

class Game5 : public GameBase {
private:
    static const int GRID_WIDTH = 40;   // 调整为 320x180 适配
    static const int GRID_HEIGHT = 22;  // 调整为 320x180 适配
    static const int CELL_SIZE = 8;     // 缩小格子以适应 320x180
    static const int MAX_SNAKE_LENGTH = GRID_WIDTH * GRID_HEIGHT;
    static const int MAX_ENEMIES = 8;
    static const int MAX_OBSTACLES = 15;
    static const int MAX_POWER_UPS = 3;
    static const int MAX_FOODS = 5;
    static const int MAX_PARTICLES = 500;
    static const int MAX_TRAIL = 200;
    
    SnakeNode* snake;
    int snakeLength;
    int direction;
    int nextDirection;
    
    Food* foods;
    int score;
    bool gameOver;
    bool gamePaused;
    float moveTimer;
    float moveDelay;
    bool shiftPressed;
    
    Enemy* enemies;
    int enemyCount;
    Obstacle* obstacles;
    int obstacleCount;
    PowerUp* powerUps;
    
    float invincibleTimer;
    float speedBoostTimer;
    float ghostTimer;
    
    Particle* particles;
    
    float* tailAlpha;
    sf::Color snakeColors[5];
    float gameTime;
    float backgroundPulse;
    
    SnakeSoundManager soundManager;
    
    struct TrailNode {
        float x, y;
        float life;
    };
    TrailNode* trail;
    int trailIndex;
    
    sf::Font gameFont;
    bool fontLoaded;

    Button returnButton;

    bool returnButtonInitialized;
    
    void initEnemies();
    void initObstacles();
    void initPowerUps();
    void spawnAllFoods();
    void spawnFood(int index);
    void spawnPowerUp(int index);
    void collectPowerUp(int index);
    void addEnemy();
    void createParticles(float x, float y, sf::Color color, int count);

public:
    Game5();
    ~Game5();
    
    void init();
    void handleInput(const sf::Event& event, sf::RenderWindow& window);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
    bool isGameRunning() const { return !gameOver && !gamePaused; }
    bool isGameOver() const { return gameOver; }
    int getScore() const { return score; }
    void reset();
};

#endif