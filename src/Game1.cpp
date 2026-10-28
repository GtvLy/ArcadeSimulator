#include "Game1.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Game1::Game1() {
    fontLoaded = false;
    returnButtonInitialized = false;
    cameraShakeX = 0;
    cameraShakeY = 0;
    cameraShakeIntensity = 0;
    
    std::cout << "Game1 (Street Fighter) created" << std::endl;
}

Game1::~Game1() {
    std::cout << "Game1 (Street Fighter) destroyed" << std::endl;
}

void Game1::init() {
    std::cout << "Game1 initializing..." << std::endl;
    
    fontLoaded = gameFont.loadFromFile("../assets/fonts/PixelifySans.ttf");
    if (!fontLoaded) {
        std::cerr << "Warning: Could not load font for Game1" << std::endl;
    }
    
    // 生成程序化音效
    punchSoundBuffer.loadFromSamples(generatePunchSound().data(), 
                                    generatePunchSound().size(), 1, 44100);
    kickSoundBuffer.loadFromSamples(generateKickSound().data(), 
                                   generateKickSound().size(), 1, 44100);
    hitSoundBuffer.loadFromSamples(generateHitSound().data(), 
                                  generateHitSound().size(), 1, 44100);
    specialSoundBuffer.loadFromSamples(generateSpecialSound().data(), 
                                      generateSpecialSound().size(), 1, 44100);
    koSoundBuffer.loadFromSamples(generateKOSound().data(), 
                                 generateKOSound().size(), 1, 44100);
    blockSoundBuffer.loadFromSamples(generateBlockSound().data(), 
                                    generateBlockSound().size(), 1, 44100);
    
    reset();
    std::cout << "Game1 initialized!" << std::endl;
}

void Game1::reset() {
    std::cout << "Game1 reset" << std::endl;
    
    gameOver = false;
    gamePaused = false;
    roundOver = false;
    score = 0;
    gameTime = 0;
    roundTime = 99.0f;
    
    playerWins = 0;
    aiWins = 0;
    currentRound = 1;
    roundStartTimer = 3.0f;
    roundStarted = false;
    
    cameraShakeX = 0;
    cameraShakeY = 0;
    cameraShakeIntensity = 0;
    
    // 初始化角色
    initFighter(player, true);
    initFighter(ai, false);
    
    // 初始化背景
    initBackground();
    
    // 清空容器
    hitboxes.clear();
    
    // 初始化粒子
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = false;
    }
    
    gameClock.restart();
}

void Game1::initFighter(Fighter& fighter, bool isPlayer) {
    if (isPlayer) {
        fighter.x = 80;
        fighter.facingRight = true;
    } else {
        fighter.x = 240;
        fighter.facingRight = false;
    }
    
    fighter.y = 120;
    fighter.vx = 0;
    fighter.vy = 0;
    fighter.width = 24;
    fighter.height = 40;
    
    fighter.state = FighterState::IDLE;
    fighter.stateTimer = 0;
    fighter.animFrame = 0;
    fighter.animTimer = 0;
    
    fighter.health = 300;
    fighter.maxHealth = 300;
    fighter.energy = 0;
    fighter.maxEnergy = 100;
    
    fighter.isPlayer = isPlayer;
    fighter.onGround = true;
    fighter.isBlocking = false;
    fighter.isInvincible = false;
    fighter.invincibleTimer = 0;
    
    fighter.comboCount = 0;
    fighter.comboTimer = 0;
    
    fighter.aiTimer = 0;
    fighter.aiDecisionTimer = 0;
    fighter.aiState = 0;
}

void Game1::resetRound() {
    roundOver = false;
    roundTime = 99.0f;
    roundStartTimer = 3.0f;
    roundStarted = false;
    
    initFighter(player, true);
    initFighter(ai, false);
    
    hitboxes.clear();
}

void Game1::initBackground() {
    backgroundElements.clear();
    
    // 远景建筑
    for (int i = 0; i < 5; i++) {
        BackgroundElement elem;
        elem.x = i * 80;
        elem.y = 20 + (i % 3) * 10;
        elem.parallaxSpeed = 0.2f;
        elem.type = 0; // 建筑
        elem.animTime = 0;
        backgroundElements.push_back(elem);
    }
    
    // 中景观众
    for (int i = 0; i < 8; i++) {
        BackgroundElement elem;
        elem.x = i * 50 + 20;
        elem.y = 80;
        elem.parallaxSpeed = 0.5f;
        elem.type = 1; // 观众
        elem.animTime = i * 0.5f;
        backgroundElements.push_back(elem);
    }
}

// 程序化音效生成
std::vector<sf::Int16> Game1::generatePunchSound() {
    std::vector<sf::Int16> samples;
    const int sampleRate = 44100;
    const float duration = 0.08f;
    
    for (int i = 0; i < static_cast<int>(sampleRate * duration); i++) {
        float t = static_cast<float>(i) / sampleRate;
        float noise = (rand() % 1000 - 500) / 500.0f;
        float punch = std::sin(2 * 3.14159f * (300 - t * 2000) * t);
        float env = (1.0f - t / duration);
        float sound = (punch * 0.6f + noise * 0.4f) * env * 15000;
        samples.push_back(static_cast<sf::Int16>(sound));
    }
    
    return samples;
}

std::vector<sf::Int16> Game1::generateKickSound() {
    std::vector<sf::Int16> samples;
    const int sampleRate = 44100;
    const float duration = 0.12f;
    
    for (int i = 0; i < static_cast<int>(sampleRate * duration); i++) {
        float t = static_cast<float>(i) / sampleRate;
        float noise = (rand() % 1000 - 500) / 500.0f;
        float kick = std::sin(2 * 3.14159f * (200 - t * 1500) * t);
        float env = (1.0f - t / duration);
        float sound = (kick * 0.5f + noise * 0.5f) * env * 18000;
        samples.push_back(static_cast<sf::Int16>(sound));
    }
    
    return samples;
}

std::vector<sf::Int16> Game1::generateHitSound() {
    std::vector<sf::Int16> samples;
    const int sampleRate = 44100;
    const float duration = 0.15f;
    
    for (int i = 0; i < static_cast<int>(sampleRate * duration); i++) {
        float t = static_cast<float>(i) / sampleRate;
        float noise = (rand() % 1000 - 500) / 500.0f;
        float thud = std::sin(2 * 3.14159f * (150 - t * 1000) * t);
        float env = (1.0f - t / duration);
        float sound = (thud * 0.4f + noise * 0.6f) * env * 20000;
        samples.push_back(static_cast<sf::Int16>(sound));
    }
    
    return samples;
}

std::vector<sf::Int16> Game1::generateSpecialSound() {
    std::vector<sf::Int16> samples;
    const int sampleRate = 44100;
    const float duration = 0.4f;
    
    for (int i = 0; i < static_cast<int>(sampleRate * duration); i++) {
        float t = static_cast<float>(i) / sampleRate;
        float freq = 400 + std::sin(t * 30) * 200;
        float special = std::sin(2 * 3.14159f * freq * t);
        float env = std::sin(3.14159f * t / duration);
        float sound = special * env * 12000;
        samples.push_back(static_cast<sf::Int16>(sound));
    }
    
    return samples;
}

std::vector<sf::Int16> Game1::generateKOSound() {
    std::vector<sf::Int16> samples;
    const int sampleRate = 44100;
    const float duration = 0.6f;
    
    for (int i = 0; i < static_cast<int>(sampleRate * duration); i++) {
        float t = static_cast<float>(i) / sampleRate;
        float freq = 600 - t * 500;
        float ko = std::sin(2 * 3.14159f * freq * t);
        float env = (1.0f - t / duration);
        float sound = ko * env * 18000;
        samples.push_back(static_cast<sf::Int16>(sound));
    }
    
    return samples;
}

std::vector<sf::Int16> Game1::generateBlockSound() {
    std::vector<sf::Int16> samples;
    const int sampleRate = 44100;
    const float duration = 0.06f;
    
    for (int i = 0; i < static_cast<int>(sampleRate * duration); i++) {
        float t = static_cast<float>(i) / sampleRate;
        float noise = (rand() % 1000 - 500) / 500.0f;
        float block = std::sin(2 * 3.14159f * 800 * t);
        float env = (1.0f - t / duration);
        float sound = (block * 0.3f + noise * 0.7f) * env * 10000;
        samples.push_back(static_cast<sf::Int16>(sound));
    }
    
    return samples;
}

void Game1::handleInput(const sf::Event& event, sf::RenderWindow& window) {
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
    
    // 处理返回按钮
    if (!gameOver && returnButtonInitialized) {
        returnButton.update(worldPos);
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                returnButton.handleMousePressed(worldPos);
                return;
            }
        }
        
        if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                returnButton.handleMouseReleased(worldPos);
                return;
            }
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Q) {
                returnButton.handleKeyPressed(event.key.code);
                return;
            }
        }
    }
    
    // ESC键退出
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            gamePaused = true;
            return;
        }
        
        // 游戏结束后重新开始
        if (gameOver && event.key.code == sf::Keyboard::Space) {
            reset();
        }
    }
}

