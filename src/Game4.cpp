#include "Game4.h"
#include <iostream>
#include <cmath>

Game4::Game4() {
    fontLoaded = false;
    returnButtonInitialized = false;
    
    // 初始化地图
    int map[16][16] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,0,1,0,1,1,1,1,1,1,0,1},
        {1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1},
        {1,0,1,0,1,1,1,1,1,1,1,1,0,1,0,1},
        {1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1},
        {1,1,1,0,1,0,1,1,1,1,0,1,1,1,0,1},
        {1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,0,0,1,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
        {1,1,1,1,1,0,1,1,1,1,1,1,0,1,0,1},
        {1,0,0,0,1,0,0,0,0,0,0,1,0,1,0,1},
        {1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,1},
        {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,1,1,1,1,1,2,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
    
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            worldMap[y][x] = map[y][x];
        }
    }
    
    std::cout << "Game4 (3D Maze) created" << std::endl;
}

Game4::~Game4() {
    std::cout << "Game4 (3D Maze) destroyed" << std::endl;
}

void Game4::init() {
    std::cout << "Game4 initializing..." << std::endl;
    
    // 加载字体
    fontLoaded = gameFont.loadFromFile("../assets/fonts/PixelifySans.ttf");
    if (!fontLoaded) {
        std::cerr << "Warning: Could not load font for Game4" << std::endl;
    }
    
    reset();
    std::cout << "Game4 initialized!" << std::endl;
}

void Game4::reset() {
    std::cout << "Game4 reset" << std::endl;
    
    player.x = 1.5f;
    player.y = 1.5f;
    player.angle = 0;
    player.moveSpeed = 0.05f;
    player.rotSpeed = 0.03f;
    
    completionTime = 0.0f;
    gameWon = false;
    gamePaused = false;
    
    gameClock.restart();
    animationClock.restart();
}

std::string Game4::formatTime(float seconds) {
    int mins = static_cast<int>(seconds) / 60;
    int secs = static_cast<int>(seconds) % 60;
    int millis = static_cast<int>((seconds - static_cast<int>(seconds)) * 100);
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << mins << ":"
        << std::setw(2) << secs << "."
        << std::setw(2) << millis;
    return oss.str();
}

void Game4::handleInput(const sf::Event& event, sf::RenderWindow& window) {
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
    
    // 处理返回按钮
    if (!gameWon && returnButtonInitialized) {
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
            std::cout << "Game4 paused (ESC pressed)" << std::endl;
            return;
        }
        
        // 游戏胜利后按空格重新开始
        if (gameWon && event.key.code == sf::Keyboard::Space) {
            reset();
        }
    }
}

void Game4::update(float deltaTime) {
    if (gameWon || gamePaused) return;
    
    // 键盘移动控制
    float newX = player.x;
    float newY = player.y;
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        newX += cos(player.angle) * player.moveSpeed;
        newY += sin(player.angle) * player.moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        newX -= cos(player.angle) * player.moveSpeed;
        newY -= sin(player.angle) * player.moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        newX += cos(player.angle - 3.14159f/2) * player.moveSpeed;
        newY += sin(player.angle - 3.14159f/2) * player.moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        newX += cos(player.angle + 3.14159f/2) * player.moveSpeed;
        newY += sin(player.angle + 3.14159f/2) * player.moveSpeed;
    }
    
    // 碰撞检测
    if (!checkCollision(newX, player.y)) {
        player.x = newX;
    }
    if (!checkCollision(player.x, newY)) {
        player.y = newY;
    }
    
    // 旋转控制
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        player.angle -= player.rotSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        player.angle += player.rotSpeed;
    }
    
    // 检查是否到达终点
    int mapX = static_cast<int>(player.x);
    int mapY = static_cast<int>(player.y);
    if (worldMap[mapY][mapX] == 2) {
        gameWon = true;
        completionTime = gameClock.getElapsedTime().asSeconds();
        std::cout << "Game4 completed! Time: " << formatTime(completionTime) << std::endl;
    }
}

bool Game4::checkCollision(float x, float y) {
    int mapX = static_cast<int>(x);
    int mapY = static_cast<int>(y);
    
    if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) {
        return true;
    }
    
    return worldMap[mapY][mapX] == 1;
}

