#include "Button.h"
#include <iostream>

Button::Button() 
    : currentState(ButtonState::Off)
    , isPressed(false)
    , position(0, 0)
    , hasClickSound(false)  // 添加这行
{
}

Button::Button(const std::string& offPath, const std::string& onPath, 
               const std::string& clickPath, sf::Vector2f pos)
    : currentState(ButtonState::Off)
    , isPressed(false)
    , position(pos)
    , hasClickSound(false)  // 添加这行
{
    setTextures(offPath, onPath, clickPath);
    setPosition(pos);
}

void Button::setTextures(const std::string& offPath, const std::string& onPath, 
                         const std::string& clickPath) {
    offTexturePath = offPath;
    onTexturePath = onPath;
    clickTexturePath = clickPath;
    
    // 加载默认纹理
    updateTexture();
}

void Button::setPosition(sf::Vector2f pos) {
    position = pos;
    sprite.setPosition(position);
    
    // 更新碰撞区域
    bounds = sprite.getGlobalBounds();
}

void Button::updateTexture() {
    sf::Texture* texture = nullptr;
    
    switch (currentState) {
        case ButtonState::Off:
            texture = &ResourceManager::getInstance().loadTexture(offTexturePath);
            break;
        case ButtonState::On:
            texture = &ResourceManager::getInstance().loadTexture(onTexturePath);
            break;
        case ButtonState::Click:
            texture = &ResourceManager::getInstance().loadTexture(clickTexturePath);
            break;
    }
    
    if (texture) {
        sprite.setTexture(*texture);
        bounds = sprite.getGlobalBounds();
    }
}

void Button::update(const sf::Vector2f& mousePos) {
    // 如果鼠标按下，保持Click状态
    if (isPressed) {
        if (currentState != ButtonState::Click) {
            currentState = ButtonState::Click;
            updateTexture();
        }
        return;
    }
    
    // 检查鼠标是否在按钮上
    if (contains(mousePos)) {
        if (currentState != ButtonState::On) {
            currentState = ButtonState::On;
            updateTexture();
        }
    } else {
        if (currentState != ButtonState::Off) {
            currentState = ButtonState::Off;
            updateTexture();
        }
    }
}

void Button::handleMousePressed(const sf::Vector2f& mousePos) {
    if (contains(mousePos)) {
        isPressed = true;
        currentState = ButtonState::Click;
        updateTexture();
    }
}

void Button::handleMouseReleased(const sf::Vector2f& mousePos) {
    if (isPressed && contains(mousePos)) {
        // 播放音效
        if (hasClickSound) {
            clickSound.play();
        }
        
        // 触发回调
        if (callback) {
            callback();
        }
    }
    
    isPressed = false;
    
    // 更新状态
    if (contains(mousePos)) {
        currentState = ButtonState::On;
    } else {
        currentState = ButtonState::Off;
    }
    updateTexture();
}

void Button::handleKeyPressed(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Q) {
        // 播放音效
        if (hasClickSound) {
            clickSound.play();
        }
        
        // 触发回调
        if (callback) {
            callback();
        }
    }
}

void Button::render(sf::RenderWindow& window) {
    window.draw(sprite);
}

bool Button::contains(const sf::Vector2f& point) const {
    return bounds.contains(point);
}

void Button::setClickSound(const std::string& soundPath) {
    sf::SoundBuffer& buffer = ResourceManager::getInstance().loadSoundBuffer(soundPath);
    clickSound.setBuffer(buffer);
    hasClickSound = true;
}