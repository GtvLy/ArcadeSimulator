#include "Game.h"
#include "Scene1.h"  // 添加这行
#include "Scene2.h"
#include "Scene3.h"
#include <iostream>

Game::Game() 
    : baseResolution(320, 180)
    , scale(4.0f)
    , running(true)
{
    std::cout << "=== Game Initialization Start ===" << std::endl;
    
    window.create(sf::VideoMode(1280, 720), "Street Arcade Game", sf::Style::Default);
    window.setFramerateLimit(60);
    std::cout << "Window created" << std::endl;
    
    gameView.setSize(baseResolution.x, baseResolution.y);
    gameView.setCenter(baseResolution.x / 2.f, baseResolution.y / 2.f);
    std::cout << "View initialized" << std::endl;
    
    // 添加场景一（主菜单）
    std::cout << "Adding Scene1..." << std::endl;
    sceneManager.addScene(SceneType::Scene1, std::make_unique<Scene1>());

    // 添加场景二（游戏场景）
    std::cout << "Adding Scene2..." << std::endl;
    sceneManager.addScene(SceneType::Scene2, std::make_unique<Scene2>());

    // 添加场景三
    std::cout << "Adding Scene3..." << std::endl;
    sceneManager.addScene(SceneType::Scene3, std::make_unique<Scene3>());
    
    std::cout << "Changing to test scene..." << std::endl;
    sceneManager.changeScene(SceneType::Scene1);
    
    std::cout << "=== Game Initialization Complete ===" << std::endl;
}

Game::~Game() {
    std::cout << "Game shutting down..." << std::endl;
}

void Game::run() {
    while (running && window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            running = false;
            window.close();
        }
        
        if (event.type == sf::Event::Resized) {
            updateScale();
        }
        
        // 将事件传递给场景管理器
        sceneManager.handleInput(event, window);
        
        // 检查并处理场景切换（在处理完所有事件后）
        sceneManager.checkSceneChange();
    }
}

void Game::update() {
    float deltaTime = clock.restart().asSeconds();
    std::cout << "[DEBUG] Game::update, deltaTime=" << deltaTime << std::endl;
    sceneManager.update(deltaTime);
}

void Game::updateScale() {
    sf::Vector2u windowSize = window.getSize();
    
    float scaleX = static_cast<float>(windowSize.x) / baseResolution.x;
    float scaleY = static_cast<float>(windowSize.y) / baseResolution.y;
    
    scale = std::min(scaleX, scaleY);
    
    gameView.setSize(baseResolution.x, baseResolution.y);
    gameView.setCenter(baseResolution.x / 2.f, baseResolution.y / 2.f);
    
    float viewportWidth = (baseResolution.x * scale) / windowSize.x;
    float viewportHeight = (baseResolution.y * scale) / windowSize.y;
    float viewportX = (1.f - viewportWidth) / 2.f;
    float viewportY = (1.f - viewportHeight) / 2.f;
    
    gameView.setViewport(sf::FloatRect(viewportX, viewportY, viewportWidth, viewportHeight));
}

void Game::render() {
    window.clear(sf::Color::Black);
    window.setView(gameView);
    
    // 渲染当前场景
    sceneManager.render(window);
    
    window.display();
}