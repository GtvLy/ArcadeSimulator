#ifndef NPCMANAGER_H
#define NPCMANAGER_H

#include "NPC.h"
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

class NPCManager {
private:
    std::vector<std::unique_ptr<NPC>> npcs;
    float spawnTimer;
    float spawnInterval;  // 生成间隔
    float spriteScale;
    
    sf::FloatRect spawnArea;  // NPC生成区域 (Y: 94-121)
    
public:
    NPCManager();
    
    // 初始化
    void init(float scaleRatio);
    
    // 设置生成区域
    void setSpawnArea(sf::FloatRect area) { spawnArea = area; }
    
    // 更新
    void update(float deltaTime, sf::FloatRect visibleArea);
    
    // 渲染
    void render(sf::RenderWindow& window);
    
    // 检查与玩家的碰撞（用于虚化效果）
    void checkPlayerCollision(sf::FloatRect playerBounds);
    
    // 获取所有NPC
    const std::vector<std::unique_ptr<NPC>>& getNPCs() const { return npcs; }
    
private:
    void spawnNPC(bool fromLeft);  // 生成NPC
    void removeOffScreenNPCs(sf::FloatRect visibleArea);  // 移除屏幕外的NPC
};

#endif