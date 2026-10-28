#include "SpecialButton.h"
#include <iostream>

SpecialButton::SpecialButton()
    : position(0, 0)
    , isPressed(false)
    , hasClickSound(false)
{
}

void SpecialButton::setTextures(const std::string& offPath, const std::string& buttonPath) {
    offTexturePath = offPath;
    buttonTexturePath = buttonPath;
    updateTexture();
}

void SpecialButton::setPosition(sf::Vector2f pos) {
    position = pos;
    sprite.setPosition(position);
    bounds = sprite.getGlobalBounds();
}

void SpecialButton::setClickSound(const std::string& soundPath) {
    sf::SoundBuffer& buffer = ResourceManager::getInstance().loadSoundBuffer(soundPath);
    clickSound.setBuffer(buffer);
    hasClickSound = true;
}

void SpecialButton::updateTexture() {
    sf::Texture* texture = nullptr;
    
    if (isPressed) {
        texture = &ResourceManager::getInstance().loadTexture(buttonTexturePath);
    } else {
        texture = &ResourceManager::getInstance().loadTexture(offTexturePath);
    }
    
    if (texture) {
        sprite.setTexture(*texture);
        bounds = sprite.getGlobalBounds();
    }
}

void SpecialButton::handleMousePressed(const sf::Vector2f& mousePos) {
    if (contains(mousePos)) {
        isPressed = true;
        updateTexture();
    }
}

void SpecialButton::handleMouseReleased(const sf::Vector2f& mousePos) {
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
    updateTexture();
}

void SpecialButton::handleKeyPressed(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::F) {
        isPressed = true;
        updateTexture();
    }
}

void SpecialButton::handleKeyReleased(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::F && isPressed) {
        // 播放音效
        if (hasClickSound) {
            clickSound.play();
        }
        
        // 触发回调
        if (callback) {
            callback();
        }
        
        isPressed = false;
        updateTexture();
    }
}

void SpecialButton::render(sf::RenderWindow& window) {
    window.draw(sprite);
}

bool SpecialButton::contains(const sf::Vector2f& point) const {
    return bounds.contains(point);
}