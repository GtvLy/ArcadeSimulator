#include "Game2.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

Game2::Game2() {
    fontLoaded = false;
    returnButtonInitialized = false;
    roadSegments = 0;
    
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    std::cout << "Game2 (Racing) created" << std::endl;
}

Game2::~Game2() {
    std::cout << "Game2 (Racing) destroyed" << std::endl;
}

void Game2::init() {
    std::cout << "Game2 initializing..." << std::endl;
    
    fontLoaded = gameFont.loadFromFile("../assets/fonts/PixelifySans.ttf");
    if (!fontLoaded) {
        std::cerr << "Warning: Could not load font for Game2" << std::endl;
    }
    
    reset();
    std::cout << "Game2 initialized!" << std::endl;
}

void Game2::reset() {
    std::cout << "Game2 reset" << std::endl;
    
    gameOver = false;
    gamePaused = false;
    raceStarted = false;
    score = 0;
    raceTime = 0;
    roadScroll = 0;
    roadSpeed = 100.0f;
    countdownTimer = 3.0f;
    playerRank = 1;
    
    initCars();
    initDecorations();
    
    // 初始化粒子
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = false;
    }
    
    gameClock.restart();
    animClock.restart();
}

void Game2::initCars() {
    // 玩家赛车
    playerCar.x = 160.0f;
    playerCar.y = 140.0f;
    playerCar.z = 0.0f;
    playerCar.speed = 0.0f;
    playerCar.color = sf::Color(255, 50, 50);  // 红色
    playerCar.isPlayer = true;
    playerCar.wobble = 0.0f;
    playerCar.enginePulse = 0.0f;
    
    // NPC赛车
    sf::Color npcColors[MAX_NPC_CARS] = {
        sf::Color(50, 50, 255),    // 蓝色
        sf::Color(50, 255, 50),    // 绿色
        sf::Color(255, 255, 50),   // 黄色
        sf::Color(255, 150, 50),   // 橙色
        sf::Color(200, 50, 200)    // 紫色
    };
    
    for (int i = 0; i < MAX_NPC_CARS; i++) {
        npcCars[i].x = 100.0f + (rand() % 120);
        npcCars[i].y = 140.0f;
        npcCars[i].z = 30.0f + i * 15.0f;  // 错开位置
        npcCars[i].speed = 80.0f + (rand() % 40);
        npcCars[i].targetX = 160.0f;
        npcCars[i].color = npcColors[i];
        npcCars[i].isPlayer = false;
        npcCars[i].wobble = 0.0f;
        npcCars[i].enginePulse = 0.0f;
    }
}

void Game2::initDecorations() {
    for (int i = 0; i < MAX_DECOR; i++) {
        decorations[i].y = i * 20.0f;
        decorations[i].z = 0.0f;
        decorations[i].type = rand() % 3;  // 0=树, 1=路标, 2=建筑
        decorations[i].offset = (rand() % 2 == 0) ? -1.0f : 1.0f;
    }
}

