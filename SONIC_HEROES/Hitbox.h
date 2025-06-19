#pragma once

#include <SFML/Graphics.hpp>
#include<iostream>
using namespace std;

struct Hitbox {
    float width{ 0 }, height{ 0 };
    sf::RectangleShape shape;

    Hitbox() {
        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineColor(sf::Color::Red);
        shape.setOutlineThickness(1.f);
    }
    Hitbox(float w, float h)
        : width(w), height(h) {
        shape.setSize({ w, h });
        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineColor(sf::Color::Red);
        shape.setOutlineThickness(1.f);
    }

    void setSize(float w, float h) {
        width = w;
        height = h;
        shape.setSize({ w, h });
    }
    void setPosition(float x, float y) {
        shape.setPosition(x, y);
    }
    float left()   const { return shape.getPosition().x; }
    float top()    const { return shape.getPosition().y; }
    float right()  const { return left() + width; }
    float bottom() const { return top() + height; }
};

class Platform {

public:
    static bool checkCollision(const Hitbox& hb, char** grid,int width) {
        int leftCell = int(hb.left() / GameConstants::CELL_SIZE);
        int rightCell = int((hb.right() - 1) / GameConstants::CELL_SIZE);
        int bottomCell = int((hb.bottom() - 1) / GameConstants::CELL_SIZE);
      
        if (bottomCell < 0 || bottomCell >= GameConstants::ROWS)
            return false;
        for (int col = leftCell; col <= rightCell; ++col) {
            if (col < 0 || col >= width) continue;
            if (grid[bottomCell][col] == 'w'|| grid[bottomCell][col] == 'p' || grid[bottomCell][col] == 'w') return true;
        }
        return false;
    }
    static bool checkBossCollision(const Hitbox& hb, char** grid, int width) {
        int leftCell = int(hb.left() / GameConstants::CELL_SIZE);
        int rightCell = int((hb.right() - 1) / GameConstants::CELL_SIZE);
        int bottomCell = int((hb.bottom() - 1) / GameConstants::CELL_SIZE);

        if (bottomCell < 0 || bottomCell >= GameConstants::ROWS)
            return false;
        for (int col = leftCell; col <= rightCell; ++col) {
            if (col < 0 || col >= width) continue;
            if (grid[bottomCell][col] == 'w' || grid[bottomCell][col] == 'p' || grid[bottomCell][col] == 'w') return true;
        }
        return false;
    }
    static bool checkFlyingCollision(Hitbox& Tails, Hitbox& Knuckles) {
        float L1 = Tails.left();
        float R1 = Tails.right();
        float T1 = Tails.top();
        float B1 = Tails.bottom();

        float L2 = Knuckles.left();
        float R2 = Knuckles.right();
        float T2 = Knuckles.top();
        float B2 = Knuckles.bottom();

        float tailsHeight = B1 - T1;
        float knucklesHeight = B2 - T2;

        float tailsLowerQuarterStart = B1 - (tailsHeight * 0.125f);  // Bottom 25% of Tails
        float knucklesUpperQuarterEnd = T2 + (knucklesHeight * 0.125f); // Top 25% of Knuckles

        bool horizontalOverlap = (R1 > L2) && (L1 < R2);
        bool verticalOverlap = (tailsLowerQuarterStart < knucklesUpperQuarterEnd) && (B1 > T2);

        return horizontalOverlap && verticalOverlap;
    }
    static bool checkEnemyCollision(const Hitbox& hb, const Hitbox& enemy) {

        float L1 = hb.left();
        float R1 = hb.right();
        float T1 = hb.top();
        float B1 = hb.bottom();

        float L2 = enemy.left();
        float R2 = enemy.right();
        float T2 = enemy.top();
        float B2 = enemy.bottom();

        if (R1 < L2 || L1 > R2 || B1 < T2 || T1 > B2) {
  
            return false; // No collision
        }

        return true; // Collision
    }

};