void Game1::handlePlayerInput(float deltaTime) {
    if (!canAct(player) || !roundStarted) return;
    
    // 防御
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        if (player.onGround && player.state == FighterState::IDLE) {
            changeState(player, FighterState::CROUCH);
        }
        player.isBlocking = true;
    } else {
        player.isBlocking = false;
    }
    
    // 移动
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        if (player.onGround && canAct(player)) {
            changeState(player, FighterState::WALK_BACKWARD);
            player.vx = -80.0f;
        }
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || 
               sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        if (player.onGround && canAct(player)) {
            changeState(player, FighterState::WALK_FORWARD);
            player.vx = 80.0f;
        }
    } else {
        if (player.state == FighterState::WALK_FORWARD || 
            player.state == FighterState::WALK_BACKWARD) {
            changeState(player, FighterState::IDLE);
        }
        player.vx = 0;
    }
    
    // 跳跃
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || 
         sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && 
        player.onGround && canAct(player)) {
        changeState(player, FighterState::JUMP);
        player.vy = -280.0f;
        player.onGround = false;
    }
    
    // 轻拳 (J键)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::J) && canAct(player)) {
        performAttack(player, FighterState::LIGHT_PUNCH);
    }
    
    // 重拳 (K键)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::K) && canAct(player)) {
        performAttack(player, FighterState::HEAVY_PUNCH);
    }
    
    // 轻腿 (U键)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::U) && canAct(player)) {
        performAttack(player, FighterState::LIGHT_KICK);
    }
    
    // 重腿 (I键)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I) && canAct(player)) {
        performAttack(player, FighterState::HEAVY_KICK);
    }
    
    // 必杀技 (L键 + 能量充足)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L) && 
        player.energy >= 50 && canAct(player)) {
        performAttack(player, FighterState::SPECIAL_MOVE);
        player.energy -= 50;
    }
}

void Game1::update(float deltaTime) {
    if (gameOver || gamePaused) return;
    
    // 限制deltaTime
    if (deltaTime > 0.05f) deltaTime = 0.05f;
    
    gameTime += deltaTime;
    
    // 回合开始倒计时
    if (!roundStarted) {
        roundStartTimer -= deltaTime;
        if (roundStartTimer <= 0) {
            roundStarted = true;
        }
        return;
    }
    
    // 回合时间
    if (!roundOver) {
        roundTime -= deltaTime;
        if (roundTime <= 0) {
            roundTime = 0;
            roundOver = true;
            
            // 判定胜负
            if (player.health > ai.health) {
                playerWins++;
                changeState(player, FighterState::WIN);
                changeState(ai, FighterState::LOSE);
            } else {
                aiWins++;
                changeState(player, FighterState::LOSE);
                changeState(ai, FighterState::WIN);
            }
        }
    }
    
    // 更新角色
    handlePlayerInput(deltaTime);
    updateAI(deltaTime);
    updatePlayer(deltaTime);
    updateFighterState(player, deltaTime);
    updateFighterState(ai, deltaTime);
    updateFighterAnimation(player, deltaTime);
    updateFighterAnimation(ai, deltaTime);
    
    // 更新战斗系统
    updateHitboxes(deltaTime);
    checkHitboxCollisions();
    checkFighterCollision();
    
    // 更新特效
    updateParticles(deltaTime);
    updateBackground(deltaTime);
    updateCameraShake(deltaTime);
    
    // 检查KO
    if (player.health <= 0 || ai.health <= 0) {
        if (!roundOver) {
            roundOver = true;
            
            if (player.health <= 0) {
                aiWins++;
                changeState(player, FighterState::KNOCKED_DOWN);
                changeState(ai, FighterState::WIN);
                gameSound.setBuffer(koSoundBuffer);
                gameSound.play();
                createHitEffect(player.x + player.width/2, player.y + player.height/2, true);
                addCameraShake(15);
            } else {
                playerWins++;
                changeState(ai, FighterState::KNOCKED_DOWN);
                changeState(player, FighterState::WIN);
                gameSound.setBuffer(koSoundBuffer);
                gameSound.play();
                createHitEffect(ai.x + ai.width/2, ai.y + ai.height/2, true);
                addCameraShake(15);
            }
            
            score += 1000;
        }
    }
    
    // 检查游戏结束
    if (roundOver) {
        if (playerWins >= 2) {
            gameOver = true;
            score += 5000;
        } else if (aiWins >= 2) {
            gameOver = true;
        } else {
            // 等待一段时间后开始新回合
            static float waitTimer = 0;
            waitTimer += deltaTime;
            if (waitTimer > 3.0f) {
                waitTimer = 0;
                currentRound++;
                resetRound();
            }
        }
    }
}

void Game1::updatePlayer(float deltaTime) {
    // 重力
    if (!player.onGround) {
        player.vy += 800.0f * deltaTime;
        if (player.vy > 400.0f) player.vy = 400.0f;
    }
    
    // 移动
    player.x += player.vx * deltaTime;
    player.y += player.vy * deltaTime;
    
    // 限制在场地内
    if (player.x < 10) player.x = 10;
    if (player.x > 286) player.x = 286;
    
    // 地面检测
    if (player.y >= 120) {
        player.y = 120;
        player.vy = 0;
        if (!player.onGround) {
            player.onGround = true;
            if (player.state == FighterState::JUMP) {
                changeState(player, FighterState::IDLE);
            }
        }
    } else {
        player.onGround = false;
    }
    
    // 无敌时间
    if (player.isInvincible) {
        player.invincibleTimer -= deltaTime;
        if (player.invincibleTimer <= 0) {
            player.isInvincible = false;
        }
    }
    
    // 连击计时
    if (player.comboCount > 0) {
        player.comboTimer -= deltaTime;
        if (player.comboTimer <= 0) {
            player.comboCount = 0;
        }
    }
    
    // 能量恢复
    if (player.energy < player.maxEnergy) {
        player.energy += 5.0f * deltaTime;
        if (player.energy > player.maxEnergy) {
            player.energy = player.maxEnergy;
        }
    }
    
    // 面向对手
    if (canAct(player)) {
        player.facingRight = (ai.x > player.x);
    }
}

void Game1::updateAI(float deltaTime) {
    ai.aiTimer += deltaTime;
    ai.aiDecisionTimer += deltaTime;
    
    // 每0.3秒做一次决策
    if (ai.aiDecisionTimer > 0.3f && roundStarted && !roundOver) {
        ai.aiDecisionTimer = 0;
        aiMakeDecision(deltaTime);
    }
    
    // 重力
    if (!ai.onGround) {
        ai.vy += 800.0f * deltaTime;
        if (ai.vy > 400.0f) ai.vy = 400.0f;
    }
    
    // 移动
    ai.x += ai.vx * deltaTime;
    ai.y += ai.vy * deltaTime;
    
    // 限制在场地内
    if (ai.x < 10) ai.x = 10;
    if (ai.x > 286) ai.x = 286;
    
    // 地面检测
    if (ai.y >= 120) {
        ai.y = 120;
        ai.vy = 0;
        if (!ai.onGround) {
            ai.onGround = true;
            if (ai.state == FighterState::JUMP) {
                changeState(ai, FighterState::IDLE);
            }
        }
    } else {
        ai.onGround = false;
    }
    
    // 无敌时间
    if (ai.isInvincible) {
        ai.invincibleTimer -= deltaTime;
        if (ai.invincibleTimer <= 0) {
            ai.isInvincible = false;
        }
    }
    
    // 连击计时
    if (ai.comboCount > 0) {
        ai.comboTimer -= deltaTime;
        if (ai.comboTimer <= 0) {
            ai.comboCount = 0;
        }
    }
    
    // 能量恢复
    if (ai.energy < ai.maxEnergy) {
        ai.energy += 5.0f * deltaTime;
        if (ai.energy > ai.maxEnergy) {
            ai.energy = ai.maxEnergy;
        }
    }
    
    // 面向对手
    if (canAct(ai)) {
        ai.facingRight = (player.x > ai.x);
    }
}

void Game1::aiMakeDecision(float deltaTime) {
    if (!canAct(ai)) return;
    
    float distance = getDistanceToPlayer();
    
    // 健康值低时更保守
    float healthPercent = static_cast<float>(ai.health) / ai.maxHealth;
    
    // 根据距离和血量决定策略
    if (healthPercent < 0.3f) {
        // 血量低，防守反击
        if (distance < 40) {
            // 太近了，后退
            ai.vx = ai.facingRight ? -60.0f : 60.0f;
            changeState(ai, FighterState::WALK_BACKWARD);
        } else if (distance < 60 && rand() % 100 < 30) {
            // 伺机反击
            aiExecuteAction(rand() % 4);
        } else {
            ai.vx = 0;
            ai.isBlocking = (rand() % 100 < 40);
        }
    } else {
        // 血量健康，进攻
        if (distance > 80) {
            // 太远，靠近
            ai.vx = ai.facingRight ? 60.0f : -60.0f;
            changeState(ai, FighterState::WALK_FORWARD);
        } else if (distance < 30) {
            // 近距离，高概率攻击
            if (rand() % 100 < 70) {
                aiExecuteAction(rand() % 5);
            }
        } else if (distance < 60) {
            // 中距离，适中攻击
            if (rand() % 100 < 50) {
                aiExecuteAction(rand() % 5);
            }
        } else {
            // 偶尔跳跃接近
            if (rand() % 100 < 20 && ai.onGround) {
                changeState(ai, FighterState::JUMP);
                ai.vy = -280.0f;
                ai.onGround = false;
            }
        }
    }
    
    // 能量充足时使用必杀技
    if (ai.energy >= 50 && distance < 80 && rand() % 100 < 25) {
        performAttack(ai, FighterState::SPECIAL_MOVE);
        ai.energy -= 50;
    }
}

