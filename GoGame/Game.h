#pragma once
#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <stack>
#include "Board.h"
#include "AI.h"

enum class GameMode { PlayerVsPlayer, PlayerVsAI };

class Game {
public:
    Game();
    void run();
    bool isWithinBounds(int row, int col) const; // Cho phép Game class kiểm tra vị trí click

    void processEvents();
    void update();
    void render();

    void handlePlayerInput(const sf::Vector2i& mousePos);
    void startNewGame(GameMode mode, Difficulty diff = Difficulty::Easy);
    void saveGame();
    void loadGame();
    void undoMove();
    void redoMove();

    sf::RenderWindow window;
    Board board;
    std::unique_ptr<AI> ai;

    GameMode currentMode;
    Difficulty currentDifficulty;
    Stone currentPlayer;
    bool isGameOver;

    // Stacks for Undo/Redo [cite: 85, 87]
    std::stack<std::vector<std::vector<Stone>>> moveHistory;
    std::stack<std::vector<std::vector<Stone>>> redoHistory;

    // UI elements
    sf::Font font;
    sf::Text turnIndicatorText;
    sf::Text notificationText;
};

#endif // GAME_H