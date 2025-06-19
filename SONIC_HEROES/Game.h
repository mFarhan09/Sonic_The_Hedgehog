#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Constants.h"
#include "Utilities.h"
#include "Level.h"
#include "Player.h"
#include "Systems.h"
#include <iostream>
#include<fstream>
using namespace std;
using namespace sf;




class Game {
private:
    static const int LEVEL_COUNT = 4;
    static const int STATE_MENU = 0;
    static const int STATE_SELECT = 1;
    static const int STATE_PLAY = 2;
    static const int STATE_EXIT = 3;
    static const int STATE_LEADER = 4;

    sf::RenderWindow window;
    Level* currentLevelPtr = nullptr;
    int         currentLevelIndex = -1;
    Player      player;
    sf::Font    font;
    Menu        menu;
    LevelManager levelManager;
    Leaderboard board;
    int         score = 0;
    sf::Text    scoreText;

    bool isGameOver = false;
    bool complete = false;
    sf::Text gameOverText;
    sf::Text winText;
    std::string playerName;

public:
    Game()
        : window(sf::VideoMode(GameConstants::SCREEN_X, GameConstants::SCREEN_Y), "Sonic Heroes OOP"),
        levelManager(LEVEL_COUNT)
    {
        window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(60);

        if (!font.loadFromFile("../Data/Jaro/Jaro.ttf"))
            std::cerr << "Failed to load font." << std::endl;

        // Score display
        scoreText.setFont(font);
        scoreText.setCharacterSize(40);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10.f, 10.f);
        scoreText.setString("Score: 0");

        // Game Over text
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(60);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setString("GAME OVER!!");