void Game1::aiExecuteAction(int action) {
    switch (action) {
        case 0:
            performAttack(ai, FighterState::LIGHT_PUNCH);
            break;
        case 1:
            performAttack(ai, FighterState::HEAVY_PUNCH);
            break;
        case 2:
            performAttack(ai, FighterState::LIGHT_KICK);
            break;
        case 3:
            performAttack(ai, FighterState::HEAVY_KICK);
            break;
        case 4:
            if (ai.energy >= 50) {
                performAttack(ai, FighterState::SPECIAL_MOVE);
                ai.energy -= 50;
            }
            break;
    }
}

float Game1::getDistanceToPlayer() {
    return std::abs(ai.x - player.x);
}

void Game1::updateFighterState(Fighter& fighter, float deltaTime) {
    fighter.stateTimer += deltaTime;
    
    switch (fighter.state) {
        case FighterState::IDLE:
            // 待机状态
            break;
            
        case FighterState::WALK_FORWARD:
        case FighterState::WALK_BACKWARD:
            // 行走状态
            break;
            
        case FighterState::JUMP:
            // 跳跃状态
            break;
            
        case FighterState::CROUCH:
            // 蹲下状态
            break;
            
        case FighterState::LIGHT_PUNCH:
            if (fighter.stateTimer > 0.2f) {
                changeState(fighter, FighterState::IDLE);
            }
            break;
            
        case FighterState::HEAVY_PUNCH:
            if (fighter.stateTimer > 0.4f) {
                changeState(fighter, FighterState::IDLE);
            }
            break;
            
        case FighterState::LIGHT_KICK:
            if (fighter.stateTimer > 0.25f) {
                changeState(fighter, FighterState::IDLE);
            }
            break;
            
        case FighterState::HEAVY_KICK:
            if (fighter.stateTimer > 0.45f) {
                changeState(fighter, FighterState::IDLE);
            }
            break;
            
        case FighterState::SPECIAL_MOVE:
            if (fighter.stateTimer > 0.6f) {
                changeState(fighter, FighterState::IDLE);
            }
            break;
            
        case FighterState::HIT:
            if (fighter.stateTimer > 0.3f) {
                changeState(fighter, FighterState::IDLE);
            }
            break;
            
        case FighterState::KNOCKED_DOWN:
            // KO状态
            break;
            
        case FighterState::WIN:
        case FighterState::LOSE:
            // 胜利/失败状态
            break;
            
        default:
            break;
    }
}

void Game1::updateFighterAnimation(Fighter& fighter, float deltaTime) {
    fighter.animTimer += deltaTime;
    
    // 根据状态设置动画速度
    float animSpeed = 0.1f;
    
    if (fighter.state == FighterState::WALK_FORWARD || 
        fighter.state == FighterState::WALK_BACKWARD) {
        animSpeed = 0.08f;
    } else if (isAttacking(fighter)) {
        animSpeed = 0.05f;
    }
    
    if (fighter.animTimer > animSpeed) {
        fighter.animTimer = 0;
        fighter.animFrame = (fighter.animFrame + 1) % 8;
    }
}

void Game1::updateHitboxes(float deltaTime) {
    for (auto it = hitboxes.begin(); it != hitboxes.end();) {
        it->lifetime -= deltaTime;
        
        if (it->lifetime <= 0 || it->hasHit) {
            it = hitboxes.erase(it);
        } else {
            ++it;
        }
    }
}

void Game1::updateParticles(float deltaTime) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].vx *= 0.98f;
            particles[i].vy += 400.0f * deltaTime;
            particles[i].x += particles[i].vx * deltaTime;
            particles[i].y += particles[i].vy * deltaTime;
            particles[i].life -= deltaTime;
            particles[i].rotation += particles[i].rotationSpeed * deltaTime;
            
            if (particles[i].life <= 0) {
                particles[i].active = false;
            }
        }
    }
}

void Game1::updateBackground(float deltaTime) {
    for (auto& elem : backgroundElements) {
        elem.animTime += deltaTime;
    }
}

void Game1::updateCameraShake(float deltaTime) {
    if (cameraShakeIntensity > 0) {
        cameraShakeIntensity -= deltaTime * 30.0f;
        if (cameraShakeIntensity < 0) cameraShakeIntensity = 0;
        
        cameraShakeX = (rand() % 100 - 50) / 50.0f * cameraShakeIntensity;
        cameraShakeY = (rand() % 100 - 50) / 50.0f * cameraShakeIntensity;
    } else {
        cameraShakeX = 0;
        cameraShakeY = 0;
    }
}

void Game1::performAttack(Fighter& attacker, FighterState attackType) {
    if (!canAct(attacker)) return;
    
    changeState(attacker, attackType);
    
    float hitboxX = attacker.x + (attacker.facingRight ? attacker.width : -20);
    float hitboxY = attacker.y + attacker.height / 2;
    float hitboxWidth = 20;
    float hitboxHeight = 15;
    int damage = 0;
    float knockback = 0;
    
    switch (attackType) {
        case FighterState::LIGHT_PUNCH:
            damage = 5;
            knockback = 20.0f;
            hitboxWidth = 18;
            hitboxHeight = 12;
            gameSound.setBuffer(punchSoundBuffer);
            gameSound.play();
            break;
            
        case FighterState::HEAVY_PUNCH:
            damage = 12;
            knockback = 40.0f;
            hitboxWidth = 25;
            hitboxHeight = 15;
            hitboxY -= 5;
            gameSound.setBuffer(punchSoundBuffer);
            gameSound.play();
            break;
            
        case FighterState::LIGHT_KICK:
            damage = 7;
            knockback = 25.0f;
            hitboxWidth = 22;
            hitboxHeight = 14;
            hitboxY += 10;
            gameSound.setBuffer(kickSoundBuffer);
            gameSound.play();
            break;
            
        case FighterState::HEAVY_KICK:
            damage = 15;
            knockback = 50.0f;
            hitboxWidth = 30;
            hitboxHeight = 18;
            hitboxY += 5;
            gameSound.setBuffer(kickSoundBuffer);
            gameSound.play();
            break;
            
        case FighterState::SPECIAL_MOVE:
            damage = 25;
            knockback = 80.0f;
            hitboxWidth = 50;
            hitboxHeight = 30;
            hitboxY -= 10;
            gameSound.setBuffer(specialSoundBuffer);
            gameSound.play();
            createSpecialEffect(hitboxX + hitboxWidth/2, hitboxY + hitboxHeight/2);
            addCameraShake(8);
            break;
            
        default:
            return;
    }
    
    // 创建攻击判定框（延迟生成，模拟出手时间）
    int ownerIndex = attacker.isPlayer ? 0 : 1;
    
    // 根据攻击类型设置不同的生命周期
    float lifetime = 0.05f;
    if (attackType == FighterState::SPECIAL_MOVE) {
        lifetime = 0.3f;
    } else if (attackType == FighterState::HEAVY_PUNCH || 
               attackType == FighterState::HEAVY_KICK) {
        lifetime = 0.1f;
    }
    
    createHitbox(hitboxX, hitboxY, hitboxWidth, hitboxHeight, 
                damage, knockback, ownerIndex);
}

void Game1::createHitbox(float x, float y, float width, float height, 
                        int damage, float knockback, int owner) {
    Hitbox hitbox;
    hitbox.x = x;
    hitbox.y = y;
    hitbox.width = width;
    hitbox.height = height;
    hitbox.damage = damage;
    hitbox.knockback = knockback;
    hitbox.active = true;
    hitbox.lifetime = 0.1f;
    hitbox.hasHit = false;
    hitbox.ownerIndex = owner;
    
    hitboxes.push_back(hitbox);
}

