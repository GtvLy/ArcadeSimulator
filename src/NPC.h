#ifndef NPC_H
#define NPC_H

#include "SpriteSheet.h"
#include <SFML/Graphics.hpp>

class NPC {
private:
    SpriteSheet idleAnim;
    SpriteSheet walkAnim;
    
    sf::Vector2f position;
    sf::Vector2f velocity;
    
    float moveSpeed;
    bool facingRight;
    bool isWalking;
    
    float displayScale;
    int npcType;  // 1 或 2，对应不同的NPC资源
    
    float opacity;  // 透明度 (0-255)

    void updateSpriteColor();

public:
    NPC();
    
    // 初始化
    void init(int type, float scaleRatio);
    
    // 设置位置和方向
    void setPosition(sf::Vector2f pos);
    void setVelocity(sf::Vector2f vel);
    void setFacingRight(bool right) { facingRight = right; }
    
    // 获取位置
    sf::Vector2f getPosition() const { return position; }
    
    // 设置透明度
    void setOpacity(float alpha);  // 0-255
    float getOpacity() const { return opacity; }
    
    // 更新
    void update(float deltaTime);
    
    // 渲染
    void render(sf::RenderWindow& window);
    
    // 获取边界
    sf::FloatRect getBounds() const;
    
    // 检查是否超出屏幕
    bool isOffScreen(float minX, float maxX) const;
};

#endif