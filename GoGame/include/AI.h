#ifndef AI_H
#define AI_H


#include "../include/Board.h"
#include "../include/AI_hard.h" 
#include <vector>
#include <utility>

enum class Difficulty { Easy, Medium, Hard };

class AI {
public:
    AI(Difficulty level);

    Move findBestMove(const Board& board, Stone player) const;

private:
    Difficulty difficulty;

    Move findRandomMove(const Board& board, Stone player) const;
    Move findMediumMove(const Board& board, Stone player) const;
    Move findHardMove(const Board& board, Stone player) const;

    // --- THUẬT TOÁN MINIMAX ---
    int minimax(const Board& board, int depth, bool isMaximizing, Stone aiPlayer, bool usePruning, int alpha, int beta) const;

    int getPositionalScore(int r, int c, int boardSize) const;

    // Hàm mô phỏng
    Board placeStoneSimulated(const Board& currentBoard, int row, int col, Stone player) const;

    std::vector<Move> getValidMoves(const Board& board, Stone player) const;
    int evaluateBoard(const Board& board, Stone player) const;
    std::vector<std::pair<int, int>> getNeighbors(int r, int c, int boardSize) const;
    int getGameStage(const Board& board) const;

    int countDefensiveStructures(const Board& board, Stone player) const;
    int countTerritory(const Board& board, Stone player) const;
    int countAliveGroups(const Board& board, Stone player) const;

    void findGroup(const Board& board, int r, int c, Stone player,
        std::vector<std::pair<int, int>>& group,
        std::vector<std::vector<bool>>& visited) const;

    int countEyesForGroup(const Board& board, const std::vector<std::pair<int, int>>& group) const;
};

#endif // AI_H