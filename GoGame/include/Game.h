#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> 
#include <stack>
#include <vector> 
#include <memory> 
#include "Board.h"
#include "AI.h"

enum class GameMode { PlayerVsPlayer, PlayerVsAI };

class Game {
public:

    Game();
    void run();
    void draw(sf::RenderWindow& window);

    void handlePlayerInput(const sf::Vector2i& mousePos);
    void startNewGame(GameMode mode, Difficulty diff = Difficulty::Easy);
    void startNewGame();
    void NewGame();
    bool isWithinBounds(int row, int col) const;
    bool saveGame();
    bool loadGame();
    bool undoMove();
    bool redoMove();
    void passTurn();
    bool AI_move();
	bool placeStone(int row, int col, Stone player);
    void handleEndGame();
    void updateLastGrid();
    std::pair<float, float> calculateFinalScores() const;
    int getBlackCaptured();
    int getWhiteCaptured();

    Board board;
    bool isGameOver;

    std::unique_ptr<AI> ai;

    GameMode currentMode;
    Difficulty currentDifficulty;
    Stone currentPlayer;
    
    // Stacks for Undo/Redo
    std::stack<std::pair<std::vector<std::vector<Stone>>, std::pair<int, int>>> moveHistory;
    std::stack<std::pair<std::vector<std::vector<Stone>>, std::pair<int, int>>> redoHistory;

    int consecutivePasses;
};

#endif // GAME_H