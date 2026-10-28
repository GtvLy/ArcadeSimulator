#include "Game5.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>

// SnakeSoundManager 实现
void SnakeSoundManager::generateEatSound() {
    const unsigned int sampleRate = 44100;
    const unsigned int duration = 0.15f * sampleRate;
    sf::Int16* samples = new sf::Int16[duration];
    
    for (unsigned int i = 0; i < duration; i++) {
        float t = (float)i / sampleRate;
        float frequency = 800.0f - t * 400.0f;
        float amplitude = (1.0f - t / 0.15f) * 15000;
        samples[i] = (sf::Int16)(amplitude * sin(2.0f * 3.14159f * frequency * t));
    }
    
    eatBuffer.loadFromSamples(samples, duration, 1, sampleRate);
    delete[] samples;
}

void SnakeSoundManager::generatePowerUpSound() {
    const unsigned int sampleRate = 44100;
    const unsigned int duration = 0.3f * sampleRate;
    sf::Int16* samples = new sf::Int16[duration];
    
    for (unsigned int i = 0; i < duration; i++) {
        float t = (float)i / sampleRate;
        float frequency = 400.0f + t * 800.0f;
        float amplitude = (1.0f - t / 0.3f) * 12000;
        samples[i] = (sf::Int16)(amplitude * sin(2.0f * 3.14159f * frequency * t));
    }
    
    powerUpBuffer.loadFromSamples(samples, duration, 1, sampleRate);
    delete[] samples;
}

void SnakeSoundManager::generateHitSound() {
    const unsigned int sampleRate = 44100;
    const unsigned int duration = 0.2f * sampleRate;
    sf::Int16* samples = new sf::Int16[duration];
    
    for (unsigned int i = 0; i < duration; i++) {
        float t = (float)i / sampleRate;
        float noise = ((rand() % 1000) / 500.0f - 1.0f);
        float amplitude = (1.0f - t / 0.2f) * 18000;
        samples[i] = (sf::Int16)(amplitude * noise * 0.5f);
    }
    
    hitBuffer.loadFromSamples(samples, duration, 1, sampleRate);
    delete[] samples;
}

void SnakeSoundManager::generateGameOverSound() {
    const unsigned int sampleRate = 44100;
    const unsigned int duration = 0.5f * sampleRate;
    sf::Int16* samples = new sf::Int16[duration];
    
    for (unsigned int i = 0; i < duration; i++) {
        float t = (float)i / sampleRate;
        float frequency = 600.0f - t * 500.0f;
        float amplitude = (1.0f - t / 0.5f) * 20000;
        samples[i] = (sf::Int16)(amplitude * sin(2.0f * 3.14159f * frequency * t));
    }
    
    gameOverBuffer.loadFromSamples(samples, duration, 1, sampleRate);
    delete[] samples;
}

void SnakeSoundManager::generateMoveSound() {
    const unsigned int sampleRate = 44100;
    const unsigned int duration = 0.05f * sampleRate;
    sf::Int16* samples = new sf::Int16[duration];
    
    for (unsigned int i = 0; i < duration; i++) {
        float t = (float)i / sampleRate;
        float frequency = 200.0f;
        float amplitude = 8000;
        samples[i] = (sf::Int16)(amplitude * sin(2.0f * 3.14159f * frequency * t));
    }
    
    moveBuffer.loadFromSamples(samples, duration, 1, sampleRate);
    delete[] samples;
}

SnakeSoundManager::SnakeSoundManager() : soundEnabled(true), volume(50.0f) {
    generateEatSound();
    generatePowerUpSound();
    generateHitSound();
    generateGameOverSound();
    generateMoveSound();
    
    eatSound.setBuffer(eatBuffer);
    powerUpSound.setBuffer(powerUpBuffer);
    hitSound.setBuffer(hitBuffer);
    gameOverSound.setBuffer(gameOverBuffer);
    moveSound.setBuffer(moveBuffer);
    
    setVolume(volume);
}

void SnakeSoundManager::playEat() {
    if (soundEnabled) eatSound.play();
}

void SnakeSoundManager::playPowerUp() {
    if (soundEnabled) powerUpSound.play();
}

void SnakeSoundManager::playHit() {
    if (soundEnabled) hitSound.play();
}

void SnakeSoundManager::playGameOver() {
    if (soundEnabled) gameOverSound.play();
}

void SnakeSoundManager::playMove() {
    if (soundEnabled && moveSound.getStatus() != sf::Sound::Playing) {
        moveSound.play();
    }
}

void SnakeSoundManager::setVolume(float vol) {
    volume = vol;
    eatSound.setVolume(volume);
    powerUpSound.setVolume(volume);
    hitSound.setVolume(volume * 0.8f);
    gameOverSound.setVolume(volume * 0.9f);
    moveSound.setVolume(volume * 0.3f);
}

void SnakeSoundManager::toggleSound() {
    soundEnabled = !soundEnabled;
}

