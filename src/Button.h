#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <functional>
#include "ResourceManager.h"

// 按钮状态
enum class ButtonState {
    Off,    // 未选中
    On,     // 鼠标悬停
    Click   // 点击
};

class Button {
private:
    sf::Sprite sprite;
    sf::Vector2f position;
    sf::FloatRect bounds;  // 碰撞区域
    
    // 三种状态的纹理路径
    std::string offTexturePath;
    std::string onTexturePath;
    std::string clickTexturePath;
    
    ButtonState currentState;
    bool isPressed;  // 鼠标是否按下
    
    std::function<void()> callback;  // 点击回调函数
    
    void updateTexture();

    sf::Sound clickSound;  // 点击音效
    bool hasClickSound;    // 是否有音效

public:
    Button();
    Button(const std::string& offPath, const std::string& onPath, 
           const std::string& clickPath, sf::Vector2f pos);
    
    // 设置纹理路径
    void setTextures(const std::string& offPath, const std::string& onPath, 
                     const std::string& clickPath);
    
    // 设置位置
    void setPosition(sf::Vector2f pos);
    sf::Vector2f getPosition() const { return position; }
    
    // 设置回调函数
    void setCallback(std::function<void()> func) { callback = func; }
    
    // 更新按钮状态
    void update(const sf::Vector2f& mousePos);
    
    // 处理鼠标事件
    void handleMousePressed(const sf::Vector2f& mousePos);
    void handleMouseReleased(const sf::Vector2f& mousePos);
    
    // 处理键盘事件（Q键）
    void handleKeyPressed(sf::Keyboard::Key key);
    
    // 渲染
    void render(sf::RenderWindow& window);
    
    // 检查鼠标是否在按钮上
    bool contains(const sf::Vector2f& point) const;
    
    // 获取当前状态
    ButtonState getState() const { return currentState; }

    // 设置点击音效
    void setClickSound(const std::string& soundPath);
};

#endif