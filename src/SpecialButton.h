#ifndef SPECIALBUTTON_H
#define SPECIALBUTTON_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <functional>
#include "ResourceManager.h"

class SpecialButton {
private:
    sf::Sprite sprite;
    sf::Vector2f position;
    sf::FloatRect bounds;
    
    std::string offTexturePath;
    std::string buttonTexturePath;
    
    bool isPressed;
    
    sf::Sound clickSound;
    bool hasClickSound;
    
    std::function<void()> callback;
    
    void updateTexture();

public:
    SpecialButton();
    
    void setTextures(const std::string& offPath, const std::string& buttonPath);
    void setPosition(sf::Vector2f pos);
    void setClickSound(const std::string& soundPath);
    void setCallback(std::function<void()> func) { callback = func; }
    
    void handleMousePressed(const sf::Vector2f& mousePos);
    void handleMouseReleased(const sf::Vector2f& mousePos);
    void handleKeyPressed(sf::Keyboard::Key key);
    void handleKeyReleased(sf::Keyboard::Key key);
    
    void render(sf::RenderWindow& window);
    bool contains(const sf::Vector2f& point) const;
};

#endif