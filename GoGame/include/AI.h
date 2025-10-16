#ifndef AI_H
#define AI_H

#include "Board.h"
#include <utility>
#include <vector>
#include <map>

enum class Difficulty { Easy, Medium, Hard };

class AI {
public:
    AI(Difficulty level);
    Move findBestMove(Board& board, Stone player);

private:
    Difficulty difficulty;

    // --- Các chiến lược chính ---
    Move findRandomMove(const Board& board, Stone player);
    Move findMediumMove(const Board& board, Stone player);
    Move findHardMove(const Board& board, Stone player);

    // --- Các hàm trợ giúp chung (từ helper_functions.py) ---
    std::vector<std::pair<int, int>> getNeighbors(int r, int c, int boardSize) const;
    std::vector<Move> getValidMoves(const Board& board, Stone player);
    int getGameStage(const Board& board) const;

    // --- Logic cho AI Medium (từ evaluate_move.py) ---
    int evaluateMove(const Board& initialBoard, const Move& move);

    // --- Logic cho AI Hard (từ evaluate_board.py) ---
    int minimax(Board currentBoard, int depth, bool isMaximizing, Stone aiPlayer, int alpha, int beta);
    int evaluateBoard(const Board& board, Stone player);

    // Các hàm đánh giá trạng thái bàn cờ (từ evaluate_board.py)
    int countDefensiveStructures(const Board& board, Stone player) const;
    int countTerritory(const Board& board, Stone player) const;
    int countAliveGroups(const Board& board, Stone player) const;

    // Hàm trợ giúp cho việc tìm nhóm quân và mắt
    void findGroup(const Board& board, int r, int c, Stone player,
        std::vector<std::pair<int, int>>& group,
        std::vector<std::vector<bool>>& visited) const;
    int countEyesForGroup(const Board& board, const std::vector<std::pair<int, int>>& group) const;
};

#endif // AI_H