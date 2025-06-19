#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "Hitbox.h"

class Projectile {
private:
    sf::Sprite   sprite;
    sf::Texture* texture;
    float        x, y;
    float        vx, vy;
    float        lifeTimer = 0.f;
    const float  lifeMax = 2.0f;
    bool         alive = true;
    int          damage;

    Hitbox       hitbox;    // ← your new hitbox member

public:
    Projectile(sf::Texture* tex,
        float startX, float startY,
        float velX, float velY,
        int dmg)
        : texture(tex), x(startX), y(startY),
        vx(velX), vy(velY), damage(dmg),
        hitbox(GameConstants::CELL_SIZE / 2.0f,
            GameConstants::CELL_SIZE / 2.0f)  // half‐cell hitbox
    {
        sprite.setTexture(*texture);
        sprite.setPosition(x, y);
        sprite.setScale(1.0f, 1.0f);

        // Initialize hitbox at the same spot
        hitbox.setPosition(x, y);
    }

    void update(float dt) {
        // Move
        x += vx * dt;
        y += vy * dt;
        sprite.setPosition(x, y);

        // Sync hitbox
        hitbox.setPosition(x, y);

        // Lifetime by timer
        lifeTimer += dt;
        if (lifeTimer >= lifeMax)
            alive = false;
    }

    void render(float offsetX, sf::RenderWindow& win) {
        // draw bullet
        sprite.setPosition(x - offsetX, y);
        win.draw(sprite);
        sprite.setPosition(x, y);

        // draw hitbox (for debug)
        hitbox.shape.setPosition(hitbox.left(), hitbox.top());
        win.draw(hitbox.shape);
    }

    bool isAlive() const { return alive; }
    int  getDamage() const { return damage; }

    // Expose the hitbox for collision tests
    const Hitbox& getHitbox() const {
        return hitbox;
    }
};
