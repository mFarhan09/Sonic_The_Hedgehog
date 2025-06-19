// Obstacle.hpp
#pragma once
#include "Utilities.h"
#include "Animation.h"
#include "character.h"

class Character;  // forward

class Obstacle {
protected:
    bool isSolid{ true };
public:
    Hitbox hitbox;
    Animation anim;
    virtual ~Obstacle() = default;
    virtual void collide(Character& c) = 0;
};