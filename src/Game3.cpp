#include "Game3.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Game3::Game3() {
    fontLoaded = false;
    returnButtonInitialized = false;
    cameraX = 0;
    
    std::cout << "Game3 (Super Mario) created" << std::endl;
}

Game3::~Game3() {
    std::cout << "Game3 (Super Mario) destroyed" << std::endl;
}

void Game3::init() {
    std::cout << "Game3 initializing..." << std::endl;
    
    fontLoaded = gameFont.loadFromFile("../assets/fonts/PixelifySans.ttf");
    if (!fontLoaded) {
        std::cerr << "Warning: Could not load font for Game3" << std::endl;
    }
    
    // 生成程序化音效
    jumpSoundBuffer.loadFromSamples(generateJumpSound().data(), 
                                   generateJumpSound().size(), 1, 44100);
    coinSoundBuffer.loadFromSamples(generateCoinSound().data(), 
                                   generateCoinSound().size(), 1, 44100);
    hitSoundBuffer.loadFromSamples(generateHitSound().data(), 
                                  generateHitSound().size(), 1, 44100);
    powerUpSoundBuffer.loadFromSamples(generatePowerUpSound().data(), 
                                      generatePowerUpSound().size(), 1, 44100);
    
    reset();
    std::cout << "Game3 initialized!" << std::endl;
}

void Game3::reset() {
    std::cout << "Game3 reset" << std::endl;
    
    gameOver = false;
    gamePaused = false;
    levelComplete = false;
    score = 0;
    gameTime = 0;
    cameraX = 0;
    
    // 初始化玩家
    player.x = 40;
    player.y = 120;
    player.vx = 0;
    player.vy = 0;
    player.width = 12;
    player.height = 16;
    player.onGround = false;
    player.jumping = false;
    player.health = 3;
    player.invincible = false;
    player.invincibleTimer = 0;
    player.coins = 0;
    player.isDead = false;
    player.animTime = 0;
    player.animFrame = 0;
    
    // 清空容器
    blocks.clear();
    enemies.clear();
    coins.clear();
    items.clear();
    
    // 生成关卡
    generateLevel();
    
    // 初始化粒子
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = false;
    }
    
    gameClock.restart();
}

void Game3::generateLevel() {
    // 地面
    for (int i = 0; i < 100; i++) {
        generateBlock(i * 16, 160, 0, false);
    }
    
    // 第一段：简单的平台和问号砖
    generateBlock(80, 128, 1, true);  // 问号砖（有蘑菇）
    generateBlock(96, 128, 1, true);  // 问号砖（有金币）
    generateBlock(112, 128, 1, false);
    
    generateBlock(160, 112, 0, false);
    generateBlock(176, 112, 0, false);
    generateBlock(192, 112, 0, false);
    
    // 敌人
    generateEnemy(120, 144, 0);
    generateEnemy(200, 144, 0);
    
    // 空中金币
    for (int i = 0; i < 5; i++) {
        generateCoin(240 + i * 16, 96);
    }
    
    // 管道
    generateBlock(320, 144, 3, false);
    generateBlock(320, 128, 3, false);
    
    // 第二段：跳跃挑战
    generateBlock(400, 128, 0, false);
    generateBlock(416, 128, 0, false);
    generateBlock(432, 112, 0, false);
    generateBlock(448, 96, 0, false);
    
    generateBlock(480, 96, 1, true);
    
    // 更多敌人
    generateEnemy(420, 112, 1);
    generateEnemy(500, 144, 0);
    
    // 第三段：砖块迷宫
    for (int i = 0; i < 8; i++) {
        generateBlock(560 + i * 16, 128, 0, false);
    }
    
    generateBlock(600, 96, 1, true);
    generateBlock(616, 96, 0, false);
    generateBlock(632, 96, 1, true);
    
    // 终点旗杆（用管道代表）
    for (int i = 0; i < 2; i++) {
        generateBlock(800, 160 - i * 16, 3, false);
    }
    
    // 更多敌人和金币
    for (int i = 0; i < 10; i++) {
        if (rand() % 3 == 0) {
            generateEnemy(300 + i * 80, 144, rand() % 2);
        }
        if (rand() % 2 == 0) {
            generateCoin(350 + i * 60, 80 + rand() % 40);
        }
    }
}

void Game3::generateBlock(float x, float y, int type, bool hasItem) {
    Block block;
    block.x = x;
    block.y = y;
    block.width = 16;
    block.height = 16;
    block.type = type;
    block.broken = false;
    block.hasItem = hasItem;
    block.bounceOffset = 0;
    blocks.push_back(block);
}

void Game3::generateEnemy(float x, float y, int type) {
    Game3Enemy enemy;
    enemy.x = x;
    enemy.y = y;
    enemy.vx = -30.0f;
    enemy.width = 14;
    enemy.height = 14;
    enemy.active = true;
    enemy.moving = true;
    enemy.type = type;
    enemy.animTime = 0;
    enemies.push_back(enemy);
}

void Game3::generateCoin(float x, float y) {
    Coin coin;
    coin.x = x;
    coin.y = y;
    coin.vy = 0;
    coin.collected = false;
    coin.animTime = 0;
    coins.push_back(coin);
}

void Game3::handleInput(const sf::Event& event, sf::RenderWindow& window) {
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
        
        // 跳跃
        if (event.key.code == sf::Keyboard::Space || 
            event.key.code == sf::Keyboard::W ||
            event.key.code == sf::Keyboard::Up) {
            if (player.onGround && !player.isDead) {
                player.vy = -250.0f;
                player.jumping = true;
                player.onGround = false;
                gameSound.setBuffer(jumpSoundBuffer);
                gameSound.play();
            }
        }
        
        // 游戏结束后重新开始
        if (gameOver && event.key.code == sf::Keyboard::Space) {
            reset();
        }
    }
}

