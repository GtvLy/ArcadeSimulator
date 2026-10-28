#include "TestSpriteScene.h"
#include <iostream>

TestSpriteScene::TestSpriteScene() 
    : isFlipped(false)  // 初始化
{
    std::cout << "TestSpriteScene created" << std::endl;
}

TestSpriteScene::~TestSpriteScene() {
    std::cout << "TestSpriteScene destroyed" << std::endl;
}

void TestSpriteScene::onEnter() {
    std::cout << "TestSpriteScene entered" << std::endl;
    
    // 设置背景色
    background.setSize(sf::Vector2f(320, 180));
    background.setFillColor(sf::Color(50, 50, 80));
    
    // 加载主角站立动画测试
    testAnim.load("../assets/images/scene2/main_Idle.png", 128, 128, 9, 10.0f);
    
    // 设置原点为精灵中心
    testAnim.setOriginCenter();
    
    // 设置位置为屏幕中心
    testAnim.setPosition(160, 90);
    
    // 设置缩放（让128x128缩小到合适大小，比如缩小到1/4）
    testAnim.setScale(0.25f, 0.25f);  // 现在是32x32
    
    testAnim.setLoop(true);
    testAnim.play();
    
    std::cout << "Test animation loaded and playing" << std::endl;
    std::cout << "Press SPACE to pause/play" << std::endl;
    std::cout << "Press F to flip horizontally" << std::endl;
}

void TestSpriteScene::onExit() {
    std::cout << "TestSpriteScene exited" << std::endl;
}

void TestSpriteScene::handleInput(const sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::KeyPressed) {
        // 按空格键切换动画
        if (event.key.code == sf::Keyboard::Space) {
            if (testAnim.isPlaying()) {
                testAnim.pause();
                std::cout << "Animation paused" << std::endl;
            } else {
                testAnim.play();
                std::cout << "Animation playing" << std::endl;
            }
        }
        
        // 按F键翻转精灵
        if (event.key.code == sf::Keyboard::F) {
            isFlipped = !isFlipped;
            testAnim.setFlipX(isFlipped);
            std::cout << "Sprite flipped: " << (isFlipped ? "Yes" : "No") << std::endl;
        }
    }
}

void TestSpriteScene::update(float deltaTime) {
    testAnim.update(deltaTime);
}

void TestSpriteScene::render(sf::RenderWindow& window) {
    window.draw(background);
    testAnim.render(window);
}