        // Win text
        winText.setFont(font);
        winText.setCharacterSize(60);
        winText.setFillColor(sf::Color::Green);
        winText.setString("YOU WON!!");
    }

    ~Game() {
        delete currentLevelPtr;
        window.close();
    }

    std::string getPlayerName(sf::RenderWindow& window);

    void saveScore() {
        std::ofstream ofs("scores.txt", std::ios::app);
        if (!ofs) {
            std::cerr << "Failed to open scores file." << std::endl;
            return;
        }
        ofs << playerName << ' ' << score << '\n';
    }

    void loadLevel(int index) {
        delete currentLevelPtr;
        currentLevelPtr = nullptr;
        currentLevelIndex = index;

        switch (index) {
        case 0: currentLevelPtr = new Level1();    break;
        case 1: currentLevelPtr = new Level2();    break;
        case 2: currentLevelPtr = new Level3();    break;
        case 3: currentLevelPtr = new BossLevel(); break;
        default: break;
        }

        if (currentLevelPtr) {
            currentLevelPtr->setup(&player);
            player.resetForLevel();

            score = 0;
            isGameOver = false;
            complete = false;
            scoreText.setString("Score: 0");
        }
    }

    void run() {
        int state = STATE_MENU;
        sf::Clock frameClock;

        while (window.isOpen() && state != STATE_EXIT) {
            sf::Event ev;
            while (window.pollEvent(ev)) {
                if (ev.type == sf::Event::Closed)
                    state = STATE_EXIT;

                // --- MENU ---
                if (state == STATE_MENU) {
                    int sel = menu.navigate(ev);
                    if (sel >= 0) {
                        switch (sel) {
                        case 0:
                            playerName = getPlayerName(window);
                            loadLevel(0);
                            state = STATE_PLAY;
                            break;
                        case 1:
                            state = STATE_SELECT;
                            break;
                        case 2:
                            state = STATE_LEADER;
                            break;
                        case 3:
                            state = STATE_EXIT;
                            break;
                        }
                    }
                }
                // --- LEVEL SELECT ---
                else if (state == STATE_SELECT) {
                    int lvl = levelManager.navigate(ev);
                    if (lvl > 0 && lvl <= LEVEL_COUNT) {
                        playerName = getPlayerName(window);
                        loadLevel(lvl - 1);
                        state = STATE_PLAY;
                    }
                }
                // --- BACK TO MENU AFTER OVER/COMPLETE ---
                if ((isGameOver || complete) && state == STATE_PLAY && ev.type == sf::Event::KeyPressed) {
                    state = STATE_MENU;
                }
            }

            float dt = frameClock.restart().asSeconds();
            if (dt > 0.05f) dt = 0.05f;

            window.clear();

            // Draw according to state
            if (state == STATE_MENU) {
                menu.draw(window);
            }
            else if (state == STATE_SELECT) {
                levelManager.draw(window);
            }
            else if (state == STATE_LEADER) {
                board.viewScoreboard(window, font);
                state = STATE_MENU;
            }
            else if (state == STATE_PLAY && currentLevelPtr) {
                if (!isGameOver && !complete) {
                    // Check death
                    if (!player.isAlive()) {
                        isGameOver = true;
                        saveScore();
                        // center text
                        auto gb = gameOverText.getGlobalBounds();
                        gameOverText.setPosition(
                            GameConstants::SCREEN_X / 2 - gb.width / 2,
                            GameConstants::SCREEN_Y / 2 - gb.height / 2
                        );
                    }
                    // Check level completion
                    else if (currentLevelPtr->isComplete()) {
                        int next = currentLevelIndex + 1;
                        if (next < LEVEL_COUNT) {
                            loadLevel(next);
                        }
                        else {
                            complete = true;
                            saveScore();
                            auto gb = winText.getGlobalBounds();
                            winText.setPosition(
                                GameConstants::SCREEN_X / 2 - gb.width / 2,
                                GameConstants::SCREEN_Y / 2 - gb.height / 2
                            );
                        }
                    }

                    // Normal update & render
                    if (!isGameOver && !complete) {
                        char** grid = currentLevelPtr->getGrid();
                        player.update(grid, dt, currentLevelIndex);
                        scoreText.setString("Score: " + std::to_string(score));

                        float levelPixW = currentLevelPtr->getWidth() * GameConstants::CELL_SIZE- GameConstants::CELL_SIZE;
                        float playerCenterX = player.getX() + GameConstants::CELL_SIZE * 0.5f;
                        float halfW = GameConstants::SCREEN_X * 0.5f;
                        float offsetX = (levelPixW <= GameConstants::SCREEN_X)
                            ? 0.f
                            : std::max(halfW,
                                std::min(playerCenterX, levelPixW - halfW)) - halfW;

                        currentLevelPtr->update(dt, offsetX, player.getPX(), score);
                        currentLevelPtr->render(window, offsetX, player.getPX());
                        player.draw(window, offsetX);
                        window.draw(scoreText);
                    }
                }

                // Draw end‐of‐game message
                if (isGameOver) {
                    window.draw(gameOverText);
                }
                else if (complete) {
                    window.draw(winText);
                }
            }

            window.display();
        }
    }
};





string Game::getPlayerName(RenderWindow& window) {
    char* name = new char[31];
    name[0] = '\0';
    unsigned int length = 0;

    Text nameText("", font, 30);
    nameText.setPosition(300, 300);
    nameText.setFillColor(Color::White);

    Text instruction("Enter your name: ", font, 30);
    instruction.setPosition(300, 250);
    instruction.setFillColor(Color::White);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                delete[] name;
                return "";
            }
            else if (event.type == Event::TextEntered) {
                // Backspace
                if (event.text.unicode == 8 && length > 0) {
                    --length;
                    name[length] = '\0';
                }
                // Printable ASCII
                else if (event.text.unicode >= 32
                    && event.text.unicode < 128
                    && length < 30) {
                    name[length++] = static_cast<char>(event.text.unicode);
                    name[length] = '\0';
                }
            }
            else if (event.type == Event::KeyPressed
                && event.key.code == Keyboard::Enter) {
                std::string result(name);
                delete[] name;
                return result;
            }
        }

        window.clear(Color::Black);
        nameText.setString(name);
        window.draw(instruction);
        window.draw(nameText);
        window.display();
    }

    // Fallback
    std::string result(name);
    delete[] name;
    return result;
}


