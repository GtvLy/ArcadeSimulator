#include "TestScene.h"
#include <iostream>

TestScene::TestScene() {
    std::cout << "TestScene created" << std::endl;
}

TestScene::~TestScene() {
    std::cout << "TestScene destroyed" << std::endl;
}

void TestScene::onEnter() {
    std::cout << "TestScene entered" << std::endl;
    
    try {
        // 加载背景图
        std::cout << "Loading background..." << std::endl;
        sf::Texture& bgTexture = ResourceManager::getInstance().loadTexture("../assets/images/scene1/bg.png");
        background.setTexture(bgTexture);
        background.setPosition(0, 0);
        std::cout << "Background loaded!" << std::endl;
        
        // 创建测试按钮
        std::cout << "Loading button textures..." << std::endl;
        testButton.setTextures(
            "../assets/images/scene1/start_off.png",
            "../assets/images/scene1/start_on.png",
            "../assets/images/scene1/start_click.png"
        );
        testButton.setPosition(sf::Vector2f(13, 61));
        std::cout << "Button textures loaded!" << std::endl;
        
        // 设置按钮回调
        testButton.setCallback([this]() {
            std::cout << "Button clicked!" << std::endl;
        });
        
        std::cout << "TestScene initialization complete!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception in TestScene::onEnter: " << e.what() << std::endl;
    }
}

void TestScene::onExit() {
    std::cout << "TestScene exited" << std::endl;
}

void TestScene::handleInput(const sf::Event& event, sf::RenderWindow& window) {
    // 获取鼠标位置（转换到游戏坐标系）
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
    
    // 实时更新按钮状态
    testButton.update(worldPos);
    
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            testButton.handleMousePressed(worldPos);
        }
    }
    
    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            testButton.handleMouseReleased(worldPos);
        }
    }
    
    if (event.type == sf::Event::KeyPressed) {
        testButton.handleKeyPressed(event.key.code);
    }
}

void TestScene::update(float deltaTime) {
    // 暂时为空
}

void TestScene::render(sf::RenderWindow& window) {
    window.draw(background);
    testButton.render(window);
}