#include "../include/AI.h"
#include "../include/Board.h"
#include "../include/AI_hard.h"
#include <vector>
#include <random>
#include <algorithm>
#include <limits>
#include <iostream>
#include <set>
#include <queue>

std::random_device rd;
std::mt19937 g(rd());

AI::AI(Difficulty level) : difficulty(level) {}

Board AI::placeStoneSimulated(const Board& currentBoard, int row, int col, Stone player) const {
    Board tempBoard = currentBoard;

    if (!tempBoard.isWithinBounds(row, col) || tempBoard.getStone(row, col) != Stone::None) {
        return currentBoard;
    }

    const auto& lastStateFromOriginalBoard = currentBoard.getLastBoardState();

    tempBoard.grid[row][col] = player;

    Stone opponent = (player == Stone::Black) ? Stone::White : Stone::Black;
    bool capturedAny = false;
    int numCapture = 0;

    int dr[] = { -1, 1, 0, 0 };
    int dc[] = { 0, 0, -1, 1 };

    for (int i = 0; i < 4; ++i) {
        int nr = row + dr[i];
        int nc = col + dc[i];
        if (tempBoard.isWithinBounds(nr, nc) && tempBoard.getStone(nr, nc) == opponent) {
            if (tempBoard.countLiberties(nr, nc, opponent) == 0) {
                numCapture += tempBoard.removeGroup(nr, nc, opponent);
                capturedAny = true;
            }
        }
    }

    if (!capturedAny && tempBoard.countLiberties(row, col, player) == 0) {
        return currentBoard;
    }

    if (tempBoard.getBoardState().first == lastStateFromOriginalBoard) {
        return currentBoard;
    }

    if (player == Stone::Black) {
        tempBoard.blackCapture += numCapture;
    }
    else {
        tempBoard.whiteCapture += numCapture;
    }

    return tempBoard;
}

std::vector<Move> AI::getValidMoves(const Board& board, Stone player) const {
    std::vector<Move> validMoves;
    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) == Stone::None) {
                Board simulatedBoard = placeStoneSimulated(board, r, c, player);
                if (!(simulatedBoard.getBoardState() == board.getBoardState())) {
                    validMoves.push_back({ r, c, player });
                }
            }
        }
    }
    return validMoves;
}

Move AI::findBestMove(const Board& board, Stone player) const {
    /*int moveCount = 0;
    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) != Stone::None) moveCount++;
        }
    }

    if (moveCount < 2) {
        int center = board.getSize() / 2;
        Board tempCheckBoard = board;
        if (tempCheckBoard.placeStone(center, center, player)) {
            return { center, center, player };
        }
    }*/

	std::cerr << "AI is selecting move for difficulty level: ";
    std::cerr << (static_cast<int>(difficulty) == 0 ? "Easy\n" :
        (static_cast<int>(difficulty) == 1 ? "Medium\n" : "Hard\n")) << "\n";

    switch (difficulty) {
    case Difficulty::Easy:
        return findRandomMove(board, player);
    case Difficulty::Medium:
        return findMediumMove(board, player);
    case Difficulty::Hard:
		// std::cerr << "Something is missing\n";
        Move move = AIHard::getBestMove(player);
		std::cerr << "AI (Hard) selected move: (" << move.row << ", " << move.col << ")\n";
        return move;
    default:
        return findRandomMove(board, player);
    }
}

// ==============================================================================
// ==                          AI DIFFICULTY LEVELS                            ==
// ==============================================================================

Move AI::findRandomMove(const Board& board, Stone player) const {
    auto validMoves = getValidMoves(board, player);
    if (validMoves.empty()) {
        std::cerr << "AI (Easy) could not find valid move, passing.\n";
        return { -1, -1, player };
    }
    std::shuffle(validMoves.begin(), validMoves.end(), g);
    return validMoves[0];
}

