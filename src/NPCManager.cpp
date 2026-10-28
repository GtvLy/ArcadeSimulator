#include "NPCManager.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

NPCManager::NPCManager()
    : spawnTimer(0.0f)
    , spawnInterval(1.0f)  // 每3秒生成一个NPC
    , spriteScale(1.0f)
    , spawnArea(0, 137, 483, 27)  // Y范围: 94-121
{
    // 初始化随机数种子
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void NPCManager::init(float scaleRatio) {
    spriteScale = scaleRatio;
    std::cout << "NPCManager initialized with scale: " << scaleRatio << std::endl;
}

void NPCManager::update(float deltaTime, sf::FloatRect visibleArea) {
    // 更新生成计时器
    spawnTimer += deltaTime;
    
    // 检查是否需要生成新NPC
    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0.0f;
        
        // 随机选择从左边还是右边生成
        bool fromLeft = (std::rand() % 2) == 0;
        spawnNPC(fromLeft);
    }
    
    // 更新所有NPC
    for (auto& npc : npcs) {
        npc->update(deltaTime);
    }
    
    // 移除屏幕外的NPC
    removeOffScreenNPCs(visibleArea);
}

void NPCManager::render(sf::RenderWindow& window) {
    for (auto& npc : npcs) {
        npc->render(window);
    }
}

void NPCManager::checkPlayerCollision(sf::FloatRect playerBounds) {
    for (auto& npc : npcs) {
        sf::FloatRect npcBounds = npc->getBounds();
        
        // 检查是否重叠（X和Y都要重叠）
        bool xOverlap = (playerBounds.left < npcBounds.left + npcBounds.width) &&
                       (playerBounds.left + playerBounds.width > npcBounds.left);
        
        bool yOverlap = (playerBounds.top < npcBounds.top + npcBounds.height) &&
                       (playerBounds.top + playerBounds.height > npcBounds.top);
        
        if (xOverlap && yOverlap) {
            // 重叠时增加透明度（虚化）
            float currentOpacity = npc->getOpacity();
            float newOpacity = std::max(100.0f, currentOpacity - 300.0f * 0.016f);
            npc->setOpacity(newOpacity);
        } else {
            // 不重叠时恢复透明度
            float currentOpacity = npc->getOpacity();
            float newOpacity = std::min(255.0f, currentOpacity + 300.0f * 0.016f);
            npc->setOpacity(newOpacity);
        }
    }
}

void NPCManager::spawnNPC(bool fromLeft) {
    auto npc = std::make_unique<NPC>();
    
    // 随机选择NPC类型 (1 或 2)
    int type = (std::rand() % 2) + 1;
    npc->init(type, spriteScale);
    
    // 随机Y坐标 (94-121)
    float randomY = spawnArea.top + static_cast<float>(std::rand() % static_cast<int>(spawnArea.height));
    
    // 设置位置和速度
    if (fromLeft) {
        // 从左边生成，向右移动
        npc->setPosition(sf::Vector2f(-50, randomY));
        npc->setVelocity(sf::Vector2f(30.0f, 0));  // 向右移动
        npc->setFacingRight(true);
    } else {
        // 从右边生成，向左移动
        npc->setPosition(sf::Vector2f(spawnArea.width + 50, randomY));
        npc->setVelocity(sf::Vector2f(-30.0f, 0));  // 向左移动
        npc->setFacingRight(false);
    }
    
    npcs.push_back(std::move(npc));
    std::cout << "NPC spawned (type " << type << ") from " << (fromLeft ? "left" : "right") << std::endl;
}

void NPCManager::removeOffScreenNPCs(sf::FloatRect visibleArea) {
    npcs.erase(
        std::remove_if(npcs.begin(), npcs.end(),
            [&visibleArea](const std::unique_ptr<NPC>& npc) {
                return npc->isOffScreen(visibleArea.left, visibleArea.left + visibleArea.width);
            }),
        npcs.end()
    );
}