void Game3::update(float deltaTime) {
    if (gameOver || gamePaused) return;
    
    gameTime += deltaTime;
    
    // 限制deltaTime防止穿透
    if (deltaTime > 0.05f) deltaTime = 0.05f;
    
    updatePlayer(deltaTime);
    updateEnemies(deltaTime);
    updateCoins(deltaTime);
    updateItems(deltaTime);
    updateParticles(deltaTime);
    updateCamera();
    
    if (player.x >= 800)
    {
        levelComplete = true;
        gameOver = true;
    }
    
    // 检查玩家是否掉落
    if (player.y > 200) {
        player.isDead = true;
        player.health = 0;
    }
    
    if (player.health <= 0 && !gameOver) {
        gameOver = true;
    }
}

void Game3::updatePlayer(float deltaTime) {
    if (player.isDead) {
        player.vy += 600.0f * deltaTime;
        player.y += player.vy * deltaTime;
        return;
        return;
    }

    // 无敌时间与闪烁
    if (player.invincible) {
        player.invincibleTimer -= deltaTime;
        if (player.invincibleTimer <= 0.0f) player.invincible = false;
    }

    // 动画计时
    player.animTime += deltaTime;
    if (player.animTime > 0.1f) {
        player.animTime -= 0.1f;
        player.animFrame = (player.animFrame + 1) % 4;
    }

    // ==================== 1. 水平运动 + 水平碰撞 ====================
    player.vx = 0.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        player.vx = -120.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        player.vx = +120.0f;

    player.x += player.vx * deltaTime;

    // X 方向只处理左右挤压（使用中心点判断推左还是推右）
    for (const auto& block : blocks) {
        if (block.broken) continue;

        if (checkCollision(player.x, player.y, player.width, player.height,
                           block.x, block.y, block.width, block.height)) {

            float playerCenterX = player.x + player.width  / 2.0f;
            float blockCenterX = block.x + block.width / 2.0f;

            if (playerCenterX < blockCenterX)                 // 玩家在左边 → 推左
                player.x = block.x - player.width;
            else                                              // 玩家在右边 → 推右
                player.x = block.x + block.width;

            player.vx = 0.0f;        // 撞墙停住
        }
    }
    if (player.x < 10.0f) player.x = 10.0f;

    // ==================== 2. 垂直运动 + 垂直碰撞 ====================
    player.vy += 800.0f * deltaTime;           // 重力
    if (player.vy > 400.0f) player.vy = 400.0f;

    player.y += player.vy * deltaTime;

    player.onGround = false;                  // 每帧先假设不在地面

    for (auto& block : blocks) {              // 注意这里要 & 才能修改 bounceOffset
        if (block.broken) continue;

        // 砖块被顶时的弹跳动画
        if (block.bounceOffset > 0.0f) block.bounceOffset -= 0.8f;

        if (checkCollision(player.x, player.y, player.width, player.height,
                           block.x, block.y, block.width, block.height)) {

            // 计算穿透深度（用于更精确判断上下左右）
            float playerTop    = player.y;
            float playerBottom = player.y + player.height;
            float playerLeft   = player.x;
            float playerRight  = player.x + player.width;

            float blockTop    = block.y;
            float blockBottom = block.y + block.height;
            float blockLeft   = block.x;
            float blockRight  = block.x + block.width;

            float topDepth    = playerBottom - blockTop;
            float bottomDepth = blockBottom - playerTop;
            float leftDepth   = playerRight  - blockLeft;
            float rightDepth  = blockRight - playerLeft;

            // 取最小穿透方向作为最终分离方向
            if (topDepth < bottomDepth && topDepth < leftDepth && topDepth < rightDepth) {
                // 从上方落地
                player.y = blockTop - player.height;
                player.vy = 0.0f;
                player.onGround = true;
                player.jumping = false;
            }
            else if (bottomDepth < topDepth && bottomDepth < leftDepth && bottomDepth < rightDepth) {
                // 从下方顶砖块（最重要！）
                player.y = blockBottom;
                player.vy = 0.0f;
                hitBlock(block);                     // ← 这里弹出道具
            }
            else if (leftDepth < rightDepth) {
                player.x = blockLeft - player.width;
                player.vx = 0.0f;
            }
            else {
                player.x = blockRight;
                player.vx = 0.0f;
            }
        }
    }

    // ==================== 3. 其他碰撞检测（金币、道具、敌人） ====================
    checkPlayerCoinCollision();
    checkPlayerItemCollision();     // 现在可以正常吃到蘑菇/火焰花了
    checkPlayerEnemyCollision();

    // 掉出屏幕死亡
    if (player.y > 220.0f) {
        damagePlayer();
    }
}

void Game3::updateEnemies(float deltaTime) {
    for (auto& enemy : enemies) {
        if (!enemy.active) continue;
        
        enemy.animTime += deltaTime;
        
        // 移动
        enemy.x += enemy.vx * deltaTime;
        
        // 重力
        enemy.y += 200.0f * deltaTime;
        
        // 检查是否在地面上
        bool onGround = false;
        for (const auto& block : blocks) {
            if (checkCollision(enemy.x, enemy.y + 1, enemy.width, enemy.height,
                             block.x, block.y, block.width, block.height)) {
                enemy.y = block.y - enemy.height;
                onGround = true;
                break;
            }
        }
        
        // 检查墙壁碰撞（转向）
        for (const auto& block : blocks) {
            if (checkCollision(enemy.x + enemy.vx * deltaTime, enemy.y, 
                             enemy.width, enemy.height,
                             block.x, block.y, block.width, block.height)) {
                enemy.vx = -enemy.vx;
                break;
            }
        }
        
        // 移除屏幕外的敌人
        if (enemy.x < cameraX - 50 || enemy.x > cameraX + 370 || enemy.y > 200) {
            enemy.active = false;
        }
    }
}