//
//
//struct GameState {
//    int levelIndex;
//    int hp;
//    int characterId;
//    float posX, posY;
//    int rings;
//    int enemiesDefeated;
//    int score;
//    std::string playerName;
//};
//
//class SaveSystem {
//private:
//    std::string savePath;
//public:
//    SaveSystem(const std::string& path)
//        : savePath(path)
//    {
//    }
//
//    void save(const GameState& state) {
//        std::ofstream ofs(savePath, std::ios::binary);
//        if (!ofs) { std::cerr << "Failed to open save file." << std::endl; return; }
//        ofs.write(reinterpret_cast<const char*>(&state.levelIndex), sizeof(state.levelIndex));
//        ofs.write(reinterpret_cast<const char*>(&state.hp), sizeof(state.hp));
//        ofs.write(reinterpret_cast<const char*>(&state.characterId), sizeof(state.characterId));
//        ofs.write(reinterpret_cast<const char*>(&state.posX), sizeof(state.posX));
//        ofs.write(reinterpret_cast<const char*>(&state.posY), sizeof(state.posY));
//        ofs.write(reinterpret_cast<const char*>(&state.rings), sizeof(state.rings));
//        ofs.write(reinterpret_cast<const char*>(&state.enemiesDefeated), sizeof(state.enemiesDefeated));
//        ofs.write(reinterpret_cast<const char*>(&state.score), sizeof(state.score));
//        size_t nameLen = state.playerName.size();
//        ofs.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
//        ofs.write(state.playerName.c_str(), nameLen);
//    }
//
//    bool load(GameState& state) {
//        std::ifstream ifs(savePath, std::ios::binary);
//        if (!ifs) return false;
//        ifs.read(reinterpret_cast<char*>(&state.levelIndex), sizeof(state.levelIndex));
//        ifs.read(reinterpret_cast<char*>(&state.hp), sizeof(state.hp));
//        ifs.read(reinterpret_cast<char*>(&state.characterId), sizeof(state.characterId));
//        ifs.read(reinterpret_cast<char*>(&state.posX), sizeof(state.posX));
//        ifs.read(reinterpret_cast<char*>(&state.posY), sizeof(state.posY));
//        ifs.read(reinterpret_cast<char*>(&state.rings), sizeof(state.rings));
//        ifs.read(reinterpret_cast<char*>(&state.enemiesDefeated), sizeof(state.enemiesDefeated));
//        ifs.read(reinterpret_cast<char*>(&state.score), sizeof(state.score));
//        size_t nameLen;
//        ifs.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
//        state.playerName.resize(nameLen);
//        ifs.read(&state.playerName[0], nameLen);
//        return true;
//    }
//
//    bool hasSave() const {
//        std::ifstream ifs(savePath);
//        return ifs.good();
//    }
//};
//
//class Game {
//private:
//    static const int LEVEL_COUNT = 4;
//    static const int STATE_MENU = 0;
//    static const int STATE_SELECT_LEVEL = 1;
//    static const int STATE_PLAY = 2;
//    static const int STATE_EXIT = 3;
//
//    sf::RenderWindow window;
//    Level* currentLevelPtr = nullptr;
//    int currentLevelIndex = -1;
//    Player player;
//    sf::Font font;
//    Menu menu;
//    LevelManager levelManager;
//
//    // Score display
//    int score = 0;
//    sf::Text scoreText;
//
//    // Save and load
//    SaveSystem saveSystem;
//
//public:
//    Game()
//        : window(sf::VideoMode(GameConstants::SCREEN_X, GameConstants::SCREEN_Y), "Sonic Heroes OOP"),
//        levelManager(LEVEL_COUNT),
//        saveSystem("savegame.dat")
//    {
//        window.setVerticalSyncEnabled(true);
//        window.setFramerateLimit(60);
//
//        if (!font.loadFromFile("../Data/Jaro/Jaro.ttf"))
//            std::cerr << "Failed to load font." << std::endl;
//
//        scoreText.setFont(font);
//        scoreText.setCharacterSize(40);
//        scoreText.setFillColor(sf::Color::White);
//        scoreText.setPosition(10.f, 10.f);
//        scoreText.setString("Score: 0");
//
//        // If a saved game exists, tell menu to show "Continue"
//        if (saveSystem.hasSave()) {
//            menu.enableContinueOption(true);
//        }
//    }
//
//    ~Game() {
//        // On exit, save current state if playing
//        if (currentLevelPtr) {
//            GameState state;
//            state.levelIndex = currentLevelIndex;
//            state.hp = player.getHP();
//            state.characterId = player.getCharacterId();
//            state.posX = player.getX();
//            state.posY = player.getY();
//            state.rings = player.getRings();
//            state.enemiesDefeated = player.getEnemiesDefeated();
//            state.score = score;
//            state.playerName = player.getName();
//            saveSystem.save(state);
//        }
//        if (currentLevelPtr) delete currentLevelPtr;
//        window.close();
//    }
//
//    std::string getPlayerName(sf::RenderWindow& window);
//
//    void loadLevel(int index) {
//        if (currentLevelPtr) { delete currentLevelPtr; }
//        currentLevelPtr = nullptr;
//        currentLevelIndex = index;
//
//        switch (index) {
//        case 0: currentLevelPtr = new Level1(); break;
//        case 1: currentLevelPtr = new Level2(); break;
//        case 2: currentLevelPtr = new Level3(); break;
//        case 3: currentLevelPtr = new BossLevel(); break;
//        default: break;
//        }
//
//        if (currentLevelPtr) {
//            currentLevelPtr->setup(&player);
//            score = 0;
//            scoreText.setString("Score: 0");
//        }
//    }
//
//    void run() {
//        int state = STATE_MENU;
//        sf::Clock frameClock;
//
//        while (window.isOpen() && state != STATE_EXIT) {
//            sf::Event ev;
//            while (window.pollEvent(ev)) {
//                if (ev.type == sf::Event::Closed) state = STATE_EXIT;
//
//                if (state == STATE_MENU) {
//                    int sel = menu.navigate(ev);
//                    if (sel >= 0) {
//                        switch (sel) {
//                        case Menu::NEW_GAME:
//                            getPlayerName(window);
//                            loadLevel(0);
//                            state = STATE_PLAY;
//                            break;
//                        case Menu::CONTINUE_GAME:
//                        {
//                            GameState saved;
//                            if (saveSystem.load(saved)) {
//                                getPlayerName(window); // to set player name internally
//                                loadLevel(saved.levelIndex);
//                                // restore details
//                                player.setHP(saved.hp);
//                                player.setCharacter(saved.characterId);
//                                player.setPosition(saved.posX, saved.posY);
//                                player.setRings(saved.rings);
//                                player.setEnemiesDefeated(saved.enemiesDefeated);
//                                score = saved.score;
//                                player.setName(saved.playerName);
//                                scoreText.setString("Score: " + std::to_string(score));
//                                state = STATE_PLAY;
//                            }
//                        }
//                        break;
//                        case Menu::SELECT_LEVEL:
//                            state = STATE_SELECT_LEVEL;
//                            break;
//                        case Menu::LEADERBOARD:
//                            // TODO: display leaderboard
//                            break;
//                        case Menu::EXIT:
//                            state = STATE_EXIT;
//                            break;
//                        }
//                    }
//                }
//                else if (state == STATE_SELECT_LEVEL) {
//                    int lvl = levelManager.navigate(ev);
//                    if (lvl > 0 && lvl <= LEVEL_COUNT) {
//                        getPlayerName(window);
//                        loadLevel(lvl - 1);
//                        state = STATE_PLAY;
//                    }
//                }
//            }
//
//            float dt = frameClock.restart().asSeconds();
//            if (dt > 0.05f) dt = 0.05f;
//
//            window.clear();
//            if (state == STATE_MENU) menu.draw(window);
//            else if (state == STATE_SELECT_LEVEL) levelManager.draw(window);
//            else if (state == STATE_PLAY && currentLevelPtr) {
//                // existing play logic unchanged...
//                // [omitted for brevity]
//            }
//            window.display();
//        }
//    }
//};
