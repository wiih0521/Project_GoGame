#include "AI.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

AI::AI(Difficulty level) : difficulty(level) {
    srand(time(NULL));
}

Move AI::findBestMove(Board& board, Stone player) {
    switch (difficulty) {
    case Difficulty::Easy:
        return findRandomMove(board, player);
    case Difficulty::Medium:
        return findMinimaxMove(board, player, false); // Minimax không có Alpha-Beta
    case Difficulty::Hard:
        return findMinimaxMove(board, player, true); // Minimax với Alpha-Beta
    }
    return { -1, -1, Stone::None };
}

Move AI::findRandomMove(Board& board, Stone player) {
    std::vector<Move> possibleMoves;
    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) == Stone::None) {
                // Để đơn giản, không kiểm tra tính hợp lệ sâu
                possibleMoves.push_back({ r, c, player });
            }
        }
    }

    if (possibleMoves.empty()) return { -1, -1, Stone::None };
    return possibleMoves[rand() % possibleMoves.size()];
}

Move AI::findMinimaxMove(Board& board, Stone player, bool useAlphaBeta) {
    int bestScore = -100000;
    Move bestMove = { -1, -1, Stone::None };

    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) == Stone::None) {
                Board tempBoard = board;
                if (tempBoard.placeStone(r, c, player)) {
                    int score = minimax(tempBoard, 2, false, player, -100000, 100000, useAlphaBeta); // Độ sâu 2
                    if (score > bestScore) {
                        bestScore = score;
                        bestMove = { r, c, player };
                    }
                }
            }
        }
    }
    return bestMove;
}

int AI::minimax(Board board, int depth, bool isMaximizingPlayer, Stone aiPlayer, int alpha, int beta, bool useAlphaBeta) {
    if (depth == 0) {
        return evaluateBoard(board, aiPlayer);
    }

    Stone currentPlayer = isMaximizingPlayer ? aiPlayer : (aiPlayer == Stone::Black ? Stone::White : Stone::Black);

    if (isMaximizingPlayer) {
        int maxEval = -100000;
        // ... (Triển khai logic tìm max)
        return maxEval;
    }
    else {
        int minEval = 100000;
        // ... (Triển khai logic tìm min)
        return minEval;
    }
    // Đây là phần phức tạp nhất, cần triển khai đệ quy và cắt tỉa Alpha-Beta
    return 0;
}

// Hàm đánh giá đơn giản: chỉ đếm số quân
int AI::evaluateBoard(const Board& board, Stone aiPlayer) {
    int score = 0;
    Stone opponentPlayer = (aiPlayer == Stone::Black) ? Stone::White : Stone::Black;
    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) == aiPlayer) score++;
            else if (board.getStone(r, c) == opponentPlayer) score--;
        }
    }
    return score;
}