void Game3::updateCoins(float deltaTime) {
    for (auto& coin : coins) {
        if (coin.collected) {
            coin.vy -= 300.0f * deltaTime;
            coin.y += coin.vy * deltaTime;
        }
        coin.animTime += deltaTime;
    }
}

void Game3::updateItems(float deltaTime) {
    for (auto& item : items) {
        if (!item.active) continue;
        
        if (item.spawning) {
            item.spawnTimer -= deltaTime;
            item.y -= 30.0f * deltaTime;
            if (item.spawnTimer <= 0) {
                item.spawning = false;
            }
            continue;
        }
        
        // 重力
        item.vy += 400.0f * deltaTime;
        item.y += item.vy * deltaTime;
        
        // 检查地面碰撞
        for (const auto& block : blocks) {
            if (checkCollision(item.x, item.y, 12, 12,
                             block.x, block.y, block.width, block.height)) {
                item.y = block.y - 12;
                item.vy = 0;
                break;
            }
        }
    }
}

void Game3::updateParticles(float deltaTime) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].vx *= 0.98f;
            particles[i].vy += 400.0f * deltaTime;
            particles[i].x += particles[i].vx * deltaTime;
            particles[i].y += particles[i].vy * deltaTime;
            particles[i].life -= deltaTime;
            
            if (particles[i].life <= 0) {
                particles[i].active = false;
            }
        }
    }
}

void Game3::updateCamera() {
    // 摄像机跟随玩家
    float targetX = player.x - 160;
    if (targetX < 0) targetX = 0;
    
    cameraX += (targetX - cameraX) * 0.1f;
}

void Game3::checkPlayerBlockCollision() {
    player.onGround = false;
    
    for (auto& block : blocks) {
        if (block.broken) continue;
        
        // 更新砖块弹跳动画
        if (block.bounceOffset > 0) {
            block.bounceOffset -= 0.5f;
        }
        
        if (checkCollision(player.x, player.y, player.width, player.height,
                         block.x, block.y, block.width, block.height)) {
            
            // 从上方落下
            if (player.vy > 0 && player.y + player.height - player.vy * 0.016f <= block.y) {
                player.y = block.y - player.height;
                player.vy = 0;
                player.onGround = true;
                player.jumping = false;
            }
            // 从下方顶砖块
            else if (player.vy < 0 && player.y - player.vy * 0.016f >= block.y + block.height) {
                player.y = block.y + block.height;
                player.vy = 0;
                hitBlock(block);
            }
            // 左右碰撞
            else {
                // 修复：根据玩家/砖块中心位置决定推左/右（标准 AABB 分离）
                float playerCenterX = player.x + player.width / 2.0f;
                float blockCenterX = block.x + block.width / 2.0f;
                if (playerCenterX > blockCenterX) {
                    player.x = block.x + block.width;     // 玩家在右 → 推右
                } else {
                    player.x = block.x - player.width;    // 玩家在左 → 推左
                }
            }
        }
    }
}

void Game3::hitBlock(Block& block) {
    block.bounceOffset = 5;
    
    if (block.type == 1) { // 问号砖
        if (block.hasItem) {
            // 生成道具
            Item item;
            item.x = block.x;
            item.y = block.y - 12;
            item.vy = 0;
            item.type = (rand() % 2 == 0) ? 0 : 1;
            item.active = true;
            item.spawning = true;
            item.spawnTimer = 0.3f;
            items.push_back(item);
            
            block.hasItem = false;
            block.type = 0; // 变成普通砖
            
            gameSound.setBuffer(powerUpSoundBuffer);
            gameSound.play();
        } else {
            // 已经被顶过了，只有音效
            gameSound.setBuffer(hitSoundBuffer);
            gameSound.play();
        }
        
        createParticles(block.x + 8, block.y, sf::Color(255, 200, 0), 5);
    }
}

void Game3::checkPlayerEnemyCollision() {
    if (player.invincible || player.isDead) return;
    
    for (auto& enemy : enemies) {
        if (!enemy.active) continue;
        
        if (checkCollision(player.x, player.y, player.width, player.height,
                         enemy.x, enemy.y, enemy.width, enemy.height)) {
            
            // 从上方跳到敌人头上
            if (player.vy > 0 && player.y + player.height - 10 < enemy.y + enemy.height / 2) {
                killEnemy(enemy);
                player.vy = -200.0f; // 弹跳
                score += 100;
            } else {
                // 受伤
                damagePlayer();
            }
        }
    }
}

void Game3::checkPlayerCoinCollision() {
    for (auto& coin : coins) {
        if (coin.collected) continue;
        
        if (checkCollision(player.x, player.y, player.width, player.height,
                         coin.x, coin.y, 8, 8)) {
            collectCoin(coin);
        }
    }
}

void Game3::checkPlayerItemCollision() {
    for (auto& item : items) {
        if (!item.active || item.spawning) continue;
        
        if (checkCollision(player.x, player.y, player.width, player.height,
                         item.x, item.y, 12, 12)) {
            if (item.type == 0) { // 蘑菇
                if (player.health < 3) player.health++;
            } else { // 火焰花
                score += 200;
            }
            
            item.active = false;
            createParticles(item.x + 6, item.y + 6, sf::Color::Yellow, 8);
            
            gameSound.setBuffer(powerUpSoundBuffer);
            gameSound.play();
        }
    }
}

