#include "Player.h"
#include <iostream>
#include <cmath>

Player::Player()
    : currentState(PlayerState::Idle)
    , position(0, 0)
    , velocity(0, 0)
    , moveSpeed(100.0f)
    , facingRight(true)
    , displayScale(1.0f)
    , hasBounds(false)  // 添加这行
{
}

void Player::init(float scaleRatio) {
    displayScale = scaleRatio;
    
    std::cout << "Initializing player with scale ratio: " << scaleRatio << std::endl;
    
    // 加载站立动画
    idleAnim.load("../assets/images/scene2/main_Idle.png", 128, 128, 9, 10.0f);
    idleAnim.setOriginCenter();
    idleAnim.setScale(scaleRatio, scaleRatio);
    idleAnim.setLoop(true);
    
    // 加载行走动画
    walkAnim.load("../assets/images/scene2/main_Walk.png", 128, 128, 12, 12.0f);
    walkAnim.setOriginCenter();
    walkAnim.setScale(scaleRatio, scaleRatio);
    walkAnim.setLoop(true);
    
    // 设置初始状态
    setState(PlayerState::Idle);
    
    std::cout << "Player initialized successfully!" << std::endl;
}

void Player::setPosition(sf::Vector2f pos) {
    position = pos;
    idleAnim.setPosition(position);
    walkAnim.setPosition(position);
}

sf::Vector2f Player::getCenter() const {
    // 返回角色的中心点（用于摄像机跟随）
    return position;
}

void Player::setState(PlayerState newState) {
    if (currentState == newState) return;
    
    currentState = newState;
    
    // 停止之前的动画
    idleAnim.stop();
    walkAnim.stop();
    
    // 播放新动画
    switch (currentState) {
        case PlayerState::Idle:
            idleAnim.play();
            break;
        case PlayerState::Walking:
            walkAnim.play();
            break;
    }
}

void Player::handleInput() {
    velocity.x = 0;
    
    // A键向左
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        std::cout << "[DEBUG] A key pressed!" << std::endl;
        velocity.x = -moveSpeed;
        facingRight = false;
    }
    // D键向右
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        std::cout << "[DEBUG] D key pressed!" << std::endl;
        velocity.x = moveSpeed;
        facingRight = true;
    }
    
    // 根据速度设置状态
    if (std::abs(velocity.x) > 0.1f) {
        setState(PlayerState::Walking);
    } else {
        setState(PlayerState::Idle);
    }
}

void Player::update(float deltaTime) {
    // 更新位置
    position.x += velocity.x * deltaTime;
    
    // 限制在边界内
    if (hasBounds) {
        // 计算角色实际显示的宽度（32像素）
        float halfWidth = 16.0f;  // 32 / 2
        
        if (position.x - halfWidth < movementBounds.left) {
            position.x = movementBounds.left + halfWidth;
        }
        if (position.x + halfWidth > movementBounds.left + movementBounds.width) {
            position.x = movementBounds.left + movementBounds.width - halfWidth;
        }
    }
    
    // 更新精灵位置
    idleAnim.setPosition(position);
    walkAnim.setPosition(position);
    
    // 更新翻转
    idleAnim.setFlipX(!facingRight);
    walkAnim.setFlipX(!facingRight);
    
    // 更新动画
    updateAnimation(deltaTime);
}

void Player::updateAnimation(float deltaTime) {
    switch (currentState) {
        case PlayerState::Idle:
            idleAnim.update(deltaTime);
            break;
        case PlayerState::Walking:
            walkAnim.update(deltaTime);
            break;
    }
}

void Player::render(sf::RenderWindow& window) {
    switch (currentState) {
        case PlayerState::Idle:
            idleAnim.render(window);
            break;
        case PlayerState::Walking:
            walkAnim.render(window);
            break;
    }
}

sf::FloatRect Player::getBounds() const {
    // 返回当前动画的边界
    switch (currentState) {
        case PlayerState::Idle:
            return idleAnim.getGlobalBounds();
        case PlayerState::Walking:
            return walkAnim.getGlobalBounds();
        default:
            return sf::FloatRect();
    }
}

void Player::setBounds(sf::FloatRect bounds) {
    movementBounds = bounds;
    hasBounds = true;
}