sf::Color Game4::getWallColor(int x, int y, bool isVertical, float distance) {
    // 根据位置和距离创建多彩效果
    float brightness = 1.0f - (distance / MAX_DEPTH) * 0.7f;
    brightness = std::max(0.3f, std::min(1.0f, brightness));
    
    int colorPattern = (x + y) % 4;
    sf::Color baseColor;
    
    switch(colorPattern) {
        case 0: baseColor = sf::Color(255, 100, 100); break; // 红
        case 1: baseColor = sf::Color(100, 255, 100); break; // 绿
        case 2: baseColor = sf::Color(100, 100, 255); break; // 蓝
        case 3: baseColor = sf::Color(255, 255, 100); break; // 黄
    }
    
    // 垂直墙面稍暗以增加深度感
    if (!isVertical) brightness *= 0.75f;
    
    return sf::Color(
        static_cast<sf::Uint8>(baseColor.r * brightness),
        static_cast<sf::Uint8>(baseColor.g * brightness),
        static_cast<sf::Uint8>(baseColor.b * brightness)
    );
}

void Game4::castRays(sf::RenderWindow& window) {
    int VIEW_WIDTH = 320;
    int VIEW_HEIGHT = 180;
    
    float rayAngle = player.angle - FOV / 2.0f;
    float rayStep = FOV / NUM_RAYS;
    
    for (int ray = 0; ray < NUM_RAYS; ray++) {
        float rayDirX = cos(rayAngle);
        float rayDirY = sin(rayAngle);
        
        float distance = 0;
        bool hitWall = false;
        bool isVertical = false;
        int hitX = 0, hitY = 0;
        
        while (!hitWall && distance < MAX_DEPTH) {
            distance += 0.05f;
            
            int testX = static_cast<int>(player.x + rayDirX * distance);
            int testY = static_cast<int>(player.y + rayDirY * distance);
            
            if (testX < 0 || testX >= MAP_WIDTH || testY < 0 || testY >= MAP_HEIGHT) {
                hitWall = true;
                distance = MAX_DEPTH;
            } else if (worldMap[testY][testX] == 1 || worldMap[testY][testX] == 2) {
                hitWall = true;
                hitX = testX;
                hitY = testY;
                
                // 判断是垂直还是水平墙面
                float testAngle = atan2(testY - player.y, testX - player.x);
                isVertical = (fabs(cos(testAngle)) > fabs(sin(testAngle)));
            }
        }
        
        // 修正鱼眼效果
        distance *= cos(rayAngle - player.angle);
        
        // 计算墙的高度
        int ceiling = static_cast<int>(VIEW_HEIGHT / 2.0f - VIEW_HEIGHT / distance);
        int floor = VIEW_HEIGHT - ceiling;
        
        float columnWidth = static_cast<float>(VIEW_WIDTH) / NUM_RAYS;
        
        // 绘制天花板
        sf::RectangleShape ceilingRect(sf::Vector2f(columnWidth + 1, ceiling));
        ceilingRect.setPosition(ray * columnWidth, 0);
        ceilingRect.setFillColor(sf::Color(50, 50, 80));
        window.draw(ceilingRect);
        
        // 绘制墙
        if (distance < MAX_DEPTH) {
            int wallHeight = floor - ceiling;
            sf::RectangleShape wallRect(sf::Vector2f(columnWidth + 1, wallHeight));
            wallRect.setPosition(ray * columnWidth, ceiling);
            
            // 如果是终点，使用特殊颜色
            if (worldMap[hitY][hitX] == 2) {
                float pulse = (sin(animationClock.getElapsedTime().asSeconds() * 5) + 1.0f) / 2.0f;
                wallRect.setFillColor(sf::Color(255, static_cast<sf::Uint8>(215 * pulse), 0));
            } else {
                wallRect.setFillColor(getWallColor(hitX, hitY, isVertical, distance));
            }
            
            window.draw(wallRect);
        }
        
        // 绘制地板
        sf::RectangleShape floorRect(sf::Vector2f(columnWidth + 1, VIEW_HEIGHT - floor));
        floorRect.setPosition(ray * columnWidth, floor);
        floorRect.setFillColor(sf::Color(80, 80, 80));
        window.draw(floorRect);
        
        rayAngle += rayStep;
    }
}