bool Game3::checkCollision(float x1, float y1, float w1, float h1,
                           float x2, float y2, float w2, float h2) {
    return x1 < x2 + w2 && x1 + w1 > x2 &&
           y1 < y2 + h2 && y1 + h1 > y2;
}

void Game3::killEnemy(Game3Enemy& enemy) {
    enemy.active = false;
    createParticles(enemy.x + enemy.width / 2, enemy.y + enemy.height / 2, 
                   sf::Color(200, 100, 50), 10);
    
    gameSound.setBuffer(hitSoundBuffer);
    gameSound.play();
}

void Game3::collectCoin(Coin& coin) {
    coin.collected = true;
    coin.vy = -100.0f;
    player.coins++;
    score += 10;
    
    gameSound.setBuffer(coinSoundBuffer);
    gameSound.play();
    
    createParticles(coin.x + 4, coin.y + 4, sf::Color::Yellow, 5);
}

void Game3::damagePlayer() {
    player.health--;
    player.invincible = true;
    player.invincibleTimer = 2.0f;
    
    gameSound.setBuffer(hitSoundBuffer);
    gameSound.play();
    
    createParticles(player.x + player.width / 2, player.y + player.height / 2, 
                   sf::Color::Red, 15);
}

void Game3::createParticles(float x, float y, sf::Color color, int count) {
    for (int i = 0; i < MAX_PARTICLES && count > 0; i++) {
        if (!particles[i].active) {
            particles[i].x = x;
            particles[i].y = y;
            particles[i].vx = (rand() % 200 - 100);
            particles[i].vy = -(rand() % 150 + 50);
            particles[i].life = 0.5f + (rand() % 100) / 200.0f;
            particles[i].color = color;
            particles[i].active = true;
            particles[i].size = 2 + rand() % 3;
            count--;
        }
    }
}

// 程序化音效生成
std::vector<sf::Int16> Game3::generateJumpSound() {
    std::vector<sf::Int16> samples;
    const int sampleRate = 44100;
    const float duration = 0.15f;
    
    for (int i = 0; i < static_cast<int>(sampleRate * duration); i++) {
        float t = static_cast<float>(i) / sampleRate;
        float freq = 400 + (1.0f - t / duration) * 400;
        float amp = (1.0f - t / duration) * 10000;
        samples.push_back(static_cast<sf::Int16>(std::sin(2 * 3.14159f * freq * t) * amp));
    }
    
    return samples;
}

std::vector<sf::Int16> Game3::generateCoinSound() {
    std::vector<sf::Int16> samples;
    const int sampleRate = 44100;
    const float duration = 0.1f;
    
    for (int i = 0; i < static_cast<int>(sampleRate * duration); i++) {
        float t = static_cast<float>(i) / sampleRate;
        float freq = 800 + std::sin(t * 50) * 200;
        float amp = (1.0f - t / duration) * 8000;
        samples.push_back(static_cast<sf::Int16>(std::sin(2 * 3.14159f * freq * t) * amp));
    }
    
    return samples;
}

std::vector<sf::Int16> Game3::generateHitSound() {
    std::vector<sf::Int16> samples;
    const int sampleRate = 44100;
    const float duration = 0.2f;
    
    for (int i = 0; i < static_cast<int>(sampleRate * duration); i++) {
        float t = static_cast<float>(i) / sampleRate;
        float noise = (rand() % 1000 - 500) / 500.0f;
        float amp = (1.0f - t / duration) * 12000;
        samples.push_back(static_cast<sf::Int16>(noise * amp));
    }
    
    return samples;
}

std::vector<sf::Int16> Game3::generatePowerUpSound() {
    std::vector<sf::Int16> samples;
    const int sampleRate = 44100;
    const float duration = 0.3f;
    
    for (int i = 0; i < static_cast<int>(sampleRate * duration); i++) {
        float t = static_cast<float>(i) / sampleRate;
        float freq = 400 + (t / duration) * 600;
        float amp = 10000;
        samples.push_back(static_cast<sf::Int16>(std::sin(2 * 3.14159f * freq * t) * amp));
    }
    
    return samples;
}

// 渲染部分太长了，我分成下一条消息发送...
void Game3::render(sf::RenderWindow& window) {
    // 重置视图（必须！）
    sf::View gameView(sf::FloatRect(cameraX, 0, 320, 180));
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
    renderBlocks(window);
    renderCoins(window);
    renderItems(window);
    renderEnemies(window);
    renderPlayer(window);
    renderParticles(window);
    
    // UI使用固定视图
    sf::View uiView(sf::FloatRect(0, 0, 320, 180));
    window.setView(uiView);
    
    renderUI(window);
    
    // 游戏结束画面
    if (gameOver) {
        sf::RectangleShape overlay(sf::Vector2f(320, 180));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);
        
        if (fontLoaded) {
            sf::Text gameOverText;
            gameOverText.setFont(gameFont);
            
            if (levelComplete) {
                gameOverText.setString("LEVEL COMPLETE!");
                gameOverText.setFillColor(sf::Color::Yellow);
            } else {
                gameOverText.setString("GAME OVER");
                gameOverText.setFillColor(sf::Color::Red);
            }
            
            gameOverText.setCharacterSize(20);
            sf::FloatRect textBounds = gameOverText.getLocalBounds();
            gameOverText.setPosition(160 - textBounds.width / 2, 50);
            window.draw(gameOverText);
            
            sf::Text scoreText;
            scoreText.setFont(gameFont);
            scoreText.setString("Score: " + std::to_string(score));
            scoreText.setCharacterSize(14);
            scoreText.setFillColor(sf::Color::White);
            textBounds = scoreText.getLocalBounds();
            scoreText.setPosition(160 - textBounds.width / 2, 80);
            window.draw(scoreText);
            
            sf::Text coinsText;
            coinsText.setFont(gameFont);
            coinsText.setString("Coins: " + std::to_string(player.coins));
            coinsText.setCharacterSize(14);
            coinsText.setFillColor(sf::Color(255, 215, 0));
            textBounds = coinsText.getLocalBounds();
            coinsText.setPosition(160 - textBounds.width / 2, 100);
            window.draw(coinsText);
            
            sf::Text restartText;
            restartText.setFont(gameFont);
            restartText.setString("Press SPACE to restart");
            restartText.setCharacterSize(10);
            restartText.setFillColor(sf::Color(200, 200, 200));
            textBounds = restartText.getLocalBounds();
            restartText.setPosition(160 - textBounds.width / 2, 130);
            window.draw(restartText);
        }
    }
    
    // 返回按钮（固定在屏幕上）
    if (!gameOver && returnButtonInitialized) {
        returnButton.render(window);
    }
}