void Game2::handleInput(const sf::Event& event, sf::RenderWindow& window) {
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

void Game2::update(float deltaTime) {
    if (gameOver || gamePaused) return;
    
    // 倒计时
    if (!raceStarted) {
        countdownTimer -= deltaTime;
        if (countdownTimer <= 0) {
            raceStarted = true;
        }
        return;
    }
    
    raceTime += deltaTime;
    
    // 玩家控制
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        playerCar.x -= 150.0f * deltaTime;
        playerCar.wobble = -0.1f;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || 
               sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        playerCar.x += 150.0f * deltaTime;
        playerCar.wobble = 0.1f;
    } else {
        playerCar.wobble *= 0.9f;
    }
    
    // 限制玩家赛车在赛道内
    if (playerCar.x < 60) playerCar.x = 60;
    if (playerCar.x > 260) playerCar.x = 260;
    
    // 加速/减速
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        playerCar.speed += 200.0f * deltaTime;
        if (playerCar.speed > 200.0f) playerCar.speed = 200.0f;
        
        // 生成尾气粒子
        if (rand() % 3 == 0) {
            createParticles(playerCar.x, playerCar.y + 10, playerCar.z, 
                          sf::Color(150, 150, 150, 200), 2);
        }
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || 
               sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        playerCar.speed -= 150.0f * deltaTime;
        if (playerCar.speed < 0) playerCar.speed = 0;
    } else {
        // 自动减速
        playerCar.speed -= 50.0f * deltaTime;
        if (playerCar.speed < 60.0f) playerCar.speed = 60.0f;
    }
    
    // 更新引擎脉冲
    playerCar.enginePulse += deltaTime * 10.0f;
    
    // 更新赛道滚动
    roadScroll += (roadSpeed + playerCar.speed) * deltaTime;
    
    // 更新分数
    score = static_cast<int>(raceTime * 10);
    
    // 更新NPC和装饰
    updateNPCCars(deltaTime);
    updateDecorations(deltaTime);
    updateParticles(deltaTime);
    checkCollisions();
    calculateRanking();
    
    // 检查比赛结束（60秒）
    if (raceTime >= 60.0f) {
        gameOver = true;
    }
}

void Game2::updateNPCCars(float deltaTime) {
    for (int i = 0; i < MAX_NPC_CARS; i++) {
        Car& npc = npcCars[i];
        
        // 每隔一段时间才改变目标（减少抖动）
        if (static_cast<int>(raceTime * 2) % 3 == i % 3) {
            float centerX = 160.0f;
            float randomOffset = (rand() % 60) - 30;
            npc.targetX = centerX + randomOffset;
        }
        
        // 平滑移动
        if (npc.x < npc.targetX) {
            npc.x += 100.0f * deltaTime;
        } else if (npc.x > npc.targetX) {
            npc.x -= 100.0f * deltaTime;
        }
        
        // 限制在赛道内
        if (npc.x < 60) npc.x = 60;
        if (npc.x > 260) npc.x = 260;
        
        // 更新Z位置（相对于玩家）
        npc.z -= (playerCar.speed - npc.speed) * deltaTime;
        
        // 如果NPC超出屏幕，重新生成（只在后方生成）
        if (npc.z < -100.0f) {
            npc.z = 200.0f + (rand() % 100);  // 随机在后方生成
            npc.x = 100.0f + (rand() % 120);
            npc.speed = 80.0f + (rand() % 40);
        }
        // 删除 else if 那段，不要在前方生成

        
        // NPC之间的碰撞避让
        for (int j = 0; j < MAX_NPC_CARS; j++) {
            if (i != j) {
                float dx = npc.x - npcCars[j].x;
                float dz = npc.z - npcCars[j].z;
                if (std::abs(dx) < 20 && std::abs(dz) < 20) {
                    // 太近了，横向避让
                    if (dx > 0) npc.x += 50.0f * deltaTime;
                    else npc.x -= 50.0f * deltaTime;
                }
            }
        }
        
        // 引擎脉冲
        npc.enginePulse += deltaTime * 10.0f;
        
        // 偶尔生成尾气
        if (npc.z > -20 && npc.z < 100 && rand() % 5 == 0) {
            float scale = getPerspectiveScale(npc.z);
            createParticles(npc.x, npc.y + 10 * scale, npc.z,
                          sf::Color(150, 150, 150, 150), 1);
        }
    }
}

void Game2::updateDecorations(float deltaTime) {
    for (int i = 0; i < MAX_DECOR; i++) {
        decorations[i].y -= (roadSpeed + playerCar.speed) * deltaTime;
        
        // 循环装饰物
        if (decorations[i].y < -50) {
            decorations[i].y += MAX_DECOR * 20.0f;
            decorations[i].type = rand() % 3;
            decorations[i].offset = (rand() % 2 == 0) ? -1.0f : 1.0f;
        }
    }
}