void Game1::checkHitboxCollisions() {
    for (auto& hitbox : hitboxes) {
        if (!hitbox.active || hitbox.hasHit) continue;
        
        // 检查玩家被击中
        if (hitbox.ownerIndex == 1) { // AI的攻击
            if (player.x < hitbox.x + hitbox.width &&
                player.x + player.width > hitbox.x &&
                player.y < hitbox.y + hitbox.height &&
                player.y + player.height > hitbox.y) {
                
                if (player.isBlocking && player.onGround) {
                    // 格挡成功
                    gameSound.setBuffer(blockSoundBuffer);
                    gameSound.play();
                    createParticles(hitbox.x, hitbox.y + hitbox.height/2, 
                                  sf::Color(255, 255, 100), 10, 2);
                    addCameraShake(2);
                    hitbox.hasHit = true;
                } else if (!player.isInvincible) {
                    // 受到伤害
                    applyDamage(player, hitbox.damage, hitbox.knockback, !ai.facingRight);
                    hitbox.hasHit = true;
                }
            }
        }
        // 检查AI被击中
        else if (hitbox.ownerIndex == 0) { // 玩家的攻击
            if (ai.x < hitbox.x + hitbox.width &&
                ai.x + ai.width > hitbox.x &&
                ai.y < hitbox.y + hitbox.height &&
                ai.y + ai.height > hitbox.y) {
                
                if (ai.isBlocking && ai.onGround) {
                    // AI格挡成功
                    gameSound.setBuffer(blockSoundBuffer);
                    gameSound.play();
                    createParticles(hitbox.x, hitbox.y + hitbox.height/2, 
                                  sf::Color(255, 255, 100), 10, 2);
                    addCameraShake(2);
                    hitbox.hasHit = true;
                } else if (!ai.isInvincible) {
                    // AI受到伤害
                    applyDamage(ai, hitbox.damage, hitbox.knockback, !player.facingRight);
                    hitbox.hasHit = true;
                }
            }
        }
    }
}

void Game1::applyDamage(Fighter& target, int damage, float knockback, bool knockbackRight) {
    // 应用伤害
    target.health -= damage;
    if (target.health < 0) target.health = 0;
    
    // 改变状态
    if (target.health > 0) {
        changeState(target, FighterState::HIT);
    }
    
    // 击退
    target.vx = knockbackRight ? knockback : -knockback;
    target.vy = -knockback * 0.5f;
    target.onGround = false;
    
    // 无敌帧
    target.isInvincible = true;
    target.invincibleTimer = 0.3f;
    
    // 连击系统
    Fighter& attacker = target.isPlayer ? ai : player;
    attacker.comboCount++;
    attacker.comboTimer = 2.0f;
    
    // 能量增加
    attacker.energy += damage * 0.5f;
    if (attacker.energy > attacker.maxEnergy) {
        attacker.energy = attacker.maxEnergy;
    }
    
    // 音效和特效
    gameSound.setBuffer(hitSoundBuffer);
    gameSound.play();
    
    bool isCritical = (damage >= 15);
    createHitEffect(target.x + target.width/2, target.y + target.height/2, isCritical);
    
    // 摄像机震动
    float shakeAmount = damage / 5.0f;
    if (isCritical) shakeAmount *= 2;
    addCameraShake(shakeAmount);
    
    // 分数
    if (target.isPlayer == false) { // 玩家击中AI
        score += damage * 10;
    }
}

void Game1::checkFighterCollision() {
    // 防止角色重叠
    if (player.x < ai.x + ai.width &&
        player.x + player.width > ai.x &&
        player.y < ai.y + ai.height &&
        player.y + player.height > ai.y) {
        
        float overlapX = (player.x + player.width) - ai.x;
        if (player.x + player.width/2 < ai.x + ai.width/2) {
            player.x -= overlapX / 2;
            ai.x += overlapX / 2;
        } else {
            player.x += overlapX / 2;
            ai.x -= overlapX / 2;
        }
    }
}

void Game1::changeState(Fighter& fighter, FighterState newState) {
    fighter.state = newState;
    fighter.stateTimer = 0;
    fighter.animFrame = 0;
    fighter.animTimer = 0;
}

bool Game1::canAct(const Fighter& fighter) {
    return fighter.state == FighterState::IDLE ||
           fighter.state == FighterState::WALK_FORWARD ||
           fighter.state == FighterState::WALK_BACKWARD ||
           fighter.state == FighterState::CROUCH ||
           fighter.state == FighterState::JUMP;
}

bool Game1::isAttacking(const Fighter& fighter) {
    return fighter.state == FighterState::LIGHT_PUNCH ||
           fighter.state == FighterState::HEAVY_PUNCH ||
           fighter.state == FighterState::LIGHT_KICK ||
           fighter.state == FighterState::HEAVY_KICK ||
           fighter.state == FighterState::SPECIAL_MOVE;
}

void Game1::createParticles(float x, float y, sf::Color color, int count, int type) {
    for (int i = 0; i < MAX_PARTICLES && count > 0; i++) {
        if (!particles[i].active) {
            particles[i].x = x;
            particles[i].y = y;
            particles[i].vx = (rand() % 200 - 100);
            particles[i].vy = -(rand() % 150 + 50);
            particles[i].life = 0.5f + (rand() % 100) / 200.0f;
            particles[i].maxLife = particles[i].life;
            particles[i].color = color;
            particles[i].active = true;
            particles[i].size = 2 + rand() % 4;
            particles[i].rotation = 0;
            particles[i].rotationSpeed = (rand() % 200 - 100) * 3.14159f;
            particles[i].type = type;
            count--;
        }
    }
}

void Game1::createHitEffect(float x, float y, bool critical) {
    int particleCount = critical ? 30 : 15;
    sf::Color hitColor = critical ? sf::Color(255, 100, 0) : sf::Color(255, 200, 50);
    
    createParticles(x, y, hitColor, particleCount, 1);
    
    // 血液粒子
    createParticles(x, y, sf::Color(200, 0, 0), 8, 3);
    
    // 冲击波粒子
    if (critical) {
        createParticles(x, y, sf::Color(255, 255, 255, 200), 10, 2);
    }
}

void Game1::createSpecialEffect(float x, float y) {
    // 特效粒子（火焰）
    createParticles(x, y, sf::Color(255, 150, 0), 40, 1);
    createParticles(x, y, sf::Color(255, 255, 100), 20, 1);
    createParticles(x, y, sf::Color(255, 255, 255), 15, 2);
}

void Game1::addCameraShake(float intensity) {
    cameraShakeIntensity += intensity;
    if (cameraShakeIntensity > 20) cameraShakeIntensity = 20;
}

sf::Color Game1::getHealthColor(float healthPercent) {
    if (healthPercent > 0.6f) {
        return sf::Color(50, 255, 50); // 绿色
    } else if (healthPercent > 0.3f) {
        return sf::Color(255, 200, 0); // 黄色
    } else {
        return sf::Color(255, 50, 50); // 红色
    }
}

void Game1::render(sf::RenderWindow& window) {
    // 应用摄像机震动
    sf::View gameView(sf::FloatRect(cameraShakeX, cameraShakeY, 320, 180));
    window.setView(gameView);
    
    // 延迟初始化返回按钮
    if (!returnButtonInitialized) {
        try {
            returnButton.setTextures(
                "../assets/images/return_off.png",
                "../assets/images/return_on.png",
                "../assets/images/return_click.png"
            );
            returnButton.setPosition(sf::Vector2f(5, 5));
            returnButton.setClickSound("../assets/audio/button.ogg");
            returnButton.setCallback([this]() {
                gamePaused = true;
            });
            returnButtonInitialized = true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to init return button: " << e.what() << std::endl;
        }
    }
    
    renderBackground(window);
    renderStage(window);
    renderParticles(window);
    renderFighter(window, ai);
    renderFighter(window, player);
    
    // 调试：显示碰撞框（可选）
    // renderHitboxes(window);
    
    // UI使用固定视图（不受震动影响）
    sf::View uiView(sf::FloatRect(0, 0, 320, 180));
    window.setView(uiView);
    
    renderUI(window);
    renderHealthBars(window);
    renderEnergyBars(window);
    renderRoundInfo(window);
    
    if (player.comboCount > 1) {
        renderComboCounter(window);
    }
    
    if (!roundStarted) {
        renderRoundStart(window);
    }
    
    if (roundOver) {
        renderRoundEnd(window);
    }
    
    if (gameOver) {
        renderGameOver(window);
    }
    
    // 返回按钮
    if (!gameOver && returnButtonInitialized) {
        returnButton.render(window);
    }
}