void Game3::renderBackground(sf::RenderWindow& window) {
    // 天空渐变
    for (int i = 0; i < 90; i++) {
        float ratio = i / 90.0f;
        sf::Color skyColor(
            100 + static_cast<int>(55 * ratio),
            150 + static_cast<int>(50 * ratio),
            255
        );
        sf::RectangleShape line(sf::Vector2f(1600, 1));
        line.setPosition(0, i);
        line.setFillColor(skyColor);
        window.draw(line);
    }
    
    // 地面渐变（草地到土壤）
    for (int i = 90; i < 180; i++) {
        float ratio = (i - 90) / 90.0f;
        sf::Color groundColor(
            100 - static_cast<int>(30 * ratio),
            180 - static_cast<int>(80 * ratio),
            80 - static_cast<int>(30 * ratio)
        );
        sf::RectangleShape line(sf::Vector2f(1600, 1));
        line.setPosition(0, i);
        line.setFillColor(groundColor);
        window.draw(line);
    }
    
    // 云朵装饰
    int cloudSeed = static_cast<int>(cameraX / 80);
    for (int i = 0; i < 5; i++) {
        float cloudX = (cloudSeed + i) * 80 + 40 - std::fmod(cameraX * 0.3f, 80);
        float cloudY = 20 + (i % 3) * 15;
        
        // 简单的云朵（三个圆）
        sf::CircleShape cloud1(8);
        cloud1.setPosition(cloudX, cloudY);
        cloud1.setFillColor(sf::Color(255, 255, 255, 200));
        window.draw(cloud1);
        
        sf::CircleShape cloud2(10);
        cloud2.setPosition(cloudX + 10, cloudY - 2);
        cloud2.setFillColor(sf::Color(255, 255, 255, 200));
        window.draw(cloud2);
        
        sf::CircleShape cloud3(7);
        cloud3.setPosition(cloudX + 22, cloudY + 2);
        cloud3.setFillColor(sf::Color(255, 255, 255, 200));
        window.draw(cloud3);
    }
    
    // 远山装饰
    for (int i = 0; i < 10; i++) {
        float hillX = i * 100 - std::fmod(cameraX * 0.5f, 100);
        sf::ConvexShape hill;
        hill.setPointCount(3);
        hill.setPoint(0, sf::Vector2f(hillX + 50, 90));
        hill.setPoint(1, sf::Vector2f(hillX, 120));
        hill.setPoint(2, sf::Vector2f(hillX + 100, 120));
        hill.setFillColor(sf::Color(80, 140, 80, 150));
        window.draw(hill);
    }
}

