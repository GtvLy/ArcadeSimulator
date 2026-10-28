#include "Scene1.h"
#include <iostream>

Scene1::Scene1() 
    : showingHelp(false)
    , bgMusic(nullptr)
{
    std::cout << "Scene1 (Main Menu) created" << std::endl;
}

Scene1::~Scene1() {
    std::cout << "Scene1 (Main Menu) destroyed" << std::endl;
}

void Scene1::onEnter() {
    std::cout << "Scene1 entered - Loading main menu..." << std::endl;
    
    try {
        // 加载背景图
        std::cout << "Loading main menu background..." << std::endl;
        sf::Texture& bgTexture = ResourceManager::getInstance().loadTexture("../assets/images/scene1/bg.png");
        background.setTexture(bgTexture);
        background.setPosition(0, 0);
        
        // 加载背景音乐
        std::cout << "Loading background music..." << std::endl;
        bgMusic = &ResourceManager::getInstance().loadMusic("../assets/audio/night.ogg");
        bgMusic->setLoop(true);
        bgMusic->play();
        std::cout << "Background music playing..." << std::endl;
        
        // 创建开始游戏按钮
        std::cout << "Loading start button..." << std::endl;
        startButton.setTextures(
            "../assets/images/scene1/start_off.png",
            "../assets/images/scene1/start_on.png",
            "../assets/images/scene1/start_click.png"
        );
        startButton.setPosition(sf::Vector2f(13, 61));
        startButton.setClickSound("../assets/audio/button.ogg");  // 添加这行
        startButton.setCallback([this]() {
            startGame();
        });
        
        // 创建帮助按钮
        std::cout << "Loading help button..." << std::endl;
        helpButton.setTextures(
            "../assets/images/scene1/help_off.png",
            "../assets/images/scene1/help_on.png",
            "../assets/images/scene1/help_click.png"
        );
        helpButton.setPosition(sf::Vector2f(13, 96));
        helpButton.setClickSound("../assets/audio/button.ogg");  // 添加这行
        helpButton.setCallback([this]() {
            showHelpPage();
        });
        
        // 加载帮助页面背景
        std::cout << "Loading help page..." << std::endl;
        sf::Texture& helpTexture = ResourceManager::getInstance().loadTexture("../assets/images/scene1/help.png");
        helpBackground.setTexture(helpTexture);
        helpBackground.setPosition(0, 0);
        
        // 创建返回按钮
        std::cout << "Loading return button..." << std::endl;
        returnButton.setTextures(
            "../assets/images/return_off.png",
            "../assets/images/return_on.png",
            "../assets/images/return_click.png"
        );
        returnButton.setPosition(sf::Vector2f(149, 164));
        returnButton.setClickSound("../assets/audio/button.ogg");  // 添加这行
        returnButton.setCallback([this]() {
            hideHelpPage();
        });
        
        std::cout << "Scene1 initialization complete!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception in Scene1::onEnter: " << e.what() << std::endl;
    }
}

void Scene1::onExit() {
    std::cout << "Scene1 exited" << std::endl;
    
    // 停止背景音乐
    if (bgMusic) {
        bgMusic->stop();
    }
}

void Scene1::handleInput(const sf::Event& event, sf::RenderWindow& window) {
    // 获取鼠标位置（转换到游戏坐标系）
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
    
    if (showingHelp) {
        // 帮助页面：只处理返回按钮
        returnButton.update(worldPos);
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                returnButton.handleMousePressed(worldPos);
            }
        }
        
        if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                returnButton.handleMouseReleased(worldPos);
            }
        }
        
        if (event.type == sf::Event::KeyPressed) {
            returnButton.handleKeyPressed(event.key.code);
        }
        
    } else {
        // 主菜单：处理开始和帮助按钮
        startButton.update(worldPos);
        helpButton.update(worldPos);
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                startButton.handleMousePressed(worldPos);
                helpButton.handleMousePressed(worldPos);
            }
        }
        
        if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                startButton.handleMouseReleased(worldPos);
                helpButton.handleMouseReleased(worldPos);
            }
        }
        
        if (event.type == sf::Event::KeyPressed) {
            startButton.handleKeyPressed(event.key.code);
            helpButton.handleKeyPressed(event.key.code);
        }
    }
}

void Scene1::update(float deltaTime) {
    // 主菜单暂时没有需要更新的内容
}

void Scene1::render(sf::RenderWindow& window) {
    if (showingHelp) {
        // 渲染帮助页面
        window.draw(helpBackground);
        returnButton.render(window);
    } else {
        // 渲染主菜单
        window.draw(background);
        startButton.render(window);
        helpButton.render(window);
    }
}

void Scene1::showHelpPage() {
    std::cout << "Showing help page..." << std::endl;
    showingHelp = true;
}

void Scene1::hideHelpPage() {
    std::cout << "Hiding help page..." << std::endl;
    showingHelp = false;
}

void Scene1::startGame() {
    std::cout << "Starting game - switching to Scene2..." << std::endl;
    requestSceneChange(SceneType::Scene2);  // 现在可以切换了
}