// Game5 实现
Game5::Game5() {
    snake = new SnakeNode[MAX_SNAKE_LENGTH];
    particles = new Particle[MAX_PARTICLES];
    tailAlpha = new float[MAX_SNAKE_LENGTH];
    enemies = new Enemy[MAX_ENEMIES];
    obstacles = new Obstacle[MAX_OBSTACLES];
    powerUps = new PowerUp[MAX_POWER_UPS];
    foods = new Food[MAX_FOODS];
    trail = new TrailNode[MAX_TRAIL];
    
    snakeColors[0] = sf::Color(147, 51, 234);
    snakeColors[1] = sf::Color(168, 85, 247);
    snakeColors[2] = sf::Color(192, 132, 252);
    snakeColors[3] = sf::Color(216, 180, 254);
    snakeColors[4] = sf::Color(233, 213, 255);
    
    fontLoaded = false;
    returnButtonInitialized = false;
    
    std::cout << "Game5 (Snake) created" << std::endl;
}

Game5::~Game5() {
    delete[] snake;
    delete[] particles;
    delete[] tailAlpha;
    delete[] enemies;
    delete[] obstacles;
    delete[] powerUps;
    delete[] foods;
    delete[] trail;
    
    std::cout << "Game5 (Snake) destroyed" << std::endl;
}

void Game5::init() {
    std::cout << "Game5 initializing..." << std::endl;
    
    // 加载字体
    fontLoaded = gameFont.loadFromFile("../assets/fonts/PixelifySans.ttf");
    if (!fontLoaded) {
        std::cerr << "Warning: Could not load font for Game5" << std::endl;
    }
    
    // 不在这里初始化按钮！
    
    reset();
    std::cout << "Game5 initialized!" << std::endl;
}

void Game5::reset() {
    std::cout << "Game5 reset" << std::endl;
    
    snakeLength = 5;
    direction = 0;
    nextDirection = 0;
    score = 0;
    gameOver = false;
    gamePaused = false;
    moveTimer = 0;
    moveDelay = 0.18f;
    gameTime = 0;
    shiftPressed = false;
    backgroundPulse = 0;
    trailIndex = 0;
    
    invincibleTimer = 0;
    speedBoostTimer = 0;
    ghostTimer = 0;
    
    for (int i = 0; i < snakeLength; i++) {
        snake[i].x = GRID_WIDTH / 2 - i;
        snake[i].y = GRID_HEIGHT / 2;
        tailAlpha[i] = 1.0f;
    }
    
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = false;
    }
    
    for (int i = 0; i < MAX_TRAIL; i++) {
        trail[i].life = 0;
    }
    
    initEnemies();
    initObstacles();
    initPowerUps();
    spawnAllFoods();
}

void Game5::initEnemies() {
    enemyCount = 3;  // 减少敌人数量以适应小屏幕
    for (int i = 0; i < enemyCount; i++) {
        enemies[i].x = (rand() % GRID_WIDTH) + 0.5f;
        enemies[i].y = (rand() % GRID_HEIGHT) + 0.5f;
        enemies[i].vx = ((rand() % 100) / 100.0f - 0.5f) * 0.05f;
        enemies[i].vy = ((rand() % 100) / 100.0f - 0.5f) * 0.05f;
        enemies[i].angle = 0;
        enemies[i].active = true;
        enemies[i].type = i % 2;
        enemies[i].stateTimer = 0;
        enemies[i].glowPulse = (rand() % 100) / 100.0f * 3.14159f;
    }
}

void Game5::initObstacles() {
    obstacleCount = 8;  // 减少障碍物数量
    
    for (int i = 0; i < obstacleCount; i++) {
        bool validPos = false;
        int attempts = 0;
        
        while (!validPos && attempts < 50) {
            int gridX = (i % 3) * (GRID_WIDTH / 3);
            int gridY = (i / 3) * (GRID_HEIGHT / 3);
            
            obstacles[i].x = gridX + (rand() % (GRID_WIDTH / 3));
            obstacles[i].y = gridY + (rand() % (GRID_HEIGHT / 3));
            
            validPos = true;
            
            int dist = abs(obstacles[i].x - GRID_WIDTH/2) + abs(obstacles[i].y - GRID_HEIGHT/2);
            if (dist < 5) validPos = false;
            
            for (int j = 0; j < i; j++) {
                int dx = abs(obstacles[i].x - obstacles[j].x);
                int dy = abs(obstacles[i].y - obstacles[j].y);
                if (dx + dy < 3) {
                    validPos = false;
                    break;
                }
            }
            
            attempts++;
        }
        
        obstacles[i].type = i % 3;
        obstacles[i].rotation = (rand() % 360);
        obstacles[i].active = true;
        obstacles[i].pulseTimer = (rand() % 100) / 100.0f * 3.14159f;
    }
}

void Game5::initPowerUps() {
    for (int i = 0; i < MAX_POWER_UPS; i++) {
        powerUps[i].active = false;
        powerUps[i].spawnTimer = 5.0f + i * 3.0f;
        powerUps[i].type = i % 3;
        powerUps[i].rotationAngle = 0;
    }
}

void Game5::spawnAllFoods() {
    for (int i = 0; i < MAX_FOODS; i++) {
        spawnFood(i);
    }
}