void Game3::renderBlocks(sf::RenderWindow& window) {
    for (const auto& block : blocks) {
        if (block.broken) continue;
        
        float drawY = block.y - block.bounceOffset;
        
        sf::RectangleShape blockShape(sf::Vector2f(block.width, block.height));
        blockShape.setPosition(block.x, drawY);
        
        if (block.type == 0) { // 普通砖块
            // 砖块主体
            blockShape.setFillColor(sf::Color(180, 100, 50));
            window.draw(blockShape);
            
            // 砖块纹理
            sf::RectangleShape line1(sf::Vector2f(16, 1));
            line1.setPosition(block.x, drawY + 8);
            line1.setFillColor(sf::Color(140, 80, 40));
            window.draw(line1);
            
            sf::RectangleShape line2(sf::Vector2f(8, 1));
            line2.setPosition(block.x, drawY + 4);
            line2.setFillColor(sf::Color(140, 80, 40));
            window.draw(line2);
            
            sf::RectangleShape line3(sf::Vector2f(8, 1));
            line3.setPosition(block.x + 8, drawY + 12);
            line3.setFillColor(sf::Color(140, 80, 40));
            window.draw(line3);
            
            // 高光
            sf::RectangleShape highlight(sf::Vector2f(14, 2));
            highlight.setPosition(block.x + 1, drawY + 1);
            highlight.setFillColor(sf::Color(220, 140, 80, 150));
            window.draw(highlight);
            
        } else if (block.type == 1) { // 问号砖块
            // 问号砖主体（金黄色）
            blockShape.setFillColor(sf::Color(255, 200, 0));
            window.draw(blockShape);
            
            // 闪烁效果
            float pulse = std::sin(gameTime * 5.0f) * 0.3f + 0.7f;
            sf::RectangleShape glow(sf::Vector2f(12, 12));
            glow.setPosition(block.x + 2, drawY + 2);
            glow.setFillColor(sf::Color(255, 255, 150, static_cast<sf::Uint8>(pulse * 100)));
            window.draw(glow);
            
            // 问号标记（简化版）
            if (block.hasItem) {
                sf::RectangleShape qTop(sf::Vector2f(4, 2));
                qTop.setPosition(block.x + 6, drawY + 4);
                qTop.setFillColor(sf::Color(100, 50, 0));
                window.draw(qTop);
                
                sf::RectangleShape qMiddle(sf::Vector2f(2, 4));
                qMiddle.setPosition(block.x + 7, drawY + 6);
                qMiddle.setFillColor(sf::Color(100, 50, 0));
                window.draw(qMiddle);
                
                sf::RectangleShape qDot(sf::Vector2f(2, 2));
                qDot.setPosition(block.x + 7, drawY + 11);
                qDot.setFillColor(sf::Color(100, 50, 0));
                window.draw(qDot);
            }
            
            // 边框
            sf::RectangleShape border(sf::Vector2f(block.width, block.height));
            border.setPosition(block.x, drawY);
            border.setFillColor(sf::Color::Transparent);
            border.setOutlineColor(sf::Color(200, 150, 0));
            border.setOutlineThickness(1);
            window.draw(border);
            
        } else if (block.type == 3) { // 管道
            // 管道主体
            blockShape.setFillColor(sf::Color(50, 180, 50));
            window.draw(blockShape);
            
            // 管道高光
            sf::RectangleShape pipeHighlight(sf::Vector2f(6, block.height));
            pipeHighlight.setPosition(block.x + 2, drawY);
            pipeHighlight.setFillColor(sf::Color(80, 220, 80, 150));
            window.draw(pipeHighlight);
            
            // 管道阴影
            sf::RectangleShape pipeShadow(sf::Vector2f(5, block.height));
            pipeShadow.setPosition(block.x + 11, drawY);
            pipeShadow.setFillColor(sf::Color(30, 120, 30));
            window.draw(pipeShadow);
            
            // 管道边框
            sf::RectangleShape pipeBorder(sf::Vector2f(block.width, block.height));
            pipeBorder.setPosition(block.x, drawY);
            pipeBorder.setFillColor(sf::Color::Transparent);
            pipeBorder.setOutlineColor(sf::Color(30, 100, 30));
            pipeBorder.setOutlineThickness(1);
            window.draw(pipeBorder);
        }
    }
}

void Game3::renderPlayer(sf::RenderWindow& window) {
    // 无敌闪烁效果
    if (player.invincible) {
        if (static_cast<int>(player.invincibleTimer * 10) % 2 == 0) {
            return; // 闪烁
        }
    }
    
    if (player.isDead) {
        // 死亡状态：只画一个倒立的马里奥
        sf::RectangleShape body(sf::Vector2f(player.width, player.height));
        body.setPosition(player.x, player.y);
        body.setFillColor(sf::Color::Red);
        body.setRotation(180);
        window.draw(body);
        return;
    }
    
    // 马里奥身体（红色衣服）
    sf::RectangleShape body(sf::Vector2f(player.width, 10));
    body.setPosition(player.x, player.y + 6);
    body.setFillColor(sf::Color(220, 20, 20));
    window.draw(body);
    
    // 马里奥头部（肤色）
    sf::RectangleShape head(sf::Vector2f(10, 6));
    head.setPosition(player.x + 1, player.y);
    head.setFillColor(sf::Color(255, 220, 180));
    window.draw(head);
    
    // 帽子（红色）
    sf::RectangleShape hat(sf::Vector2f(12, 3));
    hat.setPosition(player.x, player.y - 2);
    hat.setFillColor(sf::Color(200, 0, 0));
    window.draw(hat);
    
    // 帽子标志M（白色）
    sf::RectangleShape mLetter(sf::Vector2f(4, 2));
    mLetter.setPosition(player.x + 4, player.y - 1);
    mLetter.setFillColor(sf::Color::White);
    window.draw(mLetter);
    
    // 眼睛
    sf::RectangleShape eye1(sf::Vector2f(2, 2));
    eye1.setPosition(player.x + 3, player.y + 2);
    eye1.setFillColor(sf::Color::Black);
    window.draw(eye1);
    
    sf::RectangleShape eye2(sf::Vector2f(2, 2));
    eye2.setPosition(player.x + 7, player.y + 2);
    eye2.setFillColor(sf::Color::Black);
    window.draw(eye2);
    
    // 胡子
    sf::RectangleShape mustache(sf::Vector2f(6, 1));
    mustache.setPosition(player.x + 3, player.y + 5);
    mustache.setFillColor(sf::Color(100, 50, 0));
    window.draw(mustache);
    
    // 裤子（蓝色）
    sf::RectangleShape pants(sf::Vector2f(player.width, 4));
    pants.setPosition(player.x, player.y + 12);
    pants.setFillColor(sf::Color(20, 80, 200));
    window.draw(pants);
    
    // 腿部动画（跑步时）
    if (std::abs(player.vx) > 10 && player.onGround) {
        int legOffset = (player.animFrame % 2 == 0) ? 1 : -1;
        
        sf::RectangleShape leg1(sf::Vector2f(4, 3));
        leg1.setPosition(player.x + 2 + legOffset, player.y + 13);
        leg1.setFillColor(sf::Color(255, 220, 180));
        window.draw(leg1);
        
        sf::RectangleShape leg2(sf::Vector2f(4, 3));
        leg2.setPosition(player.x + 6 - legOffset, player.y + 13);
        leg2.setFillColor(sf::Color(255, 220, 180));
        window.draw(leg2);
    }
    
    // 身体高光
    sf::RectangleShape bodyHighlight(sf::Vector2f(5, 4));
    bodyHighlight.setPosition(player.x + 2, player.y + 7);
    bodyHighlight.setFillColor(sf::Color(255, 100, 100, 120));
    window.draw(bodyHighlight);
    
    // 手臂（跳跃时向上）
    if (!player.onGround) {
        sf::RectangleShape arm1(sf::Vector2f(3, 5));
        arm1.setPosition(player.x, player.y + 4);
        arm1.setFillColor(sf::Color(255, 220, 180));
        window.draw(arm1);
        
        sf::RectangleShape arm2(sf::Vector2f(3, 5));
        arm2.setPosition(player.x + 9, player.y + 4);
        arm2.setFillColor(sf::Color(255, 220, 180));
        window.draw(arm2);
    }
}

