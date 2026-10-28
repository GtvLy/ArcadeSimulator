#include "Scene2.h"
#include <iostream>

Scene2::Scene2()
    : bgMusic(nullptr)
    , sceneSize(483, 180)
    , spriteScale(1.0f)
    , showFKeyPrompt(false)
    , isReturning(false)
    , isFirstEnter(true)  // 添加这行
{
    std::cout << "Scene2 created" << std::endl;
}

Scene2::~Scene2() {
    std::cout << "Scene2 destroyed" << std::endl;
}

void Scene2::onEnter() {
    std::cout << "Scene2 entered - Loading game scene..." << std::endl;
    
    try {
        // 加载背景图
        std::cout << "Loading scene2 background..." << std::endl;
        sf::Texture& bgTexture = ResourceManager::getInstance().loadTexture("../assets/images/scene2/bg.png");
        bgSprite.setTexture(bgTexture);
        bgSprite.setPosition(0, 0);
        
        // 加载灯光图层
        std::cout << "Loading lights..." << std::endl;
        sf::Texture& lightTexture = ResourceManager::getInstance().loadTexture("../assets/images/scene2/light.png");
        lightSprite.setTexture(lightTexture);
        lightSprite.setPosition(0, 0);
        
        // 加载背景音乐
        std::cout << "Loading background music..." << std::endl;
        bgMusic = &ResourceManager::getInstance().loadMusic("../assets/audio/city.ogg");
        bgMusic->setLoop(true);
        bgMusic->play();
        std::cout << "Background music playing..." << std::endl;
        
        // 计算精灵缩放比例
        calculateSpriteScale();
        
        // 初始化玩家
        std::cout << "Initializing player..." << std::endl;
        player.init(spriteScale);

        // 初始化玩家（只在第一次）
        if (isFirstEnter) {
            std::cout << "Initializing player..." << std::endl;
            player.init(spriteScale);
            player.setPosition(sf::Vector2f(25, 137));
            isFirstEnter = false;
            std::cout << "Player initialized at initial position" << std::endl;
        } else {
            std::cout << "Player already initialized, position preserved: " 
                    << player.getPosition().x << ", " << player.getPosition().y << std::endl;
        }

        // 初始化NPC管理器
        std::cout << "Initializing NPC manager..." << std::endl;
        npcManager.init(spriteScale);
        npcManager.setSpawnArea(sf::FloatRect(0, 137, sceneSize.x, 27));  // Y: 94-121

        // 初始化F键提示
        std::cout << "Initializing F key prompt..." << std::endl;
        fKeyPrompt.setTextures(
            "../assets/images/F_off.png",
            "../assets/images/F_button.png"
        );
        fKeyPrompt.setClickSound("../assets/audio/coin.ogg");  // 添加音效
        fKeyPrompt.setCallback([this]() {
            std::cout << "F key pressed - switching to Scene3..." << std::endl;
            requestSceneChange(SceneType::Scene3);
        });
        std::cout << "F key prompt OK" << std::endl;

        // 创建返回按钮
        std::cout << "Loading return button..." << std::endl;
        returnButton.setTextures(
            "../assets/images/return_off.png",
            "../assets/images/return_on.png",
            "../assets/images/return_click.png"
        );
        returnButton.setPosition(sf::Vector2f(5, 5));  // 先设置一个固定位置
        returnButton.setClickSound("../assets/audio/button.ogg");
        returnButton.setCallback([this]() {
            if (!isReturning) {  // 防止重复触发
                isReturning = true;
                std::cout << "Return button clicked - going back to Scene1..." << std::endl;
                requestSceneChange(SceneType::Scene1);
            }
        });
        
        // 设置摄像机
        std::cout << "Setting up camera..." << std::endl;
        camera.setSize(sf::Vector2f(320, 180));
        camera.setBounds(sf::FloatRect(0, 0, sceneSize.x, sceneSize.y));
        camera.followTarget(player.getCenter());
        
        std::cout << "Scene2 initialization complete!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception in Scene2::onEnter: " << e.what() << std::endl;
    }
}

void Scene2::onExit() {
    std::cout << "Scene2 exited" << std::endl;
    
    // 停止背景音乐
    if (bgMusic) {
        bgMusic->stop();
    }
    
    // 重置返回标志
    isReturning = false;
}

void Scene2::calculateSpriteScale() {
    // 场景Y轴高度是180，精灵原始高度是128
    // 我们希望精灵在场景中显示为32像素高
    // 所以缩放比例 = 180 / 128 / 5.625 = 0.25
    // 或者直接：32 / 128 = 0.25
    
    spriteScale = 32.0f / 128.0f;
    
    std::cout << "Sprite scale calculated: " << spriteScale << std::endl;
}

void Scene2::handleInput(const sf::Event& event, sf::RenderWindow& window) {
    // 获取鼠标位置
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
    
    // 只在未返回时处理按钮输入
    if (!isReturning) {
        // 始终更新返回按钮
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
            // Q键触发返回按钮
            if (event.key.code == sf::Keyboard::Q) {
                returnButton.handleKeyPressed(event.key.code);
            }
        }
        
        // F键提示处理
        if (showFKeyPrompt) {
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    fKeyPrompt.handleMousePressed(worldPos);
                }
            }
            
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    fKeyPrompt.handleMouseReleased(worldPos);
                }
            }
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::F) {
                    fKeyPrompt.handleKeyPressed(sf::Keyboard::F);
                }
            }
            
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::F) {
                    fKeyPrompt.handleKeyReleased(sf::Keyboard::F);
                }
            }
        }
    }
    
    // 玩家控制不受影响（A/D键移动）
    // Player::handleInput() 在 update 中调用，使用实时按键检测
    // 所以这里不需要处理
}

void Scene2::update(float deltaTime) {
    std::cout << "[DEBUG] Scene2::update called, isReturning=" << isReturning << std::endl;
    
    player.handleInput();
    player.update(deltaTime);
    camera.followTarget(player.getCenter());
    
    sf::FloatRect visibleArea(
        camera.getCenter().x - 160,
        camera.getCenter().y - 90,
        320,
        180
    );
    npcManager.update(deltaTime, visibleArea);
    npcManager.checkPlayerCollision(player.getBounds());
    checkInteractionZone();
    
    sf::Vector2f cameraTopLeft(
        camera.getCenter().x - 160,
        camera.getCenter().y - 90
    );
    returnButton.setPosition(sf::Vector2f(cameraTopLeft.x + 5, cameraTopLeft.y + 5));
}

void Scene2::checkInteractionZone() {
    float playerX = player.getPosition().x;
    
    // 检查玩家是否在X轴240-300范围内
    if (playerX >= 240.0f && playerX <= 300.0f) {
        if (!showFKeyPrompt) {
            showFKeyPrompt = true;
            std::cout << "Player entered interaction zone!" << std::endl;
        }
        
        // 更新F键提示位置（跟随玩家，显示在玩家右上方）
        sf::Vector2f playerPos = player.getPosition();
        fKeyPrompt.setPosition(sf::Vector2f(playerPos.x + 5, playerPos.y - 0));
        
    } else {
        if (showFKeyPrompt) {
            showFKeyPrompt = false;
            std::cout << "Player left interaction zone!" << std::endl;
        }
    }
}

void Scene2::render(sf::RenderWindow& window) {
    camera.applyTo(window);
    window.draw(bgSprite);
    npcManager.render(window);
    player.render(window);
    window.draw(lightSprite);
    
    // 渲染返回按钮
    returnButton.render(window);
    
    // 渲染F键提示
    if (showFKeyPrompt) {
        fKeyPrompt.render(window);
    }
}