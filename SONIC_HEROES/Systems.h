// Systems.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "Enemy.h"
#include<iostream>
#include<fstream>


using namespace sf;

class Menu {
private:
    static const int OPTION_COUNT = 4;
    Font font;
    Text options[OPTION_COUNT];
    int selected{ 0 };
    const char* labels[OPTION_COUNT] = { "Start Game", "Select Level", "Leaderboard", "Exit" };

    Texture backgroundTexture;
    Sprite backgroundSprite;

    Text title;

    SoundBuffer beepBuffer;
    Sound beepSound;

    Music backgroundMusic;

public:
    Menu() {
        if (!font.loadFromFile("../Data/Jaro/Jaro.ttf"))
            std::cerr << "Failed to load Jaro.ttf" << std::endl;

        if (!backgroundTexture.loadFromFile("../Data/menubg2.jpg"))
            std::cerr << "Failed to load background image" << std::endl;
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(0.4f, 0.4f);

        if (!beepBuffer.loadFromFile("../Data/select.wav"))
            std::cerr << "Failed to load select.wav" << std::endl;
        beepSound.setBuffer(beepBuffer);

        if (!backgroundMusic.openFromFile("../Data/m.wav"))
            std::cerr << "Failed to load background music" << std::endl;
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(50.f);
        backgroundMusic.play();

        Vector2f windowSize(800, 600);

        title.setFont(font);
        title.setString("Developed by Fanc");
        title.setCharacterSize(48);
        title.setFillColor(Color::Cyan);
        title.setStyle(Text::Bold);
        title.setPosition(200.f, 20.f);

        for (int i = 0; i < OPTION_COUNT; ++i) {
            options[i] = Text(labels[i], font, 36);
            options[i].setPosition(310.f, 150.f + i * 60.f);
            options[i].setFillColor(i == selected ? Color::Yellow : Color::White);
        }
    }

    int navigate(const Event& ev) {
        bool moved = false;
        if (ev.type == Event::KeyPressed) {
            if (ev.key.code == Keyboard::Up && selected > 0) {
                selected--;
                moved = true;
            }
            if (ev.key.code == Keyboard::Down && selected + 1 < OPTION_COUNT) {
                selected++;
                moved = true;
            }
            if (ev.key.code == Keyboard::Enter) return selected;

            if (moved) beepSound.play();

            for (int i = 0; i < OPTION_COUNT; ++i)
                options[i].setFillColor(i == selected ? Color::Yellow : Color::White);
        }
        return -1;
    }

    void draw(RenderWindow& win) {
        win.draw(backgroundSprite);
        win.draw(title);
        for (int i = 0; i < OPTION_COUNT; ++i)
            win.draw(options[i]);
    }
};




class SaveSystem {
private:
    std::string savePath;
public:
    
};

class Leaderboard {
private:
    int topScores[10];
    int scoreCount{ 0 };

    Texture backgroundTexture;
    Sprite backgroundSprite;

public:
    Leaderboard() {
        if (!backgroundTexture.loadFromFile("../Data/menubg2.jpg"))
            std::cerr << "Failed to load background image for leaderboard\n";
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(0.4f, 0.4f);
    }

    void addScore(const string& name, int score);

    void viewScoreboard(RenderWindow& window, Font& font) {
        ifstream file("scores.txt");
        string lines[100];
        int scores[100];
        int count = 0;

        if (file.is_open()) {
            while (file >> lines[count] >> scores[count]) {
                count++;
            }
            file.close();
        }

        for (int i = 0; i < count - 1; i++) {
            for (int j = i + 1; j < count; j++) {
                if (scores[i] < scores[j]) {
                    int tempScore = scores[i];
                    scores[i] = scores[j];
                    scores[j] = tempScore;

                    string tempLine = lines[i];
                    lines[i] = lines[j];
                    lines[j] = tempLine;
                }
            }
        }

        window.clear();
        window.draw(backgroundSprite);

        Text title("Scoreboard", font, 50);
        title.setPosition(300, 50);
        title.setFillColor(Color::Cyan);
        window.draw(title);

        for (int i = 0; i < count && i < 10; ++i) {
            Text scoreText(lines[i] + ": \t\t\t\t\t\t" + to_string(scores[i]), font, 30);
            scoreText.setPosition(300, 150 + i * 40);
            scoreText.setFillColor(Color::White);
            window.draw(scoreText);
        }

        Text instruction("Press ESC to return to menu", font, 20);
        instruction.setPosition(300, 600);
        instruction.setFillColor(Color::White);
        window.draw(instruction);

        window.display();

        while (true) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    window.close();
                    return;
                }
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                    return;
                }
            }
        }
    }
};


class LevelManager {
private:
    int choice{ 1 };
    int maxLevels;

    Texture backgroundTexture;
    Sprite  backgroundSprite;

   
    Font font;
    Text prompt;

    Text* levelTexts;

public:
    LevelManager(int maxLv)
        : maxLevels(maxLv)
    {
        
        if (!backgroundTexture.loadFromFile("../Data/menubg2.jpg"))
            std::cerr << "LevelManager: failed to load menubg2.jpg\n";
        backgroundSprite.setTexture(backgroundTexture);
  
        backgroundSprite.setScale(0.4f, 0.4f);

       
        if (!font.loadFromFile("../Data/Jaro/Jaro.ttf"))
            std::cerr << "LevelManager: failed to load Jaro.ttf\n";

     
        prompt.setFont(font);
        prompt.setString("Select Level (1-" + std::to_string(maxLevels) + ") and press Enter");
        prompt.setCharacterSize(24);
        prompt.setFillColor(Color::White);
        prompt.setPosition(50.f, 50.f);

        // 4) Create level number texts
        levelTexts = new Text[maxLevels];
        for (int i = 0; i < maxLevels; ++i) {
            levelTexts[i].setFont(font);
            levelTexts[i].setString(std::to_string(i + 1));
            levelTexts[i].setCharacterSize(30);
            levelTexts[i].setPosition(100.f + i * 80.f, 150.f);
            levelTexts[i].setFillColor((i + 1) == choice ? Color::Yellow : Color::White);
        }
    }

    ~LevelManager() {
        delete[] levelTexts;
    }

    int navigate(const Event& ev) {
        if (ev.type == Event::KeyPressed) {
            if (ev.key.code == Keyboard::Left && choice > 1) {
                choice--;
            }
            else if (ev.key.code == Keyboard::Right && choice < maxLevels) {
                choice++;
            }
            else if (ev.key.code == Keyboard::Enter) {
                return choice;
            }
         
            for (int i = 0; i < maxLevels; ++i) {
                levelTexts[i].setFillColor((i + 1) == choice
                    ? Color::Yellow
                    : Color::White);
            }
        }
        return -1;
    }

    void draw(RenderWindow& win) {
        win.draw(backgroundSprite);
        win.draw(prompt);
        for (int i = 0; i < maxLevels; ++i) {
            win.draw(levelTexts[i]);
        }
    }
};



class LevelTimer {
private:
    float timeLeft{ 0.f };
public:
    void update(float dt);
    bool isExpired() const;
};