void Game3::renderEnemies(sf::RenderWindow& window) {
    for (const auto& enemy : enemies) {
        if (!enemy.active) continue;
        
        if (enemy.type == 0) { // 蘑菇怪（库里波）
            // 身体（棕色）
            sf::RectangleShape body(sf::Vector2f(enemy.width, 10));
            body.setPosition(enemy.x, enemy.y + 4);
            body.setFillColor(sf::Color(140, 80, 40));
            window.draw(body);
            
            // 头部斑点
            sf::RectangleShape spot1(sf::Vector2f(3, 3));
            spot1.setPosition(enemy.x + 2, enemy.y + 6);
            spot1.setFillColor(sf::Color(100, 50, 20));
            window.draw(spot1);
            
            sf::RectangleShape spot2(sf::Vector2f(3, 3));
            spot2.setPosition(enemy.x + 9, enemy.y + 6);
            spot2.setFillColor(sf::Color(100, 50, 20));
            window.draw(spot2);
            
            // 眼睛（生气的表情）
            sf::RectangleShape eye1(sf::Vector2f(2, 2));
            eye1.setPosition(enemy.x + 3, enemy.y + 2);
            eye1.setFillColor(sf::Color::White);
            window.draw(eye1);
            
            sf::RectangleShape eye2(sf::Vector2f(2, 2));
            eye2.setPosition(enemy.x + 9, enemy.y + 2);
            eye2.setFillColor(sf::Color::White);
            window.draw(eye2);
            
            sf::RectangleShape pupil1(sf::Vector2f(1, 1));
            pupil1.setPosition(enemy.x + 4, enemy.y + 3);
            pupil1.setFillColor(sf::Color::Black);
            window.draw(pupil1);
            
            sf::RectangleShape pupil2(sf::Vector2f(1, 1));
            pupil2.setPosition(enemy.x + 10, enemy.y + 3);
            pupil2.setFillColor(sf::Color::Black);
            window.draw(pupil2);
            
            // 脚（行走动画）
            int footOffset = (static_cast<int>(enemy.animTime * 5) % 2 == 0) ? 1 : 0;
            sf::RectangleShape foot1(sf::Vector2f(4, 2));
            foot1.setPosition(enemy.x + 1, enemy.y + 12 + footOffset);
            foot1.setFillColor(sf::Color(100, 60, 30));
            window.draw(foot1);
            
            sf::RectangleShape foot2(sf::Vector2f(4, 2));
            foot2.setPosition(enemy.x + 9, enemy.y + 12 - footOffset);
            foot2.setFillColor(sf::Color(100, 60, 30));
            window.draw(foot2);
            
        } else { // 乌龟（库巴）
            // 龟壳（绿色）
            sf::RectangleShape shell(sf::Vector2f(enemy.width, 10));
            shell.setPosition(enemy.x, enemy.y + 2);
            shell.setFillColor(sf::Color(50, 180, 50));
            window.draw(shell);
            
            // 龟壳花纹
            for (int i = 0; i < 3; i++) {
                sf::RectangleShape pattern(sf::Vector2f(3, 3));
                pattern.setPosition(enemy.x + 2 + i * 4, enemy.y + 4);
                pattern.setFillColor(sf::Color(30, 130, 30));
                window.draw(pattern);
            }
            
            // 头部
            sf::RectangleShape head(sf::Vector2f(8, 6));
            head.setPosition(enemy.x + 3, enemy.y + 8);
            head.setFillColor(sf::Color(200, 180, 100));
            window.draw(head);
            
            // 眼睛
            sf::RectangleShape eye(sf::Vector2f(2, 2));
            eye.setPosition(enemy.x + 6, enemy.y + 10);
            eye.setFillColor(sf::Color::Black);
            window.draw(eye);
            
            // 高光
            sf::RectangleShape shellHighlight(sf::Vector2f(6, 3));
            shellHighlight.setPosition(enemy.x + 4, enemy.y + 3);
            shellHighlight.setFillColor(sf::Color(100, 255, 100, 150));
            window.draw(shellHighlight);
        }
    }
}

void Game3::renderCoins(sf::RenderWindow& window) {
    for (const auto& coin : coins) {
        if (coin.collected && coin.y < -10) continue; // 已飞走
        
        // 旋转动画
        float rotation = std::fmod(coin.animTime * 3.0f, 1.0f);
        float scale = std::abs(std::cos(rotation * 3.14159f));
        
        // 金币外圈
        sf::RectangleShape coinOuter(sf::Vector2f(8 * scale, 8));
        coinOuter.setPosition(coin.x + (1 - scale) * 4, coin.y);
        coinOuter.setFillColor(sf::Color(255, 215, 0));
        window.draw(coinOuter);
        
        // 金币内圈（深色）
        if (scale > 0.3f) {
            sf::RectangleShape coinInner(sf::Vector2f(4 * scale, 4));
            coinInner.setPosition(coin.x + (1 - scale) * 4 + 2, coin.y + 2);
            coinInner.setFillColor(sf::Color(200, 150, 0));
            window.draw(coinInner);
        }
        
        // 闪光效果
        if (scale > 0.8f) {
            sf::RectangleShape shine(sf::Vector2f(2, 6));
            shine.setPosition(coin.x + 3, coin.y + 1);
            shine.setFillColor(sf::Color(255, 255, 200, 200));
            window.draw(shine);
        }
    }
}