void Game2::updateParticles(float deltaTime) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].x += particles[i].vx * deltaTime;
            particles[i].y += particles[i].vy * deltaTime;
            particles[i].z -= playerCar.speed * deltaTime * 0.5f;
            particles[i].life -= deltaTime * 2.0f;
            
            if (particles[i].life <= 0 || particles[i].z < -50) {
                particles[i].active = false;
            }
        }
    }
}

void Game2::checkCollisions() {
    // 简单的碰撞检测
    for (int i = 0; i < MAX_NPC_CARS; i++) {
        if (npcCars[i].z > -10 && npcCars[i].z < 10) {
            float dx = playerCar.x - npcCars[i].x;
            if (std::abs(dx) < 15) {
                // 碰撞！减速
                playerCar.speed *= 0.7f;
                
                // 生成火花粒子
                createParticles(playerCar.x, playerCar.y, playerCar.z,
                              sf::Color(255, 200, 0), 10);
            }
        }
    }
}

void Game2::calculateRanking() {
    int rank = 1;
    for (int i = 0; i < MAX_NPC_CARS; i++) {
        if (npcCars[i].z < playerCar.z) {
            rank++;
        }
    }
    playerRank = rank;
}

// 继续 Game2.cpp

void Game2::createParticles(float x, float y, float z, sf::Color color, int count) {
    for (int i = 0; i < MAX_PARTICLES && count > 0; i++) {
        if (!particles[i].active) {
            particles[i].x = x + (rand() % 10 - 5);
            particles[i].y = y + (rand() % 10 - 5);
            particles[i].z = z;
            particles[i].vx = (rand() % 40 - 20);
            particles[i].vy = (rand() % 40 - 20);
            particles[i].life = 1.0f;
            particles[i].color = color;
            particles[i].active = true;
            count--;
        }
    }
}

float Game2::getPerspectiveScale(float z) {
    // 透视缩放：z越大（越远）物体越小
    float distance = 100.0f + z;
    if (distance < 10.0f) distance = 10.0f;
    return 100.0f / distance;
}

sf::Color Game2::getDarkenedColor(sf::Color color, float factor) {
    return sf::Color(
        static_cast<sf::Uint8>(color.r * factor),
        static_cast<sf::Uint8>(color.g * factor),
        static_cast<sf::Uint8>(color.b * factor),
        color.a
    );
}