void Game5::spawnFood(int index) {
    bool validPosition = false;
    int attempts = 0;
    while (!validPosition && attempts < 100) {
        foods[index].x = rand() % GRID_WIDTH;
        foods[index].y = rand() % GRID_HEIGHT;
        validPosition = true;
        
        for (int i = 0; i < snakeLength; i++) {
            if (snake[i].x == foods[index].x && snake[i].y == foods[index].y) {
                validPosition = false;
                break;
            }
        }
        
        for (int i = 0; i < obstacleCount; i++) {
            if (obstacles[i].active && obstacles[i].x == foods[index].x && obstacles[i].y == foods[index].y) {
                validPosition = false;
                break;
            }
        }
        
        attempts++;
    }
    foods[index].active = true;
    foods[index].pulseTimer = 0;
}

void Game5::spawnPowerUp(int index) {
    bool validPos = false;
    int attempts = 0;
    while (!validPos && attempts < 100) {
        powerUps[index].x = rand() % GRID_WIDTH;
        powerUps[index].y = rand() % GRID_HEIGHT;
        validPos = true;
        
        for (int i = 0; i < snakeLength; i++) {
            if (snake[i].x == powerUps[index].x && snake[i].y == powerUps[index].y) {
                validPos = false;
                break;
            }
        }
        
        attempts++;
    }
    powerUps[index].active = true;
}

void Game5::collectPowerUp(int index) {
    score += 20;
    soundManager.playPowerUp();
    createParticles(powerUps[index].x * CELL_SIZE + CELL_SIZE/2,
                   powerUps[index].y * CELL_SIZE + CELL_SIZE/2,
                   sf::Color(100, 255, 255), 30);
    
    if (powerUps[index].type == 0) speedBoostTimer = 5.0f;
    else if (powerUps[index].type == 1) invincibleTimer = 5.0f;
    else if (powerUps[index].type == 2) ghostTimer = 5.0f;
    
    powerUps[index].active = false;
    powerUps[index].spawnTimer = 10.0f + (rand() % 50) / 10.0f;
}

void Game5::addEnemy() {
    if (enemyCount < MAX_ENEMIES) {
        enemies[enemyCount].x = (rand() % GRID_WIDTH) + 0.5f;
        enemies[enemyCount].y = (rand() % GRID_HEIGHT) + 0.5f;
        enemies[enemyCount].vx = ((rand() % 100) / 100.0f - 0.5f) * 0.05f;
        enemies[enemyCount].vy = ((rand() % 100) / 100.0f - 0.5f) * 0.05f;
        enemies[enemyCount].angle = 0;
        enemies[enemyCount].active = true;
        enemies[enemyCount].type = rand() % 2;
        enemies[enemyCount].stateTimer = 0;
        enemies[enemyCount].glowPulse = 0;
        enemyCount++;
    }
}

void Game5::createParticles(float x, float y, sf::Color color, int count) {
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < MAX_PARTICLES; j++) {
            if (!particles[j].active) {
                float angle = (rand() % 360) * 3.14159f / 180.0f;
                float speed = 1.0f + (rand() % 100) / 50.0f;
                
                particles[j].x = x;
                particles[j].y = y;
                particles[j].vx = cos(angle) * speed;
                particles[j].vy = sin(angle) * speed;
                particles[j].life = 1.0f;
                particles[j].size = 1.5f + (rand() % 100) / 100.0f;
                particles[j].color = color;
                particles[j].active = true;
                break;
            }
        }
    }
}

void Game5::handleInput(const sf::Event& event, sf::RenderWindow& window) {
    // 获取鼠标位置
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
    
    // 始终处理返回按钮
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
    
    if (event.type == sf::Event::KeyPressed) {
        // ESC 键暂停（返回场景三）
        if (event.key.code == sf::Keyboard::Escape) {
            gamePaused = true;
            std::cout << "Game5 paused (ESC pressed)" << std::endl;
            return;
        }
        
        if (gameOver) {
            if (event.key.code == sf::Keyboard::Space) {
                reset();
            }
            return;
        }
        
        // M键切换音效
        if (event.key.code == sf::Keyboard::M) {
            soundManager.toggleSound();
        }
        
        // 方向控制
        if (event.key.code == sf::Keyboard::Up && direction != 1) {
            nextDirection = 3;
        } else if (event.key.code == sf::Keyboard::Down && direction != 3) {
            nextDirection = 1;
        } else if (event.key.code == sf::Keyboard::Left && direction != 0) {
            nextDirection = 2;
        } else if (event.key.code == sf::Keyboard::Right && direction != 2) {
            nextDirection = 0;
        }
        
        // Shift 加速
        if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift) {
            shiftPressed = true;
        }
    }
    
    if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift) {
            shiftPressed = false;
        }
    }
}

