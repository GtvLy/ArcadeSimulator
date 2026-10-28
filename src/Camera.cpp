#include "Camera.h"
#include <algorithm>
#include <iostream>

Camera::Camera() 
    : size(320, 180)
    , position(0, 0)
    , hasBounds(false)
{
    view.setSize(size);
    view.setCenter(size.x / 2.0f, size.y / 2.0f);
}

Camera::Camera(sf::Vector2f viewSize)
    : size(viewSize)
    , position(0, 0)
    , hasBounds(false)
{
    view.setSize(size);
    view.setCenter(size.x / 2.0f, size.y / 2.0f);
}

void Camera::setSize(sf::Vector2f viewSize) {
    size = viewSize;
    view.setSize(size);
}

void Camera::setPosition(sf::Vector2f pos) {
    position = pos;
    view.setCenter(position);
    
    if (hasBounds) {
        clampToBounds();
    }
}

void Camera::setCenter(sf::Vector2f center) {
    view.setCenter(center);
    position = center;
    
    if (hasBounds) {
        clampToBounds();
    }
}

void Camera::setBounds(sf::FloatRect boundary) {
    bounds = boundary;
    hasBounds = true;
    clampToBounds();
    
    std::cout << "Camera bounds set: (" 
              << bounds.left << ", " << bounds.top << ", " 
              << bounds.width << ", " << bounds.height << ")" << std::endl;
}

void Camera::followTarget(sf::Vector2f targetCenter) {
    setCenter(targetCenter);
}

void Camera::clampToBounds() {
    if (!hasBounds) return;
    
    sf::Vector2f center = view.getCenter();
    sf::Vector2f halfSize = size / 2.0f;
    
    // 限制左边界
    if (center.x - halfSize.x < bounds.left) {
        center.x = bounds.left + halfSize.x;
    }
    
    // 限制右边界
    if (center.x + halfSize.x > bounds.left + bounds.width) {
        center.x = bounds.left + bounds.width - halfSize.x;
    }
    
    // 限制上边界
    if (center.y - halfSize.y < bounds.top) {
        center.y = bounds.top + halfSize.y;
    }
    
    // 限制下边界
    if (center.y + halfSize.y > bounds.top + bounds.height) {
        center.y = bounds.top + bounds.height - halfSize.y;
    }
    
    view.setCenter(center);
    position = center;
}

void Camera::applyTo(sf::RenderWindow& window) {
    window.setView(view);
}