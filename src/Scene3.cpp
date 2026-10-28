#include "Scene3.h"
#include "Game1.h"
#include "Game2.h"
#include "Game3.h"
#include "Game4.h"
#include "Game5.h"
#include <iostream>

Scene3::Scene3() 
    : playingGame(false)
{
    std::cout << "Scene3 (Arcade Selection) created" << std::endl;
}

Scene3::~Scene3() {
    std::cout << "Scene3 (Arcade Selection) destroyed" << std::endl;
}

void Scene3::onEnter() {
    std::cout << "Scene3 entered - Loading arcade selection..." << std::endl;
    
    try {
        // 加载背景图
        std::cout << "Loading arcade background..." << std::endl;
        sf::Texture& bgTexture = ResourceManager::getInstance().loadTexture("../assets/images/scene3/bg.png");
        background.setTexture(bgTexture);
        background.setPosition(0, 0);
        
        // 初始化6个游戏按钮
        gameButtons.clear();
        
        // 游戏按钮的位置
        struct ButtonInfo {
            int gameIndex;
            sf::Vector2f position;
        };
        
        ButtonInfo buttonInfos[] = {
            {1, sf::Vector2f(30, 35)},
            {2, sf::Vector2f(120, 35)},
            {3, sf::Vector2f(210, 35)},
            {4, sf::Vector2f(30, 86)},
            {5, sf::Vector2f(120, 86)},
            {6, sf::Vector2f(210, 86)}
        };
        
        for (int i = 0; i < 6; i++) {
            Button btn;
            std::string basePath = "../assets/images/scene3/game" + std::to_string(i + 1);
            
            btn.setTextures(
                basePath + "_off.png",
                basePath + "_on.png",
                basePath + "_click.png"
            );
            btn.setPosition(buttonInfos[i].position);
            btn.setClickSound("../assets/audio/button.ogg");
            
            // 设置回调（捕获游戏索引）
            int gameIndex = i + 1;
            btn.setCallback([this, gameIndex]() {
                selectGame(gameIndex);
            });
            
            gameButtons.push_back(btn);
            std::cout << "Game button " << (i + 1) << " loaded" << std::endl;
        }
        
        // 创建返回按钮
        std::cout << "Loading return button..." << std::endl;
        returnButton.setTextures(
            "../assets/images/return_off.png",
            "../assets/images/return_on.png",
            "../assets/images/return_click.png"
        );
        returnButton.setPosition(sf::Vector2f(272, 2));
        returnButton.setClickSound("../assets/audio/button.ogg");
        returnButton.setCallback([this]() {
            std::cout << "Return button clicked - going back to Scene2..." << std::endl;
            requestSceneChange(SceneType::Scene2);
        });
        
        std::cout << "Scene3 initialization complete!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception in Scene3::onEnter: " << e.what() << std::endl;
    }
}

void Scene3::onExit() {
    std::cout << "Scene3 exited" << std::endl;
}

void Scene3::handleInput(const sf::Event& event, sf::RenderWindow& window) {
    // 如果正在玩游戏，将输入传给游戏
    if (playingGame && currentGame) {
        currentGame->handleInput(event, window);
        
        // 如果游戏暂停（ESC），返回选择界面
        if (!currentGame->isGameRunning() && !currentGame->isGameOver()) {
            playingGame = false;
            currentGame.reset();
            std::cout << "Returned to arcade selection" << std::endl;
            return;
        }
        
        return;  // 游戏中不处理其他输入
    }
    
    // 原来的按钮处理代码...
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
    
    for (auto& btn : gameButtons) {
        btn.update(worldPos);
    }
    
    returnButton.update(worldPos);
    
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            for (auto& btn : gameButtons) {
                btn.handleMousePressed(worldPos);
            }
            returnButton.handleMousePressed(worldPos);
        }
    }
    
    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            for (auto& btn : gameButtons) {
                btn.handleMouseReleased(worldPos);
            }
            returnButton.handleMouseReleased(worldPos);
        }
    }
    
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Q) {
            returnButton.handleKeyPressed(event.key.code);
        }
    }
}

void Scene3::update(float deltaTime) {
    // 如果正在玩游戏，更新游戏
    if (playingGame && currentGame) {
        currentGame->update(deltaTime);
    }
}

void Scene3::render(sf::RenderWindow& window) {
    // 如果正在玩游戏，渲染游戏
    if (playingGame && currentGame) {
        currentGame->render(window);
        return;
    }
    
    // 否则渲染选择界面
    window.draw(background);
    
    for (auto& btn : gameButtons) {
        btn.render(window);
    }
    
    returnButton.render(window);
}

void Scene3::selectGame(int gameIndex) {
    std::cout << "Game " << gameIndex << " selected!" << std::endl;
    
    if (gameIndex == 4) {
        // 启动3D迷宫游戏
        currentGame.reset();  // 先清空之前的游戏
        auto game4 = std::make_unique<Game4>();
        game4->init();
        currentGame = std::move(game4);
        playingGame = true;
        std::cout << "Game4 (3D Maze) started!" << std::endl;
    } else if (gameIndex == 5) {
        // 贪吃蛇
        currentGame.reset();
        auto game5 = std::make_unique<Game5>();
        game5->init();
        currentGame = std::move(game5);
        playingGame = true;
        std::cout << "Game5 (Snake) started!" << std::endl;
    } else if (gameIndex == 2) {
        // 赛车
        currentGame.reset();  // 先清空之前的游戏
        auto game2 = std::make_unique<Game2>();
        game2->init();
        currentGame = std::move(game2);
        playingGame = true;
        std::cout << "Game2 (Racing) started!" << std::endl; 
    } else if (gameIndex == 3) {
        // 赛车
        currentGame.reset();  // 先清空之前的游戏
        auto game3 = std::make_unique<Game3>();
        game3->init();
        currentGame = std::move(game3);
        playingGame = true;
        std::cout << "Game3 (Mario) started!" << std::endl; 
    } else if (gameIndex == 1) {
        //街头霸王
        currentGame.reset();  // 先清空之前的游戏
        auto game1 = std::make_unique<Game1>();
        game1->init();
        currentGame = std::move(game1);
        playingGame = true;
        std::cout << "Game3 (Mario) started!" << std::endl; 
    } 
        else {
        std::cout << "TODO: Launch game " << gameIndex << std::endl;
    }
}