void Game5::update(float deltaTime) {
    if (gameOver || gamePaused) return;
    
    gameTime += deltaTime;
    moveTimer += deltaTime;
    backgroundPulse += deltaTime;
    
    if (invincibleTimer > 0) invincibleTimer -= deltaTime;
    if (speedBoostTimer > 0) speedBoostTimer -= deltaTime;
    if (ghostTimer > 0) ghostTimer -= deltaTime;
    
    // 更新粒子
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].x += particles[i].vx;
            particles[i].y += particles[i].vy;
            particles[i].vy += 0.1f;
            particles[i].vx *= 0.98f;
            particles[i].life -= deltaTime * 1.2f;
            
            if (particles[i].life <= 0) {
                particles[i].active = false;
            }
        }
    }
    
    // 更新轨迹
    for (int i = 0; i < MAX_TRAIL; i++) {
        if (trail[i].life > 0) {
            trail[i].life -= deltaTime * 2.0f;
        }
    }
    
    // 更新食物脉冲
    for (int i = 0; i < MAX_FOODS; i++) {
        if (foods[i].active) {
            foods[i].pulseTimer += deltaTime * 3.0f;
        }
    }
    
    // 更新障碍物
    for (int i = 0; i < obstacleCount; i++) {
        if (obstacles[i].active) {
            obstacles[i].pulseTimer += deltaTime * 2.0f;
            if (obstacles[i].type == 1) {
                obstacles[i].rotation += deltaTime * 40.0f;
            } else if (obstacles[i].type == 2) {
                obstacles[i].rotation += deltaTime * 60.0f;
            }
        }
    }
    
    // 更新敌人
    for (int i = 0; i < enemyCount; i++) {
        if (!enemies[i].active) continue;
        
        enemies[i].stateTimer += deltaTime;
        enemies[i].glowPulse += deltaTime * 3.0f;
        
        if (enemies[i].type == 0) {
            enemies[i].x += enemies[i].vx;
            enemies[i].y += enemies[i].vy;
            
            if (enemies[i].x < 0 || enemies[i].x > GRID_WIDTH) enemies[i].vx *= -1;
            if (enemies[i].y < 0 || enemies[i].y > GRID_HEIGHT) enemies[i].vy *= -1;
            
            if (enemies[i].stateTimer > 3.0f) {
                enemies[i].vx = ((rand() % 100) / 100.0f - 0.5f) * 0.08f;
                enemies[i].vy = ((rand() % 100) / 100.0f - 0.5f) * 0.08f;
                enemies[i].stateTimer = 0;
            }
        } else {
            float dx = snake[0].x - enemies[i].x;
            float dy = snake[0].y - enemies[i].y;
            float dist = sqrt(dx*dx + dy*dy);
            
            if (dist > 0.1f && dist < 12.0f) {
                enemies[i].vx = (dx / dist) * 0.04f;
                enemies[i].vy = (dy / dist) * 0.04f;
            } else {
                enemies[i].vx *= 0.95f;
                enemies[i].vy *= 0.95f;
            }
            
            enemies[i].x += enemies[i].vx;
            enemies[i].y += enemies[i].vy;
        }
        
        enemies[i].angle += deltaTime * 2.0f;
        
        // 蛇身撞击敌人
        if (invincibleTimer <= 0) {
            for (int j = 1; j < snakeLength; j++) {
                float dx = snake[j].x - enemies[i].x;
                float dy = snake[j].y - enemies[i].y;
                if (sqrt(dx*dx + dy*dy) < 0.7f) {
                    score += 15;
                    soundManager.playHit();
                    createParticles(enemies[i].x * CELL_SIZE,
                                  enemies[i].y * CELL_SIZE,
                                  sf::Color(255, 215, 100), 30);
                    enemies[i].active = false;
                    break;
                }
            }
        }
    }
    
    // 更新道具
    for (int i = 0; i < MAX_POWER_UPS; i++) {
        if (powerUps[i].active) {
            powerUps[i].rotationAngle += deltaTime * 100.0f;
        } else {
            powerUps[i].spawnTimer -= deltaTime;
            if (powerUps[i].spawnTimer <= 0) {
                spawnPowerUp(i);
            }
        }
    }
    
    // 蛇移动逻辑
    float currentSpeed = moveDelay;
    if (shiftPressed) {
        currentSpeed *= 0.5f;
    } else if (speedBoostTimer > 0) {
        currentSpeed *= 0.6f;
    }
    
    if (moveTimer >= currentSpeed) {
        moveTimer = 0;
        direction = nextDirection;
        
        soundManager.playMove();
        
        // 添加轨迹
        trail[trailIndex].x = snake[0].x * CELL_SIZE + CELL_SIZE / 2;
        trail[trailIndex].y = snake[0].y * CELL_SIZE + CELL_SIZE / 2;
        trail[trailIndex].life = 1.0f;
        trailIndex = (trailIndex + 1) % MAX_TRAIL;
        
        // 移动蛇身
        for (int i = snakeLength - 1; i > 0; i--) {
            snake[i] = snake[i - 1];
        }
        
        // 移动蛇头
        if (direction == 0) snake[0].x++;
        else if (direction == 1) snake[0].y++;
        else if (direction == 2) snake[0].x--;
        else if (direction == 3) snake[0].y--;
        
        // 边界处理
        if (ghostTimer > 0) {
            if (snake[0].x < 0) snake[0].x = GRID_WIDTH - 1;
            if (snake[0].x >= GRID_WIDTH) snake[0].x = 0;
            if (snake[0].y < 0) snake[0].y = GRID_HEIGHT - 1;
            if (snake[0].y >= GRID_HEIGHT) snake[0].y = 0;
        } else {
            if (snake[0].x < 0 || snake[0].x >= GRID_WIDTH ||
                snake[0].y < 0 || snake[0].y >= GRID_HEIGHT) {
                gameOver = true;
                soundManager.playGameOver();
                createParticles(snake[0].x * CELL_SIZE + CELL_SIZE/2, 
                              snake[0].y * CELL_SIZE + CELL_SIZE/2, 
                              sf::Color(255, 100, 150), 40);
                return;
            }
        }
        
        // 撞自己
        if (invincibleTimer <= 0) {
            for (int i = 1; i < snakeLength; i++) {
                if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                    gameOver = true;
                    soundManager.playGameOver();
                    createParticles(snake[0].x * CELL_SIZE + CELL_SIZE/2,
                                  snake[0].y * CELL_SIZE + CELL_SIZE/2,
                                  sf::Color(255, 100, 150), 40);
                    return;
                }
            }
        }
        
        // 撞障碍物
        if (ghostTimer <= 0) {
            for (int i = 0; i < obstacleCount; i++) {
                if (obstacles[i].active && 
                    obstacles[i].x == snake[0].x && 
                    obstacles[i].y == snake[0].y) {
                    gameOver = true;
                    soundManager.playGameOver();
                    createParticles(snake[0].x * CELL_SIZE + CELL_SIZE/2,
                                  snake[0].y * CELL_SIZE + CELL_SIZE/2,
                                  sf::Color(255, 150, 0), 40);
                    return;
                }
            }
        }
        
        // 撞敌人
        if (invincibleTimer <= 0) {
            for (int i = 0; i < enemyCount; i++) {
                if (enemies[i].active) {
                    float dx = snake[0].x - enemies[i].x;
                    float dy = snake[0].y - enemies[i].y;
                    if (sqrt(dx*dx + dy*dy) < 0.8f) {
                        gameOver = true;
                        soundManager.playGameOver();
                        createParticles(enemies[i].x * CELL_SIZE,
                                      enemies[i].y * CELL_SIZE,
                                      sf::Color(255, 50, 100), 50);
                        return;
                    }
                }
            }
        }
        
        // 收集道具
        for (int i = 0; i < MAX_POWER_UPS; i++) {
            if (powerUps[i].active && 
                powerUps[i].x == snake[0].x && 
                powerUps[i].y == snake[0].y) {
                collectPowerUp(i);
            }
        }
        
        // 吃食物
        for (int i = 0; i < MAX_FOODS; i++) {
            if (foods[i].active && snake[0].x == foods[i].x && snake[0].y == foods[i].y) {
                score += 10;
                snakeLength++;
                
                soundManager.playEat();
                
                if (snakeLength < MAX_SNAKE_LENGTH) {
                    snake[snakeLength - 1] = snake[snakeLength - 2];
                }
                
                createParticles(foods[i].x * CELL_SIZE + CELL_SIZE/2,
                              foods[i].y * CELL_SIZE + CELL_SIZE/2,
                              sf::Color(255, 215, 0), 25);
                spawnFood(i);
                
                if (moveDelay > 0.06f) {
                    moveDelay -= 0.002f;
                }
                
                if (score % 50 == 0 && enemyCount < MAX_ENEMIES) {
                    addEnemy();
                }
                break;
            }
        }
    }
    
    // 更新尾巴透明度
    for (int i = 0; i < snakeLength; i++) {
        float targetAlpha = 1.0f - (i / (float)snakeLength) * 0.4f;
        tailAlpha[i] += (targetAlpha - tailAlpha[i]) * 0.15f;
    }
}

