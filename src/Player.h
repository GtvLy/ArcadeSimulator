#ifndef PLAYER_H
#define PLAYER_H

#include "SpriteSheet.h"
#include <SFML/Graphics.hpp>

enum class PlayerState {
    Idle,
    Walking
};

class Player {
private:
    SpriteSheet idleAnim;
    SpriteSheet walkAnim;
    
    PlayerState currentState;
    sf::Vector2f position;
    sf::Vector2f velocity;
    
    float moveSpeed;
    bool facingRight;  // true=右, false=左
    
    float displayScale;  // 显示缩放比例

    sf::FloatRect movementBounds;  // 移动边界
    bool hasBounds;                // 是否有边界限制

public:
    Player();
    
    // 初始化（加载资源）
    void init(float scaleRatio);
    
    // 设置位置
    void setPosition(sf::Vector2f pos);
    sf::Vector2f getPosition() const { return position; }
    
    // 获取中心点（用于摄像机跟随）
    sf::Vector2f getCenter() const;
    
    // 更新
    void update(float deltaTime);
    
    // 处理输入
    void handleInput();
    
    // 渲染
    void render(sf::RenderWindow& window);
    
    // 获取边界（用于碰撞检测）
    sf::FloatRect getBounds() const;

    // 设置移动边界
    void setBounds(sf::FloatRect bounds);
    
private:
    void setState(PlayerState newState);
    void updateAnimation(float deltaTime);
};

#endif