Move AI::findMediumMove(const Board& board, Stone player) const {
    auto validMoves = getValidMoves(board, player);

    // Nếu không còn nước đi
    if (validMoves.empty()) return { -1, -1, player };

    std::shuffle(validMoves.begin(), validMoves.end(), g);

    int currentBoardScore = evaluateBoard(board, player);
    int bestScore = std::numeric_limits<int>::min();
    Move bestMove = validMoves[0]; // Mặc định chọn nước đầu tiên sau khi shuffle

    for (const auto& move : validMoves) {
        Board tempBoard = placeStoneSimulated(board, move.row, move.col, player);
        if (tempBoard.getBoardState() == board.getBoardState()) continue;

        int score = evaluateBoard(tempBoard, player);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

Move AI::findHardMove(const Board& board, Stone player) const {
    auto validMoves = getValidMoves(board, player);
    int depth = 1;

    int passScore = minimax(board, depth, false, player, true,
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max());

    int bestScore = passScore;
    Move bestMove = { -1, -1, player };

    for (const auto& move : validMoves) {
        Board tempBoard = placeStoneSimulated(board, move.row, move.col, player);
        if (tempBoard.getBoardState() == board.getBoardState()) continue;

        int currentScore = minimax(tempBoard, depth, false, player, true,
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max());

        if (currentScore > bestScore) {
            bestScore = currentScore;
            bestMove = move;
        }
    }

    if (bestMove.row == -1) {
        std::cerr << "AI (Hard) chose to pass strategically.\n";
    }
    return bestMove;
}

// ==============================================================================
// ==                          CORE MINIMAX ALGORITHM                          ==
// ==============================================================================

int AI::minimax(const Board& board, int depth, bool isMaximizing, Stone aiPlayer, bool usePruning, int alpha, int beta) const {
    if (depth == 0) {
        return evaluateBoard(board, aiPlayer);
    }

    Stone currentPlayer = isMaximizing ? aiPlayer : (aiPlayer == Stone::Black ? Stone::White : Stone::Black);
    auto moves = getValidMoves(board, currentPlayer);

    if (moves.empty()) {
        return evaluateBoard(board, aiPlayer);
    }

    if (isMaximizing) {
        int maxEval = std::numeric_limits<int>::min();
        for (const auto& move : moves) {
            Board tempBoard = placeStoneSimulated(board, move.row, move.col, currentPlayer);
            if (tempBoard.getBoardState() == board.getBoardState()) continue;

            int eval = minimax(tempBoard, depth - 1, false, aiPlayer, usePruning, alpha, beta);
            maxEval = std::max(maxEval, eval);

            if (usePruning) {
                alpha = std::max(alpha, eval);
                if (beta <= alpha) break;
            }
        }
        return maxEval;
    }
    else { // Minimizing player
        int minEval = std::numeric_limits<int>::max();
        for (const auto& move : moves) {
            Board tempBoard = placeStoneSimulated(board, move.row, move.col, currentPlayer);
            if (tempBoard.getBoardState() == board.getBoardState()) continue;

            int eval = minimax(tempBoard, depth - 1, true, aiPlayer, usePruning, alpha, beta);
            minEval = std::min(minEval, eval);

            if (usePruning) {
                beta = std::min(beta, eval);
                if (beta <= alpha) break;
            }
        }
        return minEval;
    }
}


// ==============================================================================
// ==                    EVALUATION & HELPER FUNCTIONS                         ==
// ==============================================================================

// Hàm tính điểm dựa trên vị trí quân cờ (Ưu tiên đường 3 và 4)
int AI::getPositionalScore(int r, int c, int boardSize) const {
    // Khoảng cách từ quân cờ đến mép bàn cờ gần nhất
    int distRow = std::min(r, boardSize - 1 - r);
    int distCol = std::min(c, boardSize - 1 - c);
    int minEdgeDist = std::min(distRow, distCol);

    // Đường biên (Line 1) - Index 0: Rất thấp (trừ khi để nối hoặc bắt quân)
    if (minEdgeDist == 0) return 1;
    // Đường 2 (Line 2) - Index 1: Thấp (thường là nước bò biên thụ động)
    if (minEdgeDist == 1) return 2;
    // Đường 3 (Line 3) - Index 2: Tốt (Lấy đất)
    if (minEdgeDist == 2) return 10;
    // Đường 4 (Line 4) - Index 3: Rất tốt (Lấy thế/Influence)
    if (minEdgeDist == 3) return 8;
    // Thiên nguyên/Trung tâm: Bình thường
    return 5;
}

int AI::evaluateBoard(const Board& board, Stone player) const {
    Stone opponent = (player == Stone::Black) ? Stone::White : Stone::Black;
    int score = 0;

    const int CAPTURE_WEIGHT = 80;     
    const int TERRITORY_WEIGHT = 15;    
    const int LIBERTY_WEIGHT = 3;      
    const int POSITION_WEIGHT = 1;      

    if (player == Stone::Black) {
        score += board.blackCapture * CAPTURE_WEIGHT;
        score -= board.whiteCapture * CAPTURE_WEIGHT;
    }
    else {
        score += board.whiteCapture * CAPTURE_WEIGHT;
        score -= board.blackCapture * CAPTURE_WEIGHT;
    }

    int myLiberties = 0;
    int oppLiberties = 0;
    int myPosScore = 0;
    int oppPosScore = 0;

    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            Stone s = board.getStone(r, c);
            if (s == player) {
                myLiberties += board.countLiberties(r, c, player);
                myPosScore += getPositionalScore(r, c, board.getSize());
            }
            else if (s == opponent) {
                oppLiberties += board.countLiberties(r, c, opponent);
                oppPosScore += getPositionalScore(r, c, board.getSize());
            }
        }
    }

    score += myLiberties * LIBERTY_WEIGHT;
    score -= oppLiberties * LIBERTY_WEIGHT; 

    score += myPosScore * POSITION_WEIGHT;
    score -= oppPosScore * POSITION_WEIGHT;

    score += countTerritory(board, player) * TERRITORY_WEIGHT;
    score -= countTerritory(board, opponent) * TERRITORY_WEIGHT;

    return score;
}

