#ifndef GAME1_H
#define GAME1_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "ResourceManager.h"
#include "Button.h"
#include "GameBase.h"
#include <vector>
#include <cmath>
#include <map>
#include <string>

// 战斗状态枚举
enum class FighterState {
    IDLE,
    WALK_FORWARD,
    WALK_BACKWARD,
    JUMP,
    CROUCH,
    LIGHT_PUNCH,
    HEAVY_PUNCH,
    LIGHT_KICK,
    HEAVY_KICK,
    SPECIAL_MOVE,
    BLOCK,
    HIT,
    KNOCKED_DOWN,
    GET_UP,
    WIN,
    LOSE
};

// 战斗角色结构
struct Fighter {
    float x, y;
    float vx, vy;
    float width, height;
    bool facingRight;
    
    FighterState state;
    float stateTimer;
    int animFrame;
    float animTimer;
    
    int health;
    int maxHealth;
    float energy; // 气槽
    float maxEnergy;
    
    bool isPlayer;
    bool onGround;
    bool isBlocking;
    bool isInvincible;
    float invincibleTimer;
    
    // 连击系统
    int comboCount;
    float comboTimer;
    
    // AI相关
    float aiTimer;
    float aiDecisionTimer;
    int aiState; // 0=防御, 1=进攻, 2=特殊
};

// 攻击判定框
struct Hitbox {
    float x, y, width, height;
    int damage;
    float knockback;
    bool active;
    float lifetime;
    bool hasHit;
    int ownerIndex; // 0=玩家, 1=AI
};

// 特效粒子
struct Game1Particle {
    float x, y;
    float vx, vy;
    float life;
    float maxLife;
    sf::Color color;
    bool active;
    float size;
    float rotation;
    float rotationSpeed;
    int type; // 0=普通, 1=火焰, 2=冲击波, 3=血液
};

// 背景元素
struct BackgroundElement {
    float x, y;
    float parallaxSpeed;
    int type;
    float animTime;
};

class Game1 : public GameBase {
private:
    // 游戏状态
    bool gameOver;
    bool gamePaused;
    bool roundOver;
    int score;
    float gameTime;
    float roundTime;
    
    // 回合系统
    int playerWins;
    int aiWins;
    int currentRound;
    float roundStartTimer;
    bool roundStarted;
    
    // 战斗角色
    Fighter player;
    Fighter ai;
    
    // 战斗系统
    std::vector<Hitbox> hitboxes;
    
    // 粒子系统
    static const int MAX_PARTICLES = 300;
    Game1Particle particles[MAX_PARTICLES];
    
    // 背景
    std::vector<BackgroundElement> backgroundElements;
    
    // 摄像机震动
    float cameraShakeX;
    float cameraShakeY;
    float cameraShakeIntensity;
    
    // UI
    sf::Font gameFont;
    bool fontLoaded;
    Button returnButton;
    bool returnButtonInitialized;
    
    // 音效生成器（程序化音效）
    std::vector<sf::Int16> generatePunchSound();
    std::vector<sf::Int16> generateKickSound();
    std::vector<sf::Int16> generateHitSound();
    std::vector<sf::Int16> generateSpecialSound();
    std::vector<sf::Int16> generateKOSound();
    std::vector<sf::Int16> generateBlockSound();
    
    sf::SoundBuffer punchSoundBuffer;
    sf::SoundBuffer kickSoundBuffer;
    sf::SoundBuffer hitSoundBuffer;
    sf::SoundBuffer specialSoundBuffer;
    sf::SoundBuffer koSoundBuffer;
    sf::SoundBuffer blockSoundBuffer;
    sf::Sound gameSound;
    
    // 游戏计时
    sf::Clock gameClock;
    
    // 初始化函数
    void initFighter(Fighter& fighter, bool isPlayer);
    void initBackground();
    void resetRound();
    
    // 更新函数
    void updatePlayer(float deltaTime);
    void updateAI(float deltaTime);
    void updateFighterState(Fighter& fighter, float deltaTime);
    void updateFighterAnimation(Fighter& fighter, float deltaTime);
    void updateHitboxes(float deltaTime);
    void updateParticles(float deltaTime);
    void updateBackground(float deltaTime);
    void updateCameraShake(float deltaTime);
    
    // AI系统
    void aiMakeDecision(float deltaTime);
    void aiExecuteAction(int action);
    float getDistanceToPlayer();
    
    // 战斗系统
    void performAttack(Fighter& attacker, FighterState attackType);
    void createHitbox(float x, float y, float width, float height, 
                     int damage, float knockback, int owner);
    void checkHitboxCollisions();
    void applyDamage(Fighter& target, int damage, float knockback, bool facingRight);
    void checkFighterCollision();
    
    // 输入处理
    void handlePlayerInput(float deltaTime);
    bool checkSpecialMoveInput();
    
    // 渲染函数
    void renderBackground(sf::RenderWindow& window);
    void renderStage(sf::RenderWindow& window);
    void renderFighter(sf::RenderWindow& window, const Fighter& fighter);
    void renderFighterDetails(sf::RenderWindow& window, const Fighter& fighter);
    void renderHitboxes(sf::RenderWindow& window);
    void renderParticles(sf::RenderWindow& window);
    void renderUI(sf::RenderWindow& window);
    void renderHealthBars(sf::RenderWindow& window);
    void renderEnergyBars(sf::RenderWindow& window);
    void renderRoundInfo(sf::RenderWindow& window);
    void renderComboCounter(sf::RenderWindow& window);
    void renderRoundStart(sf::RenderWindow& window);
    void renderRoundEnd(sf::RenderWindow& window);
    void renderGameOver(sf::RenderWindow& window);
    
    // 特效函数
    void createParticles(float x, float y, sf::Color color, int count, int type = 0);
    void createHitEffect(float x, float y, bool critical);
    void createSpecialEffect(float x, float y);
    void addCameraShake(float intensity);
    
    // 辅助函数
    void changeState(Fighter& fighter, FighterState newState);
    bool canAct(const Fighter& fighter);
    bool isAttacking(const Fighter& fighter);
    sf::Color getHealthColor(float healthPercent);

public:
    Game1();
    ~Game1();
    
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