void Game4::drawMinimap(sf::RenderWindow& window) {
    int minimapSize = 80;  // 缩小小地图
    int cellSize = minimapSize / MAP_WIDTH;
    int offsetX = 320 - minimapSize - 5;
    int offsetY = 5;
    
    // 半透明背景
    sf::RectangleShape bg(sf::Vector2f(minimapSize, minimapSize));
    bg.setPosition(offsetX, offsetY);
    bg.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(bg);
    
    // 绘制地图
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            sf::RectangleShape cell(sf::Vector2f(cellSize - 0.5f, cellSize - 0.5f));
            cell.setPosition(offsetX + x * cellSize, offsetY + y * cellSize);
            
            if (worldMap[y][x] == 1) {
                cell.setFillColor(sf::Color(100, 100, 100));
            } else if (worldMap[y][x] == 2) {
                cell.setFillColor(sf::Color(255, 215, 0));
            } else {
                cell.setFillColor(sf::Color(30, 30, 30));
            }
            window.draw(cell);
        }
    }
    
    // 绘制玩家
    sf::CircleShape playerDot(1.5f);
    playerDot.setPosition(
        offsetX + player.x * cellSize - 1.5f,
        offsetY + player.y * cellSize - 1.5f
    );
    playerDot.setFillColor(sf::Color::Red);
    window.draw(playerDot);
    
    // 绘制玩家方向
    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(offsetX + player.x * cellSize, offsetY + player.y * cellSize), sf::Color::Red),
        sf::Vertex(sf::Vector2f(
            offsetX + (player.x + cos(player.angle) * 0.5f) * cellSize,
            offsetY + (player.y + sin(player.angle) * 0.5f) * cellSize
        ), sf::Color::Red)
    };
    window.draw(line, 2, sf::Lines);
}

void Game4::render(sf::RenderWindow& window) {
    // 重置视图
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
                std::cout << "Return button clicked in Game4" << std::endl;
                gamePaused = true;
            });
            returnButtonInitialized = true;
            std::cout << "Return button initialized in Game4" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize return button: " << e.what() << std::endl;
        }
    }
    
    if (!gameWon) {
        // 绘制3D视图
        castRays(window);
        
        // 绘制小地图
        drawMinimap(window);
        
        // 绘制返回按钮
        if (returnButtonInitialized) {
            returnButton.render(window);
        }
        
        // 绘制说明和计时器
        if (fontLoaded) {
            // 说明文字
            sf::Text instructions;
            instructions.setFont(gameFont);
            instructions.setCharacterSize(6);
            instructions.setFillColor(sf::Color::White);
            instructions.setPosition(5, 165);
            instructions.setString("WASD:Move Arrow:Turn Q:Exit");
            window.draw(instructions);
            
            // 计时器
            float currentTime = gameClock.getElapsedTime().asSeconds();
            sf::Text timerText;
            timerText.setFont(gameFont);
            timerText.setCharacterSize(10);
            timerText.setFillColor(sf::Color::Yellow);
            timerText.setPosition(5, 15);
            timerText.setString("Time: " + formatTime(currentTime));
            window.draw(timerText);
        }
    } else {
        // 胜利画面
        sf::RectangleShape winBg(sf::Vector2f(320, 180));
        float pulse = (sin(animationClock.getElapsedTime().asSeconds() * 3) + 1.0f) / 2.0f;
        winBg.setFillColor(sf::Color(
            static_cast<sf::Uint8>(255 * pulse),
            static_cast<sf::Uint8>(215 * pulse),
            0
        ));
        window.draw(winBg);
        
        if (fontLoaded) {
            sf::Text winText;
            winText.setFont(gameFont);
            winText.setCharacterSize(24);
            winText.setFillColor(sf::Color::Black);
            winText.setString("You Win!");
            winText.setPosition(110, 60);
            window.draw(winText);
            
            sf::Text timeText;
            timeText.setFont(gameFont);
            timeText.setCharacterSize(12);
            timeText.setFillColor(sf::Color::Black);
            timeText.setString("Time: " + formatTime(completionTime));
            timeText.setPosition(100, 90);
            window.draw(timeText);
            
            sf::Text restartText;
            restartText.setFont(gameFont);
            restartText.setCharacterSize(10);
            restartText.setFillColor(sf::Color::Black);
            restartText.setString("SPACE:Restart Q:Exit");
            restartText.setPosition(85, 115);
            window.draw(restartText);
        }
    }
}