std::vector<std::pair<int, int>> AI::getNeighbors(int r, int c, int boardSize) const {
    std::vector<std::pair<int, int>> neighbors;
    int dr[] = { -1, 1, 0, 0 };
    int dc[] = { 0, 0, -1, 1 };
    for (int i = 0; i < 4; ++i) {
        int nr = r + dr[i];
        int nc = c + dc[i];
        if (nr >= 0 && nr < boardSize && nc >= 0 && nc < boardSize) {
            neighbors.push_back({ nr, nc });
        }
    }
    return neighbors;
}

int AI::getGameStage(const Board& board) const {
    int boardSize = board.getSize();
    int totalPoints = boardSize * boardSize;
    int moveCount = 0;
    for (int r = 0; r < boardSize; ++r) {
        for (int c = 0; c < boardSize; ++c) {
            if (board.getStone(r, c) != Stone::None) moveCount++;
        }
    }
    float progress = static_cast<float>(moveCount) / totalPoints;
    if (progress < 0.33) return 1; // Đầu game
    if (progress < 0.66) return 2; // Giữa game
    return 3; // Cuối game
}

int AI::countDefensiveStructures(const Board& board, Stone player) const {
    int highLibertyGroups = 0;
    std::vector<std::vector<bool>> visited(board.getSize(), std::vector<bool>(board.getSize(), false));
    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) == player && !visited[r][c]) {
                std::vector<std::pair<int, int>> currentGroup;
                findGroup(board, r, c, player, currentGroup, visited);
                if (!currentGroup.empty()) {
                    if (board.countLiberties(currentGroup[0].first, currentGroup[0].second, player) >= 3) {
                        highLibertyGroups++;
                    }
                }
            }
        }
    }
    return highLibertyGroups;
}

int AI::countTerritory(const Board& board, Stone player) const {
    int territory = 0;
    std::vector<std::vector<bool>> visited(board.getSize(), std::vector<bool>(board.getSize(), false));
    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) == Stone::None && !visited[r][c]) {
                std::queue<std::pair<int, int>> q;
                std::vector<std::pair<int, int>> area;
                std::set<Stone> borderColors;
                q.push({ r, c });
                visited[r][c] = true;
                while (!q.empty()) {
                    std::pair<int, int> current = q.front();
                    q.pop();
                    area.push_back(current);
                    for (const auto& neighbor : getNeighbors(current.first, current.second, board.getSize())) {
                        Stone neighborStone = board.getStone(neighbor.first, neighbor.second);
                        if (neighborStone == Stone::None) {
                            if (!visited[neighbor.first][neighbor.second]) {
                                visited[neighbor.first][neighbor.second] = true;
                                q.push(neighbor);
                            }
                        }
                        else {
                            borderColors.insert(neighborStone);
                        }
                    }
                }
                if (borderColors.size() == 1 && *borderColors.begin() == player) {
                    territory += area.size();
                }
            }
        }
    }
    return territory;
}

int AI::countAliveGroups(const Board& board, Stone player) const {
    int aliveGroups = 0;
    std::vector<std::vector<bool>> visited(board.getSize(), std::vector<bool>(board.getSize(), false));
    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) == player && !visited[r][c]) {
                std::vector<std::pair<int, int>> group;
                findGroup(board, r, c, player, group, visited);
                if (countEyesForGroup(board, group) >= 2) {
                    aliveGroups++;
                }
            }
        }
    }
    return aliveGroups;
}

void AI::findGroup(const Board& board, int r, int c, Stone player, std::vector<std::pair<int, int>>& group, std::vector<std::vector<bool>>& visited) const {
    std::queue<std::pair<int, int>> q;
    q.push({ r, c });
    visited[r][c] = true;
    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        group.push_back(current);
        for (const auto& neighbor : getNeighbors(current.first, current.second, board.getSize())) {
            if (board.getStone(neighbor.first, neighbor.second) == player && !visited[neighbor.first][neighbor.second]) {
                visited[neighbor.first][neighbor.second] = true;
                q.push(neighbor);
            }
        }
    }
}

int AI::countEyesForGroup(const Board& board, const std::vector<std::pair<int, int>>& group) const {
    int eyeCount = 0;
    std::set<std::pair<int, int>> potentialEyes;
    Stone player = group.empty() ? Stone::None : board.getStone(group[0].first, group[0].second);
    for (const auto& stone : group) {
        for (const auto& neighbor : getNeighbors(stone.first, stone.second, board.getSize())) {
            if (board.getStone(neighbor.first, neighbor.second) == Stone::None) {
                potentialEyes.insert(neighbor);
            }
        }
    }
    for (const auto& eye : potentialEyes) {
        bool isRealEye = true;
        for (const auto& eyeNeighbor : getNeighbors(eye.first, eye.second, board.getSize())) {
            Stone neighborStone = board.getStone(eyeNeighbor.first, eyeNeighbor.second);
            if (neighborStone != Stone::None && neighborStone != player) {
                isRealEye = false;
                break;
            }
        }
        if (isRealEye) {
            eyeCount++;
        }
    }
    return eyeCount;
}