void Game2::render(sf::RenderWindow& window) {
    // 重置视图（必须！）
    sf::View defaultView(sf::FloatRect(0, 0, 320, 180));
    window.setView(defaultView);
    
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
    
    // 绘制天空渐变
    sf::RectangleShape sky(sf::Vector2f(320, 90));
    sky.setPosition(0, 0);
    sf::Color skyTop(135, 206, 235);    // 天蓝色
    sf::Color skyBottom(255, 200, 150); // 橙黄色
    
    // 简单的垂直渐变效果
    for (int i = 0; i < 90; i++) {
        float ratio = i / 90.0f;
        sf::Color lineColor(
            skyTop.r + (skyBottom.r - skyTop.r) * ratio,
            skyTop.g + (skyBottom.g - skyTop.g) * ratio,
            skyTop.b + (skyBottom.b - skyTop.b) * ratio
        );
        sf::RectangleShape line(sf::Vector2f(320, 1));
        line.setPosition(0, i);
        line.setFillColor(lineColor);
        window.draw(line);
    }
    
    // 绘制赛道和游戏元素
    renderRoad(window);
    renderDecorations(window);
    renderParticles(window);
    
    // 绘制NPC赛车（先绘制远的）
    std::vector<int> sortedNPCs;
    for (int i = 0; i < MAX_NPC_CARS; i++) {
        sortedNPCs.push_back(i);
    }
    std::sort(sortedNPCs.begin(), sortedNPCs.end(), [this](int a, int b) {
        return npcCars[a].z > npcCars[b].z;
    });
    
    for (int idx : sortedNPCs) {
        if (npcCars[idx].z > -20 && npcCars[idx].z < 200) {
            float scale = getPerspectiveScale(npcCars[idx].z);
            renderCar(window, npcCars[idx], scale);
        }
    }
    
    // 绘制玩家赛车（总是在最前面）
    renderCar(window, playerCar, 1.0f);
    
    // 绘制UI
    renderUI(window);
    
    // 绘制倒计时
    if (!raceStarted) {
        renderCountdown(window);
    }
    
    // 绘制游戏结束画面
    if (gameOver) {
        sf::RectangleShape overlay(sf::Vector2f(320, 180));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);
        
        if (fontLoaded) {
            sf::Text gameOverText;
            gameOverText.setFont(gameFont);
            gameOverText.setString("RACE FINISHED!");
            gameOverText.setCharacterSize(20);
            gameOverText.setFillColor(sf::Color::Yellow);
            gameOverText.setPosition(80, 50);
            window.draw(gameOverText);
            
            sf::Text rankText;
            rankText.setFont(gameFont);
            rankText.setString("Final Rank: " + std::to_string(playerRank) + "/" + std::to_string(MAX_NPC_CARS + 1));
            rankText.setCharacterSize(14);
            rankText.setFillColor(sf::Color::White);
            rankText.setPosition(90, 80);
            window.draw(rankText);
            
            sf::Text scoreText;
            scoreText.setFont(gameFont);
            scoreText.setString("Score: " + std::to_string(score));
            scoreText.setCharacterSize(14);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition(110, 100);
            window.draw(scoreText);
            
            sf::Text restartText;
            restartText.setFont(gameFont);
            restartText.setString("Press SPACE to restart");
            restartText.setCharacterSize(10);
            restartText.setFillColor(sf::Color(200, 200, 200));
            restartText.setPosition(70, 130);
            window.draw(restartText);
        }
    }
    
    // 绘制返回按钮
    if (!gameOver && returnButtonInitialized) {
        returnButton.render(window);
    }
}

void Game2::renderRoad(sf::RenderWindow& window) {
    // 绘制赛道背景
    sf::RectangleShape roadBg(sf::Vector2f(200, 90));
    roadBg.setPosition(60, 90);
    roadBg.setFillColor(sf::Color(100, 100, 100));
    window.draw(roadBg);
    
    // 左边护栏（红白相间）
    for (int i = 0; i < 5; i++) {
        sf::RectangleShape barrier(sf::Vector2f(5, 18));
        barrier.setPosition(60, 90 + i * 18);
        barrier.setFillColor((i % 2 == 0) ? sf::Color::Red : sf::Color::White);
        window.draw(barrier);
    }

    // 右边护栏
    for (int i = 0; i < 5; i++) {
        sf::RectangleShape barrier(sf::Vector2f(5, 18));
        barrier.setPosition(255, 90 + i * 18);
        barrier.setFillColor((i % 2 == 0) ? sf::Color::Red : sf::Color::White);
        window.draw(barrier);
    }
    
    // 绘制中心虚线
    int lineOffset = static_cast<int>(roadScroll) % 20;
    for (int i = -1; i < 6; i++) {
        sf::RectangleShape centerLine(sf::Vector2f(3, 10));
        centerLine.setPosition(158, 90 + i * 20 - lineOffset);
        centerLine.setFillColor(sf::Color::White);
        window.draw(centerLine);
    }
    
    // 绘制路面纹理（条纹效果）
    int stripeOffset = static_cast<int>(roadScroll * 0.5f) % 4;
    for (int i = 0; i < 25; i++) {
        if ((i + stripeOffset) % 4 < 2) {
            sf::RectangleShape stripe(sf::Vector2f(200, 4));
            stripe.setPosition(60, 90 + i * 4);
            stripe.setFillColor(sf::Color(90, 90, 90));
            window.draw(stripe);
        }
    }
}

