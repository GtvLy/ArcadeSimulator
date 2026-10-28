#ifndef GAMEBASE_H
#define GAMEBASE_H

#include <SFML/Graphics.hpp>

class GameBase {
public:
    virtual ~GameBase() {}
    
    virtual void init() = 0;
    virtual void handleInput(const sf::Event& event, sf::RenderWindow& window) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    
    virtual bool isGameRunning() const = 0;
    virtual bool isGameOver() const = 0;
    virtual int getScore() const = 0;
    virtual void reset() = 0;
};

#endif