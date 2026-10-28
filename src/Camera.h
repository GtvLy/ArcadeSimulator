#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics.hpp>

class Camera {
private:
    sf::View view;
    sf::Vector2f position;        // 摄像机中心位置
    sf::Vector2f size;            // 摄像机大小
    sf::FloatRect bounds;         // 摄像机边界限制
    bool hasBounds;               // 是否有边界限制

public:
    Camera();
    Camera(sf::Vector2f viewSize);
    
    // 设置摄像机大小
    void setSize(sf::Vector2f viewSize);
    sf::Vector2f getSize() const { return size; }
    
    // 设置摄像机位置
    void setPosition(sf::Vector2f pos);
    void setCenter(sf::Vector2f center);
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getCenter() const { return view.getCenter(); }
    
    // 设置边界
    void setBounds(sf::FloatRect boundary);
    void clearBounds() { hasBounds = false; }
    
    // 跟随目标
    void followTarget(sf::Vector2f targetCenter);
    
    // 获取视图
    sf::View& getView() { return view; }
    const sf::View& getView() const { return view; }
    
    // 应用到窗口
    void applyTo(sf::RenderWindow& window);

private:
    void clampToBounds();  // 限制摄像机在边界内
};

#endif