void Game2::renderCar(sf::RenderWindow& window, const Car& car, float scale) {
    // 计算赛车位置
    float drawX = car.x + car.wobble * 3.0f;
    float drawY = car.y;
    
    // 如果不是玩家，应用透视
    if (!car.isPlayer) {
        drawY = 90 + (car.y - 90) * scale;
    }
    
    // 赛车尺寸
    float carWidth = 12.0f * scale;
    float carHeight = 20.0f * scale;
    
    // 引擎脉冲效果
    float pulse = std::sin(car.enginePulse) * 0.5f + 0.5f;
    sf::Color bodyColor = car.color;
    
    // 车身（主体）
    sf::RectangleShape body(sf::Vector2f(carWidth, carHeight));
    body.setPosition(drawX - carWidth/2, drawY - carHeight/2);
    body.setFillColor(bodyColor);
    window.draw(body);

    // 车身高光
    sf::RectangleShape highlight(sf::Vector2f(carWidth * 0.4f, carHeight * 0.5f));
    highlight.setPosition(drawX - carWidth * 0.2f, drawY - carHeight/2 + 2 * scale);
    sf::Color lightColor = bodyColor;
    lightColor.r = std::min(255, lightColor.r + 80);
    lightColor.g = std::min(255, lightColor.g + 80);
    lightColor.b = std::min(255, lightColor.b + 80);
    lightColor.a = 150;
    highlight.setFillColor(lightColor);
    window.draw(highlight);

    // 车身阴影（立体感）
    sf::RectangleShape shadow(sf::Vector2f(carWidth * 0.3f, carHeight));
    shadow.setPosition(drawX + carWidth * 0.2f, drawY - carHeight/2);
    shadow.setFillColor(sf::Color(0, 0, 0, 60));
    window.draw(shadow);
    
    // 车窗
    float windowWidth = carWidth * 0.7f;
    float windowHeight = carHeight * 0.3f;
    sf::RectangleShape carWindow(sf::Vector2f(windowWidth, windowHeight));  // 改名！
    carWindow.setPosition(drawX - windowWidth/2, drawY - carHeight/2 + 3 * scale);
    carWindow.setFillColor(sf::Color(50, 50, 100, 200));
    window.draw(carWindow);  // 用正确的window参数绘制
    
    // 车轮
    float wheelWidth = carWidth * 0.25f;
    float wheelHeight = carHeight * 0.15f;
    
    sf::RectangleShape wheelFL(sf::Vector2f(wheelWidth, wheelHeight));
    wheelFL.setPosition(drawX - carWidth/2 - wheelWidth/2, drawY - carHeight/4);
    wheelFL.setFillColor(sf::Color(30, 30, 30));
    window.draw(wheelFL);
    
    sf::RectangleShape wheelFR(sf::Vector2f(wheelWidth, wheelHeight));
    wheelFR.setPosition(drawX + carWidth/2 - wheelWidth/2, drawY - carHeight/4);
    wheelFR.setFillColor(sf::Color(30, 30, 30));
    window.draw(wheelFR);
    
    sf::RectangleShape wheelRL(sf::Vector2f(wheelWidth, wheelHeight));
    wheelRL.setPosition(drawX - carWidth/2 - wheelWidth/2, drawY + carHeight/4 - wheelHeight);
    wheelRL.setFillColor(sf::Color(30, 30, 30));
    window.draw(wheelRL);
    
    sf::RectangleShape wheelRR(sf::Vector2f(wheelWidth, wheelHeight));
    wheelRR.setPosition(drawX + carWidth/2 - wheelWidth/2, drawY + carHeight/4 - wheelHeight);
    wheelRR.setFillColor(sf::Color(30, 30, 30));
    window.draw(wheelRR);
    
    // 引擎光芒（速度效果）
    if (car.isPlayer && car.speed > 100.0f) {
        sf::RectangleShape glow(sf::Vector2f(carWidth * 1.5f, 3));
        glow.setPosition(drawX - carWidth * 0.75f, drawY + carHeight/2);
        glow.setFillColor(sf::Color(255, 150, 0, static_cast<sf::Uint8>(pulse * 150)));
        window.draw(glow);
    }
}

