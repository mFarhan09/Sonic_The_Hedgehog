// Player.hpp
#pragma once

#include "Character.h"
#include <SFML/Graphics.hpp>
#include <iostream>

class Player {
private:
    string name;
    unsigned int scores;
    unsigned int HP{ 10 };
    Character* chars[3];
    int idx;
    Clock invincibleClock;
    float invincibleDuration{ 1.0f };

    Music hurtSound;




public:
    Player(): idx(2)
    {

		hurtSound.openFromFile("../Data/hurt.wav");
		hurtSound.setVolume(50.f);

        chars[0] = new Sonic();
        chars[1] = new Knuckles();
        chars[2] = new Tails();
    }

    ~Player() {
        for (int i = 0; i < 3; ++i)
            delete chars[i];
    }


    void switchChar() {

        static bool zHandle = true;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
            zHandle = true;
        }
        else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && zHandle) {

            idx = (idx + 1) % 3;
            std::cout << "Switched to character index: " << idx << std::endl;
            zHandle = false;  

        }
    }
    float* getPX()
    {
        return chars[idx]->getPX();
    }

    Character* const* getChars() const {
        return chars;
    }
    


    void update(char** lvl, float dt,const int& levelId) {
        switchChar();
       

        if (idx == 0) {
            chars[0]->update(lvl, true, dt, nullptr,levelId);
            chars[1]->update(lvl, false, dt, chars[idx],levelId);
            chars[2]->update(lvl, false, dt, chars[idx],levelId);
        }
        else if (idx == 1) {
            chars[0]->update(lvl, false, dt, chars[idx],levelId);
            chars[1]->update(lvl, true, dt,nullptr,levelId);
            chars[2]->update(lvl, false, dt, chars[idx],levelId);
        }
        else {
            chars[2]->carryingOthers(chars[0], chars[1]);
            chars[0]->update(lvl, false, dt, chars[idx],levelId);
            chars[1]->update(lvl, false, dt, chars[idx],levelId);
            chars[2]->update(lvl, true, dt,nullptr,levelId);

        }
        if (invincibleClock.getElapsedTime().asSeconds() > 1) {
            chars[idx]->setInvincible(false);
        }
        if (chars[idx]->getY()>GameConstants::SCREEN_Y) {
            HP = 0;

        }
    }
    void bounceInvincible(int playerId) {

        if (idx == playerId && invincibleClock.getElapsedTime().asSeconds() > 1) {
            hurtSound.play();
            invincibleClock.restart();  // Reset invincibility timer
            chars[idx]->bounce();
            chars[idx]->setInvincible(true);
            std::cout << "Player took damage! HP = " << HP << std::endl;

        }
    }
    void takeDamage(int playerId) {
        
        if (idx == playerId && invincibleClock.getElapsedTime().asSeconds() > 1 && HP > 0) {
			hurtSound.play();
            --HP;
            invincibleClock.restart();  // Reset invincibility timer
            chars[idx]->bounce();
			chars[idx]->setInvincible(true); 
            // Bounce the character when hit
            std::cout << "Player took damage! HP = " << HP << std::endl;

            if (HP == 0) {
                std::cout << "Player died!" << std::endl;
                // Handle game over logic here
            }
        }
    }

	

    // Pops up a simple text prompt and returns whatever the player types (Enter to finish)
    



    void draw(sf::RenderWindow& win, float offsetX) {
			
        for (int i = 0; i < 3; i++)
        {

            chars[i]->draw(win, offsetX);

        }
    }
    bool isAlive()
    {
        return HP != 0;
    }
    float getX() const {
        return chars[idx]->getX();
    }

    void setAttributes(float gravity, float speed, float accel, int width) {
        for (int i = 0; i < 3; i++)
        {
            chars[i]->setAttributes(gravity, speed, accel, width);
        }
    }
    bool isInvincible() {
        return invincibleClock.getElapsedTime().asSeconds() <= 1;
    }
    void resetForLevel(){
        for (int i = 0; i < 3; i++)
        {
            chars[i]->resetChar();

        }
    }
};