void Game3::renderItems(sf::RenderWindow& window) {
    for (const auto& item : items) {
        if (!item.active) continue;
        
        if (item.type == 0) { // 蘑菇
            // 蘑菇帽（红色带白点）
            sf::RectangleShape cap(sf::Vector2f(12, 6));
            cap.setPosition(item.x, item.y);
            cap.setFillColor(sf::Color(220, 20, 20));
            window.draw(cap);
            
            // 白色斑点
            sf::RectangleShape spot1(sf::Vector2f(3, 3));
            spot1.setPosition(item.x + 2, item.y + 1);
            spot1.setFillColor(sf::Color::White);
            window.draw(spot1);
            
            sf::RectangleShape spot2(sf::Vector2f(2, 2));
            spot2.setPosition(item.x + 8, item.y + 2);
            spot2.setFillColor(sf::Color::White);
            window.draw(spot2);
            
            // 蘑菇柄（米色）
            sf::RectangleShape stem(sf::Vector2f(6, 6));
            stem.setPosition(item.x + 3, item.y + 6);
            stem.setFillColor(sf::Color(255, 240, 200));
            window.draw(stem);
            
            // 眼睛（可爱）
            sf::RectangleShape eye1(sf::Vector2f(2, 2));
            eye1.setPosition(item.x + 3, item.y + 7);
            eye1.setFillColor(sf::Color::Black);
            window.draw(eye1);
            
            sf::RectangleShape eye2(sf::Vector2f(2, 2));
            eye2.setPosition(item.x + 7, item.y + 7);
            eye2.setFillColor(sf::Color::Black);
            window.draw(eye2);
            
        } else { // 火焰花
            // 花瓣（橙色）
            for (int i = 0; i < 4; i++) {
                float angle = i * 3.14159f / 2 + std::sin(gameTime * 3) * 0.2f;
                sf::RectangleShape petal(sf::Vector2f(4, 6));
                petal.setPosition(item.x + 4, item.y + 3);
                petal.setOrigin(2, 3);
                petal.setRotation(angle * 57.3f);
                petal.setFillColor(sf::Color(255, 150, 0));
                window.draw(petal);
            }
            
            // 花心
            sf::CircleShape center(3);
            center.setPosition(item.x + 3, item.y + 3);
            center.setFillColor(sf::Color(255, 255, 0));
            window.draw(center);
            
            // 火焰粒子效果
            float pulse = std::sin(gameTime * 10) * 0.5f + 0.5f;
            sf::CircleShape flame(2);
            flame.setPosition(item.x + 5, item.y);
            flame.setFillColor(sf::Color(255, 100, 0, static_cast<sf::Uint8>(pulse * 200)));
            window.draw(flame);
        }
    }
}

void Game3::renderParticles(sf::RenderWindow& window) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            sf::CircleShape particle(particles[i].size);
            particle.setPosition(particles[i].x, particles[i].y);
            sf::Color color = particles[i].color;
            color.a = static_cast<sf::Uint8>(255 * particles[i].life / 1.0f);
            particle.setFillColor(color);
            window.draw(particle);
        }
    }
}

void Game3::renderUI(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    
    // 分数
    sf::Text scoreText;
    scoreText.setFont(gameFont);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setCharacterSize(12);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 30);
    
    // 添加阴影
    sf::Text scoreShadow = scoreText;
    scoreShadow.setPosition(11, 31);
    scoreShadow.setFillColor(sf::Color::Black);
    window.draw(scoreShadow);
    window.draw(scoreText);
    
    // 金币数
    sf::Text coinsText;
    coinsText.setFont(gameFont);
    coinsText.setString("Coins: " + std::to_string(player.coins));
    coinsText.setCharacterSize(12);
    coinsText.setFillColor(sf::Color(255, 215, 0));
    coinsText.setPosition(10, 45);
    
    sf::Text coinsShadow = coinsText;
    coinsShadow.setPosition(11, 46);
    coinsShadow.setFillColor(sf::Color::Black);
    window.draw(coinsShadow);
    window.draw(coinsText);
    
    // 生命值（心形）
    for (int i = 0; i < player.health; i++) {
        // 简化心形（用两个方块）
        sf::RectangleShape heart1(sf::Vector2f(4, 4));
        heart1.setPosition(230 + i * 15, 33);
        heart1.setFillColor(sf::Color::Red);
        window.draw(heart1);
        
        sf::RectangleShape heart2(sf::Vector2f(4, 4));
        heart2.setPosition(234 + i * 15, 33);
        heart2.setFillColor(sf::Color::Red);
        window.draw(heart2);
        
        sf::RectangleShape heart3(sf::Vector2f(8, 4));
        heart3.setPosition(230 + i * 15, 37);
        heart3.setFillColor(sf::Color::Red);
        window.draw(heart3);
    }
    
    // 时间
    sf::Text timeText;
    timeText.setFont(gameFont);
    timeText.setString("Time: " + std::to_string(static_cast<int>(gameTime)));
    timeText.setCharacterSize(10);
    timeText.setFillColor(sf::Color::White);
    timeText.setPosition(230, 50);
    window.draw(timeText);
}