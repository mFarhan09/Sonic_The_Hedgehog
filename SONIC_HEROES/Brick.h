#ifndef BRICK_H
#define BRICK_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Tile.h"

class Brick : public Tile {
private:
    sf::Texture texture;
    sf::Sprite sprite;

public:
    Brick() {
        if (!texture.loadFromFile("../Data/brick1.png")) {
            std::cerr << "Failed to load brick.png\n";
        }
        sprite.setTexture(texture);
    }

    void render(sf::RenderWindow& window, int x, int y) override {
        sprite.setPosition(x * 32, y * 32);
        window.draw(sprite);
    }
};

#endif
