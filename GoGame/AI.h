#pragma once
#ifndef AI_H
#define AI_H

#include "Board.h"
#include <utility>

enum class Difficulty { Easy, Medium, Hard };

class AI {
public:
    AI(Difficulty level);
    Move findBestMove(Board& board, Stone player);

private:
    Difficulty difficulty;

    Move findRandomMove(Board& board, Stone player);
    Move findMinimaxMove(Board& board, Stone player, bool useAlphaBeta);

    int minimax(Board board, int depth, bool isMaximizingPlayer, Stone aiPlayer, int alpha, int beta, bool useAlphaBeta);
    int evaluateBoard(const Board& board, Stone aiPlayer);
};

#endif // AI_H