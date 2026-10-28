#include "SpriteSheet.h"
#include <iostream>

SpriteSheet::SpriteSheet()
    : texture(nullptr)
    , frameWidth(0)
    , frameHeight(0)
    , totalFrames(0)
    , currentFrame(0)
    , frameTime(0.1f)
    , elapsedTime(0.0f)
    , loop(true)
    , playing(false)
{
}

void SpriteSheet::load(const std::string& texturePath, int frameW, int frameH, int frames, float fps) {
    frameWidth = frameW;
    frameHeight = frameH;
    totalFrames = frames;
    frameTime = 1.0f / fps;
    currentFrame = 0;
    elapsedTime = 0.0f;
    
    // 加载纹理
    texture = &ResourceManager::getInstance().loadTexture(texturePath);
    sprite.setTexture(*texture);
    
    // 设置初始帧
    updateFrame();
    
    std::cout << "SpriteSheet loaded: " << texturePath << std::endl;
    std::cout << "  Frame size: " << frameWidth << "x" << frameHeight << std::endl;
    std::cout << "  Total frames: " << totalFrames << std::endl;
    std::cout << "  FPS: " << fps << std::endl;
}

void SpriteSheet::update(float deltaTime) {
    if (!playing) return;
    
    elapsedTime += deltaTime;
    
    // 检查是否需要切换到下一帧
    if (elapsedTime >= frameTime) {
        elapsedTime -= frameTime;
        currentFrame++;
        
        // 检查是否到达最后一帧
        if (currentFrame >= totalFrames) {
            if (loop) {
                currentFrame = 0;  // 循环播放
            } else {
                currentFrame = totalFrames - 1;  // 停在最后一帧
                playing = false;
            }
        }
        
        updateFrame();
    }
}

void SpriteSheet::updateFrame() {
    // 计算当前帧在纹理中的位置
    int frameX = currentFrame * frameWidth;
    int frameY = 0;  // 假设精灵表是单行排列
    
    sprite.setTextureRect(sf::IntRect(frameX, frameY, frameWidth, frameHeight));
}

void SpriteSheet::setFlipX(bool flip) {
    sf::Vector2f scale = sprite.getScale();
    sf::Vector2f currentOrigin = sprite.getOrigin();
    
    if (flip) {
        scale.x = -std::abs(scale.x);  // 翻转
        // 如果原点在中心，翻转时不需要调整
        // 如果原点在左上角，需要调整
        if (currentOrigin.x == 0 && currentOrigin.y == 0) {
            sprite.setOrigin(frameWidth, 0);
        }
    } else {
        scale.x = std::abs(scale.x);   // 不翻转
        if (currentOrigin.x == frameWidth && currentOrigin.y == 0) {
            sprite.setOrigin(0, 0);
        }
    }
    sprite.setScale(scale);
}

void SpriteSheet::render(sf::RenderWindow& window) {
    window.draw(sprite);
}

void SpriteSheet::setOriginCenter() {
    sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);
}