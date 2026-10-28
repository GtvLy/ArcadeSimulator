#include "NPC.h"
#include <iostream>

NPC::NPC()
    : position(0, 0)
    , velocity(0, 0)
    , moveSpeed(50.0f)
    , facingRight(true)
    , isWalking(true)
    , displayScale(1.0f)
    , npcType(1)
    , opacity(255.0f)
{
}

void NPC::init(int type, float scaleRatio) {
    npcType = type;
    displayScale = scaleRatio;
    
    std::cout << "Initializing NPC type " << type << " with scale ratio: " << scaleRatio << std::endl;
    
    std::string idlePath, walkPath;
    int idleFrames, walkFrames;
    
    if (type == 1) {
        idlePath = "../assets/images/scene2/npc1_Idle.png";
        walkPath = "../assets/images/scene2/npc1_Walk.png";
        idleFrames = 7;
        walkFrames = 12;
    } else {
        idlePath = "../assets/images/scene2/npc2_Idle.png";
        walkPath = "../assets/images/scene2/npc2_Walk.png";
        idleFrames = 6;
        walkFrames = 12;
    }
    
    // 加载站立动画
    idleAnim.load(idlePath, 128, 128, idleFrames, 10.0f);
    idleAnim.setOriginCenter();
    idleAnim.setScale(scaleRatio, scaleRatio);
    idleAnim.setLoop(true);
    
    // 加载行走动画
    walkAnim.load(walkPath, 128, 128, walkFrames, 12.0f);
    walkAnim.setOriginCenter();
    walkAnim.setScale(scaleRatio, scaleRatio);
    walkAnim.setLoop(true);
    walkAnim.play();
    
    std::cout << "NPC type " << type << " initialized!" << std::endl;
    setOpacity(255.0f);
}

void NPC::setPosition(sf::Vector2f pos) {
    position = pos;
    idleAnim.setPosition(position);
    walkAnim.setPosition(position);
}

void NPC::setVelocity(sf::Vector2f vel) {
    velocity = vel;
}

void NPC::update(float deltaTime) {
    // 更新位置
    position += velocity * deltaTime;
    
    // 更新精灵位置
    idleAnim.setPosition(position);
    walkAnim.setPosition(position);
    
    // 更新翻转
    idleAnim.setFlipX(!facingRight);
    walkAnim.setFlipX(!facingRight);
    
    // 更新动画
    if (isWalking) {
        walkAnim.update(deltaTime);
    } else {
        idleAnim.update(deltaTime);
    }
}

void NPC::render(sf::RenderWindow& window) {
    // 这里有个问题：SpriteSheet没有直接设置透明度的方法
    // 我们需要修改SpriteSheet类来支持颜色/透明度设置
    // 暂时先正常渲染
    if (isWalking) {
        walkAnim.render(window);
    } else {
        idleAnim.render(window);
    }
}

sf::FloatRect NPC::getBounds() const {
    if (isWalking) {
        return walkAnim.getGlobalBounds();
    } else {
        return idleAnim.getGlobalBounds();
    }
}

bool NPC::isOffScreen(float minX, float maxX) const {
    return (position.x < minX - 50) || (position.x > maxX + 50);
}

void NPC::updateSpriteColor() {
    sf::Color color(255, 255, 255, static_cast<sf::Uint8>(opacity));
    idleAnim.setColor(color);
    walkAnim.setColor(color);
}

void NPC::setOpacity(float alpha) {
    opacity = std::max(0.0f, std::min(255.0f, alpha));
    updateSpriteColor();
}