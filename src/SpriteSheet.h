#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <SFML/Graphics.hpp>
#include <string>
#include "ResourceManager.h"

class SpriteSheet {
private:
    sf::Sprite sprite;
    sf::Texture* texture;
    
    int frameWidth;      // 每帧宽度
    int frameHeight;     // 每帧高度
    int totalFrames;     // 总帧数
    int currentFrame;    // 当前帧
    
    float frameTime;     // 每帧持续时间
    float elapsedTime;   // 已经过的时间
    
    bool loop;           // 是否循环播放
    bool playing;        // 是否正在播放

public:
    SpriteSheet();
    
    // 加载精灵表
    void load(const std::string& texturePath, int frameW, int frameH, int frames, float fps = 10.0f);
    
    // 更新动画
    void update(float deltaTime);
    
    // 播放控制
    void play() { playing = true; }
    void pause() { playing = false; }
    void stop() { playing = false; currentFrame = 0; updateFrame(); }
    void reset() { currentFrame = 0; elapsedTime = 0; updateFrame(); }
    
    // 设置循环
    void setLoop(bool shouldLoop) { loop = shouldLoop; }
    
    // 设置位置
    void setPosition(float x, float y) { sprite.setPosition(x, y); }
    void setPosition(const sf::Vector2f& pos) { sprite.setPosition(pos); }
    sf::Vector2f getPosition() const { return sprite.getPosition(); }
    
    // 设置缩放
    void setScale(float scaleX, float scaleY) { sprite.setScale(scaleX, scaleY); }
    void setScale(const sf::Vector2f& scale) { sprite.setScale(scale); }
    
    // 翻转
    void setFlipX(bool flip);
    
    // 渲染
    void render(sf::RenderWindow& window);
    
    // 获取边界
    sf::FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }
    sf::FloatRect getLocalBounds() const { return sprite.getLocalBounds(); }
    
    // 状态查询
    bool isPlaying() const { return playing; }
    int getCurrentFrame() const { return currentFrame; }
    int getTotalFrames() const { return totalFrames; }

    // 设置原点（用于居中等）
    void setOrigin(float x, float y) { sprite.setOrigin(x, y); }
    void setOrigin(const sf::Vector2f& origin) { sprite.setOrigin(origin); }
    void setOriginCenter();  // 设置原点为中心

    // 设置颜色和透明度
    void setColor(const sf::Color& color) { sprite.setColor(color); }
    sf::Color getColor() const { return sprite.getColor(); }

private:
    void updateFrame();  // 更新当前帧的纹理区域
};

#endif