void Game2::renderDecorations(sf::RenderWindow& window) {
    for (int i = 0; i < MAX_DECOR; i++) {
        if (decorations[i].y > 80 && decorations[i].y < 200) {
            float scale = getPerspectiveScale(decorations[i].z);
            float drawY = 90 + (decorations[i].y - 90) * 0.5f;
            
            if (decorations[i].type == 0) {
                // 树
                float treeX = (decorations[i].offset > 0) ? 270 : 50;
                sf::CircleShape tree(5 * scale);
                tree.setPosition(treeX, drawY);
                tree.setFillColor(sf::Color(34, 139, 34));
                window.draw(tree);
            } else if (decorations[i].type == 1) {
                // 路标
                float signX = (decorations[i].offset > 0) ? 265 : 55;
                sf::RectangleShape sign(sf::Vector2f(3 * scale, 8 * scale));
                sign.setPosition(signX, drawY);
                sign.setFillColor(sf::Color(200, 200, 0));
                window.draw(sign);
            } else {
                // 建筑剪影
                float buildX = (decorations[i].offset > 0) ? 275 : 40;
                sf::RectangleShape building(sf::Vector2f(10 * scale, 15 * scale));
                building.setPosition(buildX, drawY - 15 * scale);
                building.setFillColor(sf::Color(80, 80, 100, 150));
                window.draw(building);
            }
        }
    }
}

void Game2::renderParticles(sf::RenderWindow& window) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            float scale = getPerspectiveScale(particles[i].z);
            float drawY = 90 + (particles[i].y - 90) * scale;
            
            sf::CircleShape particle(2 * scale * particles[i].life);
            particle.setPosition(particles[i].x, drawY);
            sf::Color pColor = particles[i].color;
            pColor.a = static_cast<sf::Uint8>(pColor.a * particles[i].life);
            particle.setFillColor(pColor);
            window.draw(particle);
        }
    }
}

void Game2::renderUI(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    
    // 速度表
    sf::Text speedText;
    speedText.setFont(gameFont);
    speedText.setString("Speed: " + std::to_string(static_cast<int>(playerCar.speed)));
    speedText.setCharacterSize(10);
    speedText.setFillColor(sf::Color::White);
    speedText.setPosition(10, 160);
    window.draw(speedText);
    
    // 时间
    sf::Text timeText;
    timeText.setFont(gameFont);
    timeText.setString("Time: " + std::to_string(static_cast<int>(raceTime)) + "s");
    timeText.setCharacterSize(10);
    timeText.setFillColor(sf::Color::White);
    timeText.setPosition(120, 160);
    window.draw(timeText);
    
    // 排名
    sf::Text rankText;
    rankText.setFont(gameFont);
    rankText.setString("Rank: " + std::to_string(playerRank) + "/" + std::to_string(MAX_NPC_CARS + 1));
    rankText.setCharacterSize(10);
    rankText.setFillColor(sf::Color::Yellow);
    rankText.setPosition(230, 160);
    window.draw(rankText);
    
    // 分数
    sf::Text scoreText;
    scoreText.setFont(gameFont);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setCharacterSize(10);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 30);
    window.draw(scoreText);
}

void Game2::renderCountdown(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    
    int countdown = static_cast<int>(std::ceil(countdownTimer));
    if (countdown <= 0) return;
    
    sf::Text countdownText;
    countdownText.setFont(gameFont);
    countdownText.setString(std::to_string(countdown));
    countdownText.setCharacterSize(48);
    countdownText.setFillColor(sf::Color::Yellow);
    
    // 居中显示
    sf::FloatRect textBounds = countdownText.getLocalBounds();
    countdownText.setPosition(
        160 - textBounds.width / 2,
        90 - textBounds.height / 2
    );
    
    // 脉冲效果
    float pulse = std::sin(countdownTimer * 10.0f) * 0.2f + 1.0f;
    countdownText.setScale(pulse, pulse);
    
    window.draw(countdownText);
}