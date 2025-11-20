#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> // Thêm cho âm thanh
#include <stack>
#include <vector> // Cần thiết cho các trạng thái bàn cờ
#include <memory> // Cần thiết cho std::unique_ptr
#include "Board.h"
#include "AI.h"

enum class GameMode { PlayerVsPlayer, PlayerVsAI };
// enum class Difficulty { Easy, Medium, Hard }; // Đã định nghĩa trong AI.h, có thể bỏ ở đây nếu chỉ dùng AI.h

class Game {
public:

    Game();
    void run();
    void draw(sf::RenderWindow& window);

    void handlePlayerInput(const sf::Vector2i& mousePos);
    void startNewGame(GameMode mode, Difficulty diff = Difficulty::Easy);
    void startNewGame();
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

    Board board;
    bool isGameOver;

    std::unique_ptr<AI> ai;

    GameMode currentMode;
    Difficulty currentDifficulty;
    Stone currentPlayer;
    
    // Stacks for Undo/Redo
    std::stack<std::vector<std::vector<Stone>>> moveHistory;
    std::stack<std::vector<std::vector<Stone>>> redoHistory;

    int consecutivePasses;
};

#endif // GAME_H