void Game5::render(sf::RenderWindow& window) {
    int VIEW_WIDTH = 320;
    int VIEW_HEIGHT = 180;
    
    // 动态渐变背景
    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setPosition(i * CELL_SIZE, j * CELL_SIZE);
            
            float wave = sin(backgroundPulse * 0.5f + i * 0.1f + j * 0.1f) * 5.0f;
            int baseColor = 18 + wave;
            int colorVariation = ((i + j) % 2) * 3;
            
            cell.setFillColor(sf::Color(
                baseColor + colorVariation,
                baseColor + colorVariation, 
                baseColor + colorVariation + 8
            ));
            window.draw(cell);
        }
    }
    
    // 绘制轨迹效果
    for (int i = 0; i < MAX_TRAIL; i++) {
        if (trail[i].life > 0) {
            sf::CircleShape trailDot(2);
            trailDot.setPosition(trail[i].x - 2, trail[i].y - 2);
            sf::Color trailColor = snakeColors[0];
            trailColor.a = (sf::Uint8)(trail[i].life * 80);
            trailDot.setFillColor(trailColor);
            window.draw(trailDot);
        }
    }
    
    // 绘制障碍物(带发光效果)
    for (int i = 0; i < obstacleCount; i++) {
        if (!obstacles[i].active) continue;
        
        float pulse = sin(obstacles[i].pulseTimer) * 0.3f + 0.7f;
        
        // 外发光
        for (int glow = 2; glow > 0; glow--) {
            sf::RectangleShape glowRect(sf::Vector2f(CELL_SIZE + glow * 3, CELL_SIZE + glow * 3));
            glowRect.setPosition(obstacles[i].x * CELL_SIZE - glow * 1.5f, 
                                obstacles[i].y * CELL_SIZE - glow * 1.5f);
            
            int alpha = (int)(25 * pulse / glow);
            if (obstacles[i].type == 0) {
                glowRect.setFillColor(sf::Color(100, 100, 120, alpha));
            } else if (obstacles[i].type == 1) {
                glowRect.setFillColor(sf::Color(150, 80, 80, alpha));
            } else {
                glowRect.setFillColor(sf::Color(80, 120, 150, alpha));
            }
            
            glowRect.setOrigin((CELL_SIZE + glow * 3) / 2, (CELL_SIZE + glow * 3) / 2);
            glowRect.setPosition(obstacles[i].x * CELL_SIZE + CELL_SIZE/2, 
                                obstacles[i].y * CELL_SIZE + CELL_SIZE/2);
            glowRect.setRotation(obstacles[i].rotation);
            window.draw(glowRect);
        }
        
        // 主体
        sf::RectangleShape obs(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
        
        if (obstacles[i].type == 0) {
            obs.setFillColor(sf::Color(100 * pulse, 100 * pulse, 120 * pulse));
        } else if (obstacles[i].type == 1) {
            obs.setFillColor(sf::Color(150 * pulse, 70 * pulse, 70 * pulse));
        } else {
            obs.setFillColor(sf::Color(70 * pulse, 120 * pulse, 150 * pulse));
        }
        
        obs.setOrigin((CELL_SIZE - 2) / 2, (CELL_SIZE - 2) / 2);
        obs.setPosition(obstacles[i].x * CELL_SIZE + CELL_SIZE/2, 
                       obstacles[i].y * CELL_SIZE + CELL_SIZE/2);
        obs.setRotation(obstacles[i].rotation);
        window.draw(obs);
    }
    
    // 绘制道具(旋转发光)
    for (int i = 0; i < MAX_POWER_UPS; i++) {
        if (!powerUps[i].active) continue;
        
        // 外圈发光
        for (int ring = 3; ring > 0; ring--) {
            sf::CircleShape powerGlow(CELL_SIZE / 2 + ring * 2);
            powerGlow.setPosition(powerUps[i].x * CELL_SIZE - ring * 2, 
                                 powerUps[i].y * CELL_SIZE - ring * 2);
            int alpha = 30 / ring;
            
            if (powerUps[i].type == 0) {
                powerGlow.setFillColor(sf::Color(0, 255, 255, alpha));
            } else if (powerUps[i].type == 1) {
                powerGlow.setFillColor(sf::Color(255, 215, 0, alpha));
            } else {
                powerGlow.setFillColor(sf::Color(100, 255, 100, alpha));
            }
            window.draw(powerGlow);
        }
        
        // 旋转星形
        sf::CircleShape powerUp(CELL_SIZE / 2 - 1, 5);
        powerUp.setOrigin(CELL_SIZE / 2 - 1, CELL_SIZE / 2 - 1);
        powerUp.setPosition(powerUps[i].x * CELL_SIZE + CELL_SIZE/2, 
                           powerUps[i].y * CELL_SIZE + CELL_SIZE/2);
        powerUp.setRotation(powerUps[i].rotationAngle);
        
        if (powerUps[i].type == 0) {
            powerUp.setFillColor(sf::Color(0, 255, 255));
        } else if (powerUps[i].type == 1) {
            powerUp.setFillColor(sf::Color(255, 215, 0));
        } else {
            powerUp.setFillColor(sf::Color(100, 255, 100));
        }
        window.draw(powerUp);
    }
    
    // 绘制敌人(发光球体)
    for (int i = 0; i < enemyCount; i++) {
        if (!enemies[i].active) continue;
        
        float glowIntensity = sin(enemies[i].glowPulse) * 0.3f + 0.7f;
        
        // 多层发光
        for (int ring = 4; ring > 0; ring--) {
            sf::CircleShape enemyGlow(CELL_SIZE / 2 + ring * 1.5f);
            enemyGlow.setPosition(enemies[i].x * CELL_SIZE - CELL_SIZE/2 - ring * 1.5f, 
                                 enemies[i].y * CELL_SIZE - CELL_SIZE/2 - ring * 1.5f);
            int alpha = (int)(40 * glowIntensity / ring);
            
            if (enemies[i].type == 0) {
                enemyGlow.setFillColor(sf::Color(255, 50, 50, alpha));
            } else {
                enemyGlow.setFillColor(sf::Color(255, 120, 0, alpha));
            }
            window.draw(enemyGlow);
        }
        
        // 敌人本体
        sf::CircleShape enemy(CELL_SIZE / 2);
        enemy.setPosition(enemies[i].x * CELL_SIZE - CELL_SIZE/2, 
                        enemies[i].y * CELL_SIZE - CELL_SIZE/2);
        
        if (enemies[i].type == 0) {
            enemy.setFillColor(sf::Color(255, 80, 80));
        } else {
            enemy.setFillColor(sf::Color(255, 140, 60));
        }
        window.draw(enemy);
        
        // 发光眼睛
        sf::CircleShape eye(2);
        eye.setFillColor(sf::Color(255, 255, 255, 200));
        float eyeX = enemies[i].x * CELL_SIZE + cos(enemies[i].angle) * 3;
        float eyeY = enemies[i].y * CELL_SIZE + sin(enemies[i].angle) * 3;
        eye.setPosition(eyeX - 2, eyeY - 2);
        window.draw(eye);
    }
    
    // 绘制食物(脉冲发光)
    for (int f = 0; f < MAX_FOODS; f++) {
        if (!foods[f].active) continue;
        
        float pulse = sin(foods[f].pulseTimer) * 0.4f + 0.8f;
        
        // 多层光晕
        for (int ring = 4; ring >= 0; ring--) {
            sf::CircleShape foodGlow(CELL_SIZE / 2 + ring * 2);
            foodGlow.setPosition(foods[f].x * CELL_SIZE - ring * 2, 
                                foods[f].y * CELL_SIZE - ring * 2);
            int alpha = (int)(50 * pulse / (ring + 1));
            foodGlow.setFillColor(sf::Color(255, 215, 0, alpha));
            window.draw(foodGlow);
        }
        
        // 食物核心
        sf::CircleShape foodCore(CELL_SIZE / 2 - 1);
        foodCore.setPosition(foods[f].x * CELL_SIZE + 1, foods[f].y * CELL_SIZE + 1);
        foodCore.setFillColor(sf::Color(255, 230, 100));
        window.draw(foodCore);
        
        // 内部高光
        sf::CircleShape highlight(CELL_SIZE / 4);
        highlight.setPosition(foods[f].x * CELL_SIZE + 2, foods[f].y * CELL_SIZE + 2);
        highlight.setFillColor(sf::Color(255, 255, 200, 180));
        window.draw(highlight);
    }
    
    // 绘制蛇(渐变+发光)
    for (int i = snakeLength - 1; i >= 0; i--) {
        int colorIndex = (i % 5);
        sf::Color segmentColor = snakeColors[colorIndex];
        
        // 道具效果颜色
        if (invincibleTimer > 0) {
            float phase = gameTime * 10.0f + i * 0.3f;
            segmentColor = sf::Color(
                (sf::Uint8)(200 + sin(phase) * 55),
                (sf::Uint8)(180 + sin(phase + 2) * 75),
                (sf::Uint8)(0),
                (sf::Uint8)(tailAlpha[i] * 255)
            );
        } else if (ghostTimer > 0) {
            segmentColor = sf::Color(100, 255, 150, (sf::Uint8)(tailAlpha[i] * 180));
        } else if (speedBoostTimer > 0) {
            segmentColor = sf::Color(100, 200, 255, (sf::Uint8)(tailAlpha[i] * 255));
        } else {
            segmentColor.a = (sf::Uint8)(tailAlpha[i] * 255);
        }
        
        // 外发光
        if (i < 3 || invincibleTimer > 0) {
            for (int glow = 2; glow > 0; glow--) {
                float glowSize = CELL_SIZE + glow * 3;
                sf::RectangleShape segmentGlow(sf::Vector2f(glowSize, glowSize));
                float glowOffset = (CELL_SIZE - glowSize) / 2;
                segmentGlow.setPosition(snake[i].x * CELL_SIZE + glowOffset, 
                                       snake[i].y * CELL_SIZE + glowOffset);
                sf::Color glowColor = segmentColor;
                glowColor.a = (sf::Uint8)(30 / glow);
                segmentGlow.setFillColor(glowColor);
                window.draw(segmentGlow);
            }
        }
        
        // 蛇身主体
        float size = CELL_SIZE - 1 - (i / (float)snakeLength) * 2;
        sf::RectangleShape segment(sf::Vector2f(size, size));
        float offset = (CELL_SIZE - size) / 2;
        segment.setPosition(snake[i].x * CELL_SIZE + offset, 
                          snake[i].y * CELL_SIZE + offset);
        segment.setFillColor(segmentColor);
        window.draw(segment);
        
        // 蛇头特殊效果
        if (i == 0) {
            sf::CircleShape eye(1.5f);
            eye.setFillColor(sf::Color(255, 255, 255, 240));
            
            if (direction == 0 || direction == 2) {
                eye.setPosition(snake[0].x * CELL_SIZE + 2, snake[0].y * CELL_SIZE + 1);
                window.draw(eye);
                eye.setPosition(snake[0].x * CELL_SIZE + 2, snake[0].y * CELL_SIZE + 4);
                window.draw(eye);
            } else {
                eye.setPosition(snake[0].x * CELL_SIZE + 1, snake[0].y * CELL_SIZE + 2);
                window.draw(eye);
                eye.setPosition(snake[0].x * CELL_SIZE + 4, snake[0].y * CELL_SIZE + 2);
                window.draw(eye);
            }
        }
    }
    
    // 绘制粒子
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            // 粒子光晕
            sf::CircleShape particleGlow(particles[i].size + 1);
            particleGlow.setPosition(particles[i].x - particles[i].size - 1, 
                                    particles[i].y - particles[i].size - 1);
            sf::Color glowColor = particles[i].color;
            glowColor.a = (sf::Uint8)(particles[i].life * 80);
            particleGlow.setFillColor(glowColor);
            window.draw(particleGlow);
            
            // 粒子核心
            sf::CircleShape particle(particles[i].size);
            particle.setPosition(particles[i].x - particles[i].size, 
                                particles[i].y - particles[i].size);
            sf::Color pColor = particles[i].color;
            pColor.a = (sf::Uint8)(particles[i].life * 255);
            particle.setFillColor(pColor);
            window.draw(particle);
        }
    }
    
    // 延迟初始化返回按钮（第一次渲染时）
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
                std::cout << "Return button clicked in Game5" << std::endl;
                gamePaused = true;
            });
            returnButtonInitialized = true;
            std::cout << "Return button initialized in Game5" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize return button: " << e.what() << std::endl;
        }
    }
    
    // 绘制返回按钮（在所有游戏元素之后，UI之前）
    if (!gameOver && returnButtonInitialized) {
        returnButton.render(window);
    }
    
    // UI栏(渐变背景)
    int UI_HEIGHT = 24;
    for (int i = 0; i < UI_HEIGHT; i++) {
        sf::RectangleShape gradientLine(sf::Vector2f(VIEW_WIDTH, 1));
        gradientLine.setPosition(0, VIEW_HEIGHT - UI_HEIGHT + i);
        int colorVal = 25 + i / 2;
        gradientLine.setFillColor(sf::Color(colorVal, colorVal, colorVal + 5));
        window.draw(gradientLine);
    }
    
    if (fontLoaded) {
    // 分数显示（增大字体）
    char scoreText[100];
    sprintf(scoreText, "SCORE:%d L:%d E:%d", score, snakeLength, enemyCount);
    
    // 阴影
    sf::Text scoreShadow(scoreText, gameFont, 10);  // 从8改为10
    scoreShadow.setFillColor(sf::Color(0, 0, 0, 150));
    scoreShadow.setPosition(3, VIEW_HEIGHT - 21);
    window.draw(scoreShadow);
    
    // 主文字
    sf::Text scoreDisplay(scoreText, gameFont, 10);  // 从8改为10
    scoreDisplay.setFillColor(sf::Color(180, 120, 255));
    scoreDisplay.setPosition(2, VIEW_HEIGHT - 22);
    window.draw(scoreDisplay);
    
    // 状态显示（增大字体）
    char statusText[100];
    statusText[0] = '\0';
    sf::Color statusColor;
    
    if (invincibleTimer > 0) {
        sprintf(statusText, "INVINCIBLE %.1fs", invincibleTimer);
        statusColor = sf::Color(255, 215, 0);
    } else if (speedBoostTimer > 0) {
        sprintf(statusText, "SPEED %.1fs", speedBoostTimer);
        statusColor = sf::Color(100, 200, 255);
    } else if (ghostTimer > 0) {
        sprintf(statusText, "GHOST %.1fs", ghostTimer);
        statusColor = sf::Color(100, 255, 150);
    } else if (shiftPressed) {
        sprintf(statusText, "SHIFT BOOST");
        statusColor = sf::Color(150, 220, 255);
    } else {
        sprintf(statusText, "SHIFT:Speed M:Sound Q:Exit");  // 改了提示
        statusColor = sf::Color(100, 100, 120);
    }
    
    if (statusText[0] != '\0') {
        // 阴影
        sf::Text statusShadow(statusText, gameFont, 8);  // 从7改为8
        statusShadow.setFillColor(sf::Color(0, 0, 0, 150));
        statusShadow.setPosition(3, VIEW_HEIGHT - 11);
        window.draw(statusShadow);
        
        // 主文字
        sf::Text status(statusText, gameFont, 8);  // 从7改为8
        status.setFillColor(statusColor);
        status.setPosition(2, VIEW_HEIGHT - 12);
        window.draw(status);
    }
    }
    
    // 游戏结束界面
    if (gameOver) {
        // 半透明遮罩
        sf::RectangleShape overlay(sf::Vector2f(VIEW_WIDTH, VIEW_HEIGHT - UI_HEIGHT));
        overlay.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(overlay);
        
        if (fontLoaded) {
        // GAME OVER 文字（增大）
        sf::Text gameOverText("GAME OVER", gameFont, 24);  // 从20改为24
        gameOverText.setFillColor(sf::Color(255, 100, 150));
        gameOverText.setPosition(VIEW_WIDTH / 2 - 60, VIEW_HEIGHT / 2 - 35);
        window.draw(gameOverText);
        
        // 最终分数（增大）
        char finalScore[100];
        sprintf(finalScore, "Score: %d", score);
        
        sf::Text finalText(finalScore, gameFont, 14);  // 从12改为14
        finalText.setFillColor(sf::Color(255, 255, 255));
        finalText.setPosition(VIEW_WIDTH / 2 - 40, VIEW_HEIGHT / 2 - 5);
        window.draw(finalText);
        
        // 重启提示（增大）
        sf::Text restartText("SPACE to restart", gameFont, 10);  // 从8改为10
        restartText.setFillColor(sf::Color(180, 180, 200));
        restartText.setPosition(VIEW_WIDTH / 2 - 50, VIEW_HEIGHT / 2 + 15);
        window.draw(restartText);
        
        // Q键提示（增大）
        sf::Text escText("Q to exit", gameFont, 10);  // 从8改为10，文字也改了
        escText.setFillColor(sf::Color(180, 180, 200));
        escText.setPosition(VIEW_WIDTH / 2 - 30, VIEW_HEIGHT / 2 + 28);
        window.draw(escText);
        }
    }
}