void Game1::renderBackground(sf::RenderWindow& window) {
    // 天空渐变（黄昏效果）
    for (int i = 0; i < 90; i++) {
        float ratio = i / 90.0f;
        sf::Color skyColor(
            255 - static_cast<int>(100 * ratio),
            120 + static_cast<int>(80 * ratio),
            50 + static_cast<int>(150 * ratio)
        );
        sf::RectangleShape line(sf::Vector2f(320, 1));
        line.setPosition(0, i);
        line.setFillColor(skyColor);
        window.draw(line);
    }
    
    // 远景建筑物（剪影）
    for (const auto& elem : backgroundElements) {
        if (elem.type == 0) { // 建筑
            // 随机高度的建筑物
            float buildingHeight = 30 + (static_cast<int>(elem.x) % 40);
            sf::RectangleShape building(sf::Vector2f(40, buildingHeight));
            building.setPosition(elem.x, 90 - buildingHeight);
            building.setFillColor(sf::Color(40, 20, 60, 150));
            window.draw(building);
            
            // 窗户
            for (int row = 0; row < buildingHeight / 8; row++) {
                for (int col = 0; col < 3; col++) {
                    if (rand() % 3 > 0) {
                        sf::RectangleShape windowRect(sf::Vector2f(4, 4));
                        windowRect.setPosition(elem.x + 5 + col * 12, 
                                             90 - buildingHeight + 5 + row * 8);
                        windowRect.setFillColor(sf::Color(255, 255, 150, 100));
                        window.draw(windowRect);
                    }
                }
            }
        }
    }
    
    // 中景观众（简化人形）
    for (const auto& elem : backgroundElements) {
        if (elem.type == 1) { // 观众
            // 观众头部
            sf::CircleShape head(4);
            head.setPosition(elem.x, elem.y);
            
            // 随机肤色
            int colorVar = static_cast<int>(elem.x) % 3;
            sf::Color skinColor;
            if (colorVar == 0) skinColor = sf::Color(255, 220, 180);
            else if (colorVar == 1) skinColor = sf::Color(200, 160, 120);
            else skinColor = sf::Color(150, 100, 70);
            
            head.setFillColor(skinColor);
            window.draw(head);
            
            // 观众身体
            sf::RectangleShape body(sf::Vector2f(8, 12));
            body.setPosition(elem.x, elem.y + 8);
            
            // 随机衣服颜色
            sf::Color shirtColors[] = {
                sf::Color(255, 50, 50),
                sf::Color(50, 50, 255),
                sf::Color(50, 200, 50),
                sf::Color(200, 200, 50),
                sf::Color(150, 50, 150)
            };
            body.setFillColor(shirtColors[static_cast<int>(elem.x) % 5]);
            window.draw(body);
            
            // 手臂动画（欢呼）
            float armWave = std::sin(elem.animTime * 3 + elem.x) * 0.5f + 0.5f;
            sf::RectangleShape arm(sf::Vector2f(3, 6));
            arm.setPosition(elem.x - 2, elem.y + 10);
            arm.setRotation(-30 * armWave);
            arm.setFillColor(skinColor);
            window.draw(arm);
        }
    }
}

void Game1::renderStage(sf::RenderWindow& window) {
    // 舞台地板（木质纹理效果）
    for (int i = 0; i < 20; i++) {
        sf::RectangleShape plank(sf::Vector2f(320, 6));
        plank.setPosition(0, 160 + i * 6);
        
        // 交替颜色模拟木板
        if (i % 2 == 0) {
            plank.setFillColor(sf::Color(139, 90, 43));
        } else {
            plank.setFillColor(sf::Color(160, 110, 60));
        }
        window.draw(plank);
        
        // 木板纹理线
        sf::RectangleShape grain(sf::Vector2f(320, 1));
        grain.setPosition(0, 160 + i * 6 + 3);
        grain.setFillColor(sf::Color(100, 60, 30, 100));
        window.draw(grain);
    }
    
    // 舞台边缘高光
    sf::RectangleShape stageHighlight(sf::Vector2f(320, 3));
    stageHighlight.setPosition(0, 157);
    stageHighlight.setFillColor(sf::Color(200, 150, 100, 150));
    window.draw(stageHighlight);
    
    // 舞台阴影线
    sf::RectangleShape stageShadow(sf::Vector2f(320, 2));
    stageShadow.setPosition(0, 160);
    stageShadow.setFillColor(sf::Color(50, 30, 20));
    window.draw(stageShadow);
    
    // 擂台绳索（两侧）
    for (int i = 0; i < 3; i++) {
        // 左侧绳索
        sf::RectangleShape ropeL(sf::Vector2f(30, 2));
        ropeL.setPosition(0, 100 + i * 15);
        ropeL.setFillColor(sf::Color(200, 50, 50));
        window.draw(ropeL);
        
        // 右侧绳索
        sf::RectangleShape ropeR(sf::Vector2f(30, 2));
        ropeR.setPosition(290, 100 + i * 15);
        ropeR.setFillColor(sf::Color(200, 50, 50));
        window.draw(ropeR);
    }
}

void Game1::renderFighter(sf::RenderWindow& window, const Fighter& fighter) {
    // 无敌闪烁
    if (fighter.isInvincible && static_cast<int>(fighter.invincibleTimer * 10) % 2 == 0) {
        return;
    }
    
    // 阴影
    sf::CircleShape shadow(fighter.width / 2);
    shadow.setPosition(fighter.x, fighter.y + fighter.height - 2);
    shadow.setFillColor(sf::Color(0, 0, 0, 80));
    shadow.setScale(1.0f, 0.3f);
    window.draw(shadow);
    
    // 根据状态渲染不同姿势
    renderFighterDetails(window, fighter);
}

void Game1::renderFighterDetails(sf::RenderWindow& window, const Fighter& fighter) {
    // 角色颜色
    sf::Color bodyColor = fighter.isPlayer ? 
        sf::Color(255, 220, 180) : sf::Color(180, 140, 100); // 肤色
    sf::Color clothColor = fighter.isPlayer ? 
        sf::Color(255, 50, 50) : sf::Color(50, 50, 255); // 衣服颜色
    sf::Color pantsColor = fighter.isPlayer ? 
        sf::Color(255, 255, 255) : sf::Color(50, 50, 50); // 裤子颜色
    
    float x = fighter.x;
    float y = fighter.y;
    bool flip = !fighter.facingRight;
    
    // 根据状态调整姿势
    float bodyOffsetX = 0;
    float bodyOffsetY = 0;
    float legSpread = 0;
    float armAngle = 0;
    
    switch (fighter.state) {
        case FighterState::CROUCH:
            bodyOffsetY = 15;
            break;
            
        case FighterState::JUMP:
            legSpread = 8;
            armAngle = -45;
            break;
            
        case FighterState::LIGHT_PUNCH:
        case FighterState::HEAVY_PUNCH:
            armAngle = flip ? 180 : 0;
            break;
            
        case FighterState::LIGHT_KICK:
        case FighterState::HEAVY_KICK:
            legSpread = 15;
            break;
            
        case FighterState::HIT:
            bodyOffsetX = flip ? 3 : -3;
            break;
            
        case FighterState::KNOCKED_DOWN: {
            // KO躺地姿势（添加大括号创建新作用域）
            sf::RectangleShape koBody(sf::Vector2f(fighter.width, fighter.height));
            koBody.setPosition(x, y + 20);
            koBody.setFillColor(bodyColor);
            koBody.setRotation(90);
            window.draw(koBody);
            return;
        }
            
        case FighterState::WIN:
            armAngle = -90; // 胜利举手
            break;
            
        case FighterState::LOSE:
            bodyOffsetY = 10;
            break;
            
        default:
            // 行走动画
            if (fighter.state == FighterState::WALK_FORWARD || 
                fighter.state == FighterState::WALK_BACKWARD) {
                legSpread = std::sin(fighter.animTimer * 10) * 4;  // 改成animTimer
            }
            break;
    }
    
    // 腿部
    sf::RectangleShape legLeft(sf::Vector2f(6, 18));
    legLeft.setPosition(x + 6 - legSpread + bodyOffsetX, y + 22 + bodyOffsetY);
    legLeft.setFillColor(pantsColor);
    window.draw(legLeft);
    
    sf::RectangleShape legRight(sf::Vector2f(6, 18));
    legRight.setPosition(x + 12 + legSpread + bodyOffsetX, y + 22 + bodyOffsetY);
    legRight.setFillColor(pantsColor);
    window.draw(legRight);
    
    // 躯干
    sf::RectangleShape torso(sf::Vector2f(16, 14));
    torso.setPosition(x + 4 + bodyOffsetX, y + 8 + bodyOffsetY);
    torso.setFillColor(clothColor);
    window.draw(torso);
    
    // 躯干肌肉线条
    sf::RectangleShape muscleL(sf::Vector2f(5, 10));
    muscleL.setPosition(x + 6 + bodyOffsetX, y + 10 + bodyOffsetY);
    muscleL.setFillColor(sf::Color(
        clothColor.r - 30, 
        clothColor.g - 30, 
        clothColor.b - 30
    ));
    window.draw(muscleL);
    
    // 手臂（根据状态调整）
    if (fighter.state == FighterState::LIGHT_PUNCH || 
        fighter.state == FighterState::HEAVY_PUNCH) {
        // 出拳姿势
        float punchExtend = fighter.stateTimer * 100;
        if (punchExtend > 20) punchExtend = 20;
        
        sf::RectangleShape armPunch(sf::Vector2f(8 + punchExtend, 5));
        armPunch.setPosition(
            x + (flip ? 8 - punchExtend : 16) + bodyOffsetX, 
            y + 12 + bodyOffsetY
        );
        armPunch.setFillColor(bodyColor);
        window.draw(armPunch);
        
        // 拳头
        sf::RectangleShape fist(sf::Vector2f(6, 6));
        fist.setPosition(
            x + (flip ? 2 - punchExtend : 24 + punchExtend) + bodyOffsetX,
            y + 11 + bodyOffsetY
        );
        fist.setFillColor(bodyColor);
        window.draw(fist);
        
        // 拳头高光
        sf::RectangleShape fistHighlight(sf::Vector2f(3, 3));
        fistHighlight.setPosition(
            x + (flip ? 3 - punchExtend : 25 + punchExtend) + bodyOffsetX,
            y + 12 + bodyOffsetY
        );
        fistHighlight.setFillColor(sf::Color(255, 255, 255, 150));
        window.draw(fistHighlight);
        
    } else if (fighter.state == FighterState::LIGHT_KICK || 
               fighter.state == FighterState::HEAVY_KICK) {
        // 出腿姿势
        float kickExtend = fighter.stateTimer * 120;
        if (kickExtend > 25) kickExtend = 25;
        
        sf::RectangleShape legKick(sf::Vector2f(10, kickExtend));
        legKick.setPosition(
            x + (flip ? 8 : 12) + bodyOffsetX,
            y + 22 + bodyOffsetY
        );
        legKick.setFillColor(pantsColor);
        legKick.setRotation(flip ? -20 : 20);
        window.draw(legKick);
        
        // 普通手臂
        sf::RectangleShape armNormal(sf::Vector2f(5, 12));
        armNormal.setPosition(x + (flip ? 5 : 15) + bodyOffsetX, y + 10 + bodyOffsetY);
        armNormal.setFillColor(bodyColor);
        window.draw(armNormal);
        
    } else {
        // 正常手臂
        sf::RectangleShape armLeft(sf::Vector2f(5, 12));
        armLeft.setPosition(x + 4 + bodyOffsetX, y + 10 + bodyOffsetY);
        armLeft.setFillColor(bodyColor);
        if (armAngle != 0) armLeft.setRotation(armAngle);
        window.draw(armLeft);
        
        sf::RectangleShape armRight(sf::Vector2f(5, 12));
        armRight.setPosition(x + 15 + bodyOffsetX, y + 10 + bodyOffsetY);
        armRight.setFillColor(bodyColor);
        if (armAngle != 0) armRight.setRotation(armAngle);
        window.draw(armRight);
    }
    
    // 特殊技能特效
    if (fighter.state == FighterState::SPECIAL_MOVE) {
        float glowIntensity = std::sin(fighter.stateTimer * 20) * 0.5f + 0.5f;
        
        // 能量光环
        sf::CircleShape aura(fighter.width);
        aura.setPosition(x - fighter.width/2, y);
        aura.setFillColor(sf::Color(255, 200, 0, static_cast<sf::Uint8>(glowIntensity * 100)));
        window.draw(aura);
        
        // 能量波纹
        for (int i = 0; i < 3; i++) {
            float waveSize = fighter.width * (1 + i * 0.3f + fighter.stateTimer * 2);
            sf::CircleShape wave(waveSize);
            wave.setPosition(x + fighter.width/2 - waveSize, y + fighter.height/2 - waveSize);
            wave.setFillColor(sf::Color::Transparent);
            wave.setOutlineColor(sf::Color(255, 150, 0, static_cast<sf::Uint8>(100 / (i + 1))));
            wave.setOutlineThickness(2);
            window.draw(wave);
        }
    }
    
    // 头部
    sf::RectangleShape head(sf::Vector2f(12, 12));
    head.setPosition(x + 6 + bodyOffsetX, y - 4 + bodyOffsetY);
    head.setFillColor(bodyColor);
    window.draw(head);
    
    // 头发
    sf::RectangleShape hair(sf::Vector2f(14, 6));
    hair.setPosition(x + 5 + bodyOffsetX, y - 6 + bodyOffsetY);
    hair.setFillColor(fighter.isPlayer ? sf::Color(50, 30, 20) : sf::Color(200, 180, 150));
    window.draw(hair);
    
    // 头带（格斗家标志）
    sf::RectangleShape headband(sf::Vector2f(12, 2));
    headband.setPosition(x + 6 + bodyOffsetX, y - 2 + bodyOffsetY);
    headband.setFillColor(fighter.isPlayer ? sf::Color(255, 0, 0) : sf::Color(0, 0, 255));
    window.draw(headband);
    
    // 眼睛
    sf::RectangleShape eye1(sf::Vector2f(2, 2));
    eye1.setPosition(x + (flip ? 12 : 8) + bodyOffsetX, y + 2 + bodyOffsetY);
    eye1.setFillColor(sf::Color::Black);
    window.draw(eye1);
    
    sf::RectangleShape eye2(sf::Vector2f(2, 2));
    eye2.setPosition(x + (flip ? 8 : 12) + bodyOffsetX, y + 2 + bodyOffsetY);
    eye2.setFillColor(sf::Color::Black);
    window.draw(eye2);
    
    // 嘴巴（战斗表情）
    sf::RectangleShape mouth(sf::Vector2f(4, 1));
    mouth.setPosition(x + 9 + bodyOffsetX, y + 6 + bodyOffsetY);
    mouth.setFillColor(sf::Color(100, 50, 50));
    window.draw(mouth);
    
    // 汗水（血量低时）
    if (static_cast<float>(fighter.health) / fighter.maxHealth < 0.3f) {
        float sweatY = y + 4 + std::sin(gameTime * 5) * 2;
        sf::CircleShape sweat(1);
        sweat.setPosition(x + 8 + bodyOffsetX, sweatY + bodyOffsetY);
        sweat.setFillColor(sf::Color(100, 150, 255, 200));
        window.draw(sweat);
    }
}

void Game1::renderHitboxes(sf::RenderWindow& window) {
    // 调试用：显示攻击判定框
    for (const auto& hitbox : hitboxes) {
        if (hitbox.active) {
            sf::RectangleShape box(sf::Vector2f(hitbox.width, hitbox.height));
            box.setPosition(hitbox.x, hitbox.y);
            box.setFillColor(sf::Color::Transparent);
            box.setOutlineColor(sf::Color::Red);
            box.setOutlineThickness(1);
            window.draw(box);
        }
    }
}

void Game1::renderParticles(sf::RenderWindow& window) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            float alpha = 255 * (particles[i].life / particles[i].maxLife);
            sf::Color color = particles[i].color;
            color.a = static_cast<sf::Uint8>(alpha);
            
            if (particles[i].type == 0) {
                // 普通粒子
                sf::CircleShape particle(particles[i].size);
                particle.setPosition(particles[i].x, particles[i].y);
                particle.setFillColor(color);
                window.draw(particle);
                
            } else if (particles[i].type == 1) {
                // 火焰粒子
                sf::CircleShape flame(particles[i].size);
                flame.setPosition(particles[i].x, particles[i].y);
                flame.setFillColor(color);
                window.draw(flame);
                
                // 火焰内核
                sf::CircleShape flameCore(particles[i].size * 0.5f);
                flameCore.setPosition(particles[i].x + particles[i].size * 0.25f, 
                                     particles[i].y + particles[i].size * 0.25f);
                flameCore.setFillColor(sf::Color(255, 255, 200, color.a));
                window.draw(flameCore);
                
            } else if (particles[i].type == 2) {
                // 冲击波粒子（方形）
                sf::RectangleShape shock(sf::Vector2f(particles[i].size, particles[i].size));
                shock.setPosition(particles[i].x, particles[i].y);
                shock.setRotation(particles[i].rotation * 57.3f);
                shock.setFillColor(color);
                window.draw(shock);
                
            } else if (particles[i].type == 3) {
                // 血液粒子
                sf::CircleShape blood(particles[i].size * 0.7f);
                blood.setPosition(particles[i].x, particles[i].y);
                blood.setFillColor(color);
                window.draw(blood);
            }
        }
    }
}

void Game1::renderUI(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    
    // 分数
    sf::Text scoreText;
    scoreText.setFont(gameFont);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setCharacterSize(10);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(120, 5);
    
    // 文字阴影
    sf::Text scoreShadow = scoreText;
    scoreShadow.setPosition(121, 6);
    scoreShadow.setFillColor(sf::Color::Black);
    window.draw(scoreShadow);
    window.draw(scoreText);
}

void Game1::renderHealthBars(sf::RenderWindow& window) {
    // 玩家血条（左侧）
    float playerHealthPercent = static_cast<float>(player.health) / player.maxHealth;
    
    // 血条背景
    sf::RectangleShape playerHealthBg(sf::Vector2f(120, 12));
    playerHealthBg.setPosition(10, 20);
    playerHealthBg.setFillColor(sf::Color(50, 50, 50));
    playerHealthBg.setOutlineColor(sf::Color::White);
    playerHealthBg.setOutlineThickness(2);
    window.draw(playerHealthBg);
    
    // 血条（渐变效果）
    float healthWidth = 116 * playerHealthPercent;
    for (int i = 0; i < healthWidth; i++) {
        float ratio = i / 116.0f;
        sf::Color barColor = getHealthColor(playerHealthPercent);
        
        // 添加渐变
        barColor.r = static_cast<sf::Uint8>(barColor.r * (0.7f + ratio * 0.3f));
        barColor.g = static_cast<sf::Uint8>(barColor.g * (0.7f + ratio * 0.3f));
        barColor.b = static_cast<sf::Uint8>(barColor.b * (0.7f + ratio * 0.3f));
        
        sf::RectangleShape line(sf::Vector2f(1, 8));
        line.setPosition(12 + i, 22);
        line.setFillColor(barColor);
        window.draw(line);
    }
    
    // 血条高光
    sf::RectangleShape playerHealthHighlight(sf::Vector2f(healthWidth, 3));
    playerHealthHighlight.setPosition(12, 22);
    playerHealthHighlight.setFillColor(sf::Color(255, 255, 255, 100));
    window.draw(playerHealthHighlight);
    
    // 玩家名字
    if (fontLoaded) {
        sf::Text playerName;
        playerName.setFont(gameFont);
        playerName.setString("PLAYER");
        playerName.setCharacterSize(8);
        playerName.setFillColor(sf::Color(255, 200, 0));
        playerName.setPosition(12, 10);
        window.draw(playerName);
    }
    
    // AI血条（右侧，镜像显示）
    float aiHealthPercent = static_cast<float>(ai.health) / ai.maxHealth;
    
    // 血条背景
    sf::RectangleShape aiHealthBg(sf::Vector2f(120, 12));
    aiHealthBg.setPosition(190, 20);
    aiHealthBg.setFillColor(sf::Color(50, 50, 50));
    aiHealthBg.setOutlineColor(sf::Color::White);
    aiHealthBg.setOutlineThickness(2);
    window.draw(aiHealthBg);
    
    // AI血条（从右向左）
    float aiHealthWidth = 116 * aiHealthPercent;
    for (int i = 0; i < aiHealthWidth; i++) {
        float ratio = i / 116.0f;
        sf::Color barColor = getHealthColor(aiHealthPercent);
        
        barColor.r = static_cast<sf::Uint8>(barColor.r * (0.7f + ratio * 0.3f));
        barColor.g = static_cast<sf::Uint8>(barColor.g * (0.7f + ratio * 0.3f));
        barColor.b = static_cast<sf::Uint8>(barColor.b * (0.7f + ratio * 0.3f));
        
        sf::RectangleShape line(sf::Vector2f(1, 8));
        line.setPosition(306 - i, 22);
        line.setFillColor(barColor);
        window.draw(line);
    }
    
    // AI血条高光
    sf::RectangleShape aiHealthHighlight(sf::Vector2f(aiHealthWidth, 3));
    aiHealthHighlight.setPosition(306 - aiHealthWidth, 22);
    aiHealthHighlight.setFillColor(sf::Color(255, 255, 255, 100));
    window.draw(aiHealthHighlight);
    
    // AI名字
    if (fontLoaded) {
        sf::Text aiName;
        aiName.setFont(gameFont);
        aiName.setString("COMPUTER");
        aiName.setCharacterSize(8);
        aiName.setFillColor(sf::Color(100, 150, 255));
        aiName.setPosition(255, 10);
        window.draw(aiName);
    }
}

void Game1::renderEnergyBars(sf::RenderWindow& window) {
    // 玩家能量条
    float playerEnergyPercent = player.energy / player.maxEnergy;
    
    // 能量条背景
    sf::RectangleShape playerEnergyBg(sf::Vector2f(120, 6));
    playerEnergyBg.setPosition(10, 34);
    playerEnergyBg.setFillColor(sf::Color(30, 30, 30));
    window.draw(playerEnergyBg);
    
    // 能量条（金色渐变）
    float energyWidth = 120 * playerEnergyPercent;
    for (int i = 0; i < energyWidth; i++) {
        float pulse = std::sin(gameTime * 5 + i * 0.1f) * 0.3f + 0.7f;
        sf::Color energyColor(
            static_cast<sf::Uint8>(255 * pulse),
            static_cast<sf::Uint8>(215 * pulse),
            0
        );
        
        sf::RectangleShape line(sf::Vector2f(1, 6));
        line.setPosition(10 + i, 34);
        line.setFillColor(energyColor);
        window.draw(line);
    }
    
    // 能量满时闪烁
    if (player.energy >= player.maxEnergy) {
        float glow = std::sin(gameTime * 10) * 0.5f + 0.5f;
        sf::RectangleShape energyGlow(sf::Vector2f(120, 8));
        energyGlow.setPosition(10, 33);
        energyGlow.setFillColor(sf::Color(255, 255, 0, static_cast<sf::Uint8>(glow * 100)));
        window.draw(energyGlow);
    }
    
    // AI能量条
    float aiEnergyPercent = ai.energy / ai.maxEnergy;
    
    sf::RectangleShape aiEnergyBg(sf::Vector2f(120, 6));
    aiEnergyBg.setPosition(190, 34);
    aiEnergyBg.setFillColor(sf::Color(30, 30, 30));
    window.draw(aiEnergyBg);
    
    float aiEnergyWidth = 120 * aiEnergyPercent;
    for (int i = 0; i < aiEnergyWidth; i++) {
        float pulse = std::sin(gameTime * 5 + i * 0.1f) * 0.3f + 0.7f;
        sf::Color energyColor(
            static_cast<sf::Uint8>(100 * pulse),
            static_cast<sf::Uint8>(150 * pulse),
            static_cast<sf::Uint8>(255 * pulse)
        );
        
        sf::RectangleShape line(sf::Vector2f(1, 6));
        line.setPosition(310 - i, 34);
        line.setFillColor(energyColor);
        window.draw(line);
    }
    
    if (ai.energy >= ai.maxEnergy) {
        float glow = std::sin(gameTime * 10) * 0.5f + 0.5f;
        sf::RectangleShape energyGlow(sf::Vector2f(120, 8));
        energyGlow.setPosition(190, 33);
        energyGlow.setFillColor(sf::Color(100, 150, 255, static_cast<sf::Uint8>(glow * 100)));
        window.draw(energyGlow);
    }
}

void Game1::renderRoundInfo(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    
    // 回合时间（中央顶部）
    sf::Text timeText;
    timeText.setFont(gameFont);
    timeText.setString(std::to_string(static_cast<int>(roundTime)));
    timeText.setCharacterSize(20);
    
    // 时间少于10秒时变红色
    if (roundTime < 10) {
        float pulse = std::sin(gameTime * 10) * 0.5f + 0.5f;
        timeText.setFillColor(sf::Color(255, static_cast<sf::Uint8>(100 * pulse), 0));
    } else {
        timeText.setFillColor(sf::Color::White);
    }
    
    sf::FloatRect timeBounds = timeText.getLocalBounds();
    timeText.setPosition(160 - timeBounds.width / 2, 15);
    
    // 时间阴影
    sf::Text timeShadow = timeText;
    timeShadow.setPosition(161, 16);
    timeShadow.setFillColor(sf::Color::Black);
    window.draw(timeShadow);
    window.draw(timeText);
    
    // 回合数显示
    sf::Text roundText;
    roundText.setFont(gameFont);
    roundText.setString("ROUND " + std::to_string(currentRound));
    roundText.setCharacterSize(10);
    roundText.setFillColor(sf::Color(255, 215, 0));
    
    sf::FloatRect roundBounds = roundText.getLocalBounds();
    roundText.setPosition(160 - roundBounds.width / 2, 5);
    window.draw(roundText);
    
    // 胜利标记（左侧玩家）
    for (int i = 0; i < playerWins; i++) {
        sf::CircleShape star(4, 5);
        star.setPosition(10 + i * 12, 42);
        star.setFillColor(sf::Color(255, 215, 0));
        star.setOutlineColor(sf::Color(200, 150, 0));
        star.setOutlineThickness(1);
        window.draw(star);
    }
    
    // 胜利标记（右侧AI）
    for (int i = 0; i < aiWins; i++) {
        sf::CircleShape star(4, 5);
        star.setPosition(298 - i * 12, 42);
        star.setFillColor(sf::Color(100, 150, 255));
        star.setOutlineColor(sf::Color(50, 100, 200));
        star.setOutlineThickness(1);
        window.draw(star);
    }
}

void Game1::renderComboCounter(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    
    // 连击数显示
    sf::Text comboText;
    comboText.setFont(gameFont);
    comboText.setString(std::to_string(player.comboCount) + " HIT COMBO!");
    comboText.setCharacterSize(14);
    
    // 连击数越高，字体越大，颜色越亮
    float scale = 1.0f + (player.comboCount / 10.0f) * 0.5f;
    if (scale > 2.0f) scale = 2.0f;
    comboText.setScale(scale, scale);
    
    // 颜色根据连击数变化
    sf::Color comboColor;
    if (player.comboCount < 5) {
        comboColor = sf::Color::Yellow;
    } else if (player.comboCount < 10) {
        comboColor = sf::Color(255, 150, 0);
    } else {
        // 超高连击：彩虹效果
        float hue = std::fmod(gameTime * 2, 1.0f);
        comboColor = sf::Color(
            static_cast<sf::Uint8>(255 * std::abs(std::sin(hue * 6.28f))),
            static_cast<sf::Uint8>(255 * std::abs(std::sin((hue + 0.33f) * 6.28f))),
            static_cast<sf::Uint8>(255 * std::abs(std::sin((hue + 0.66f) * 6.28f)))
        );
    }
    
    comboText.setFillColor(comboColor);
    
    sf::FloatRect comboBounds = comboText.getLocalBounds();
    comboText.setPosition(
        160 - comboBounds.width * scale / 2,
        60
    );
    
    // 连击文字阴影
    sf::Text comboShadow = comboText;
    comboShadow.setPosition(
        161 - comboBounds.width * scale / 2,
        61
    );
    comboShadow.setFillColor(sf::Color::Black);
    window.draw(comboShadow);
    
    // 外发光效果
    sf::Text comboGlow = comboText;
    comboGlow.setFillColor(sf::Color(255, 255, 255, 100));
    comboGlow.setScale(scale * 1.1f, scale * 1.1f);
    comboGlow.setPosition(
        160 - comboBounds.width * scale * 1.1f / 2,
        59
    );
    window.draw(comboGlow);
    
    window.draw(comboText);
}

void Game1::renderRoundStart(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    
    // 半透明遮罩
    sf::RectangleShape overlay(sf::Vector2f(320, 180));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);
    
    // 倒计时或FIGHT!
    sf::Text startText;
    startText.setFont(gameFont);
    
    if (roundStartTimer > 1.0f) {
        // 倒计时数字
        startText.setString(std::to_string(static_cast<int>(std::ceil(roundStartTimer))));
        startText.setCharacterSize(48);
        
        // 脉冲效果
        float pulse = 1.0f - (roundStartTimer - static_cast<int>(roundStartTimer));
        startText.setScale(1.0f + pulse * 0.5f, 1.0f + pulse * 0.5f);
        
        startText.setFillColor(sf::Color(255, 200, 0));
    } else {
        // FIGHT!
        startText.setString("FIGHT!");
        startText.setCharacterSize(36);
        
        float pulse = std::sin(roundStartTimer * 10) * 0.3f + 1.0f;
        startText.setScale(pulse, pulse);
        
        startText.setFillColor(sf::Color::Red);
    }
    
    sf::FloatRect textBounds = startText.getLocalBounds();
    startText.setPosition(
        160 - textBounds.width * startText.getScale().x / 2,
        90 - textBounds.height * startText.getScale().y / 2
    );
    
    // 文字描边
    sf::Text startOutline = startText;
    startOutline.setFillColor(sf::Color::Transparent);
    startOutline.setOutlineColor(sf::Color::Black);
    startOutline.setOutlineThickness(3);
    window.draw(startOutline);
    
    // 主文字
    window.draw(startText);
    
    // 回合标题
    if (roundStartTimer > 2.0f) {
        sf::Text roundTitle;
        roundTitle.setFont(gameFont);
        roundTitle.setString("ROUND " + std::to_string(currentRound));
        roundTitle.setCharacterSize(20);
        roundTitle.setFillColor(sf::Color::White);
        
        sf::FloatRect titleBounds = roundTitle.getLocalBounds();
        roundTitle.setPosition(160 - titleBounds.width / 2, 50);
        window.draw(roundTitle);
    }
}

void Game1::renderRoundEnd(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    
    // 半透明遮罩
    sf::RectangleShape overlay(sf::Vector2f(320, 180));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);
    
    // 判定谁赢了这一回合
    bool playerWon = (player.health > ai.health) || (ai.health <= 0);
    
    sf::Text resultText;
    resultText.setFont(gameFont);
    
    if (playerWon) {
        resultText.setString("PLAYER WINS!");
        resultText.setFillColor(sf::Color(255, 215, 0));
    } else {
        resultText.setString("COMPUTER WINS!");
        resultText.setFillColor(sf::Color(100, 150, 255));
    }
    
    resultText.setCharacterSize(24);
    
    // 脉冲动画
    float pulse = std::sin(gameTime * 5) * 0.2f + 1.0f;
    resultText.setScale(pulse, pulse);
    
    sf::FloatRect resultBounds = resultText.getLocalBounds();
    resultText.setPosition(
        160 - resultBounds.width * pulse / 2,
        70
    );
    
    // 描边
    sf::Text resultOutline = resultText;
    resultOutline.setFillColor(sf::Color::Transparent);
    resultOutline.setOutlineColor(sf::Color::Black);
    resultOutline.setOutlineThickness(2);
    window.draw(resultOutline);
    
    window.draw(resultText);
    
    // KO标志
    if (player.health <= 0 || ai.health <= 0) {
        sf::Text koText;
        koText.setFont(gameFont);
        koText.setString("K.O.");
        koText.setCharacterSize(48);
        koText.setFillColor(sf::Color::Red);
        
        float koPulse = std::sin(gameTime * 8) * 0.3f + 1.2f;
        koText.setScale(koPulse, koPulse);
        
        sf::FloatRect koBounds = koText.getLocalBounds();
        koText.setPosition(
            160 - koBounds.width * koPulse / 2,
            100
        );
        
        // KO描边
        sf::Text koOutline = koText;
        koOutline.setFillColor(sf::Color::Transparent);
        koOutline.setOutlineColor(sf::Color::Black);
        koOutline.setOutlineThickness(4);
        window.draw(koOutline);
        
        window.draw(koText);
    }
}

void Game1::renderGameOver(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    
    // 全屏遮罩
    sf::RectangleShape overlay(sf::Vector2f(320, 180));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);
    
    // 游戏结束标题
    sf::Text gameOverText;
    gameOverText.setFont(gameFont);
    
    if (playerWins >= 2) {
        gameOverText.setString("YOU WIN!");
        gameOverText.setFillColor(sf::Color(255, 215, 0));
        
        // 胜利特效（闪烁星星）
        for (int i = 0; i < 20; i++) {
            float angle = (i / 20.0f) * 6.28f + gameTime * 2;
            float radius = 80 + std::sin(gameTime * 3 + i) * 10;
            float x = 160 + std::cos(angle) * radius;
            float y = 70 + std::sin(angle) * radius;
            
            sf::CircleShape star(3, 5);
            star.setPosition(x, y);
            star.setFillColor(sf::Color(255, 255, 0, 200));
            window.draw(star);
        }
    } else {
        gameOverText.setString("YOU LOSE!");
        gameOverText.setFillColor(sf::Color::Red);
    }
    
    gameOverText.setCharacterSize(32);
    
    float pulse = std::sin(gameTime * 3) * 0.2f + 1.0f;
    gameOverText.setScale(pulse, pulse);
    
    sf::FloatRect gameOverBounds = gameOverText.getLocalBounds();
    gameOverText.setPosition(
        160 - gameOverBounds.width * pulse / 2,
        50
    );
    
    // 描边
    sf::Text gameOverOutline = gameOverText;
    gameOverOutline.setFillColor(sf::Color::Transparent);
    gameOverOutline.setOutlineColor(sf::Color::Black);
    gameOverOutline.setOutlineThickness(3);
    window.draw(gameOverOutline);
    
    window.draw(gameOverText);
    
    // 最终分数
    sf::Text finalScore;
    finalScore.setFont(gameFont);
    finalScore.setString("Final Score: " + std::to_string(score));
    finalScore.setCharacterSize(16);
    finalScore.setFillColor(sf::Color::White);
    
    sf::FloatRect scoreBounds = finalScore.getLocalBounds();
    finalScore.setPosition(160 - scoreBounds.width / 2, 95);
    window.draw(finalScore);
    
    // 比分
    sf::Text matchScore;
    matchScore.setFont(gameFont);
    matchScore.setString("Match: " + std::to_string(playerWins) + " - " + std::to_string(aiWins));
    matchScore.setCharacterSize(14);
    matchScore.setFillColor(sf::Color(200, 200, 200));
    
    sf::FloatRect matchBounds = matchScore.getLocalBounds();
    matchScore.setPosition(160 - matchBounds.width / 2, 115);
    window.draw(matchScore);
    
    // 重新开始提示
    sf::Text restartText;
    restartText.setFont(gameFont);
    restartText.setString("Press SPACE to restart");
    restartText.setCharacterSize(10);
    restartText.setFillColor(sf::Color(150, 150, 150));
    
    sf::FloatRect restartBounds = restartText.getLocalBounds();
    restartText.setPosition(160 - restartBounds.width / 2, 140);
    window.draw(restartText);
    
    // 控制说明
    sf::Text controlsText;
    controlsText.setFont(gameFont);
    controlsText.setString("WASD-Move | JKUI-Attack | L-Special");
    controlsText.setCharacterSize(8);
    controlsText.setFillColor(sf::Color(100, 100, 100));
    
    sf::FloatRect controlsBounds = controlsText.getLocalBounds();
    controlsText.setPosition(160 - controlsBounds.width / 2, 160);
    window.draw(controlsText);
}