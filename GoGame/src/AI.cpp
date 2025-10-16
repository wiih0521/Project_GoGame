#include "../include/AI.h"
#include "../include/board.h"
#include <vector>
#include <random>
#include <algorithm>
#include <limits>
#include <iostream>
#include <set>
#include <queue>

AI::AI(Difficulty level) : difficulty(level) {}

Move AI::findBestMove(Board& board, Stone player) {
    // Logic gợi ý nước đi đầu game (từ helper_functions.py)
    int moveCount = 0;
    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) != Stone::None) moveCount++;
        }
    }
    if (moveCount < 2) { // Nếu là 1-2 nước đi đầu tiên
        int center = board.getSize() / 2;
        if (board.getStone(center, center) == Stone::None) {
            return { center, center, player };
        }
    }

    switch (difficulty) {
    case Difficulty::Easy:
        return findRandomMove(board, player);
    case Difficulty::Medium:
        return findMediumMove(board, player);
    case Difficulty::Hard:
        return findHardMove(board, player);
    default:
        return findRandomMove(board, player);
    }
}


// === CÁC HÀM TRỢ GIÚP CHUNG (từ helper_functions.py) ===

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

std::vector<Move> AI::getValidMoves(const Board& board, Stone player) {
    std::vector<Move> validMoves;
    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) == Stone::None) {
                Board tempBoard = board;
                if (tempBoard.placeStone(r, c, player)) {
                    validMoves.push_back({ r, c, player });
                }
            }
        }
    }
    return validMoves;
}

// Chuyển đổi từ `get_game_stage`
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


// === CHIẾN LƯỢC AI CẤP ĐỘ DỄ (EASY) ===
Move AI::findRandomMove(const Board& board, Stone player) {
    auto validMoves = getValidMoves(board, player);
    if (validMoves.empty()) return { -1, -1, player };
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(validMoves.begin(), validMoves.end(), g);
    return validMoves[0];
}

// === CHIẾN LƯỢC AI CẤP ĐỘ TRUNG BÌNH (MEDIUM) (từ evaluate_move.py) ===

Move AI::findMediumMove(const Board& board, Stone player) {
    auto validMoves = getValidMoves(board, player);
    if (validMoves.empty()) return { -1, -1, player };

    Move bestMove = validMoves[0];
    int bestScore = std::numeric_limits<int>::min();

    for (const auto& move : validMoves) {
        int currentScore = evaluateMove(board, move);
        if (currentScore > bestScore) {
            bestScore = currentScore;
            bestMove = move;
        }
    }
    return bestMove;
}

// Hàm tổng hợp điểm cho một nước đi, dựa trên logic của evaluate_move.py
int AI::evaluateMove(const Board& initialBoard, const Move& move) {
    int score = 0;
    Stone player = move.player;
    Stone opponent = (player == Stone::Black) ? Stone::White : Stone::Black;

    Board nextBoard = initialBoard;
    nextBoard.placeStone(move.row, move.col, player);

    // 1. Capture Score
    int initialOpponentStones = 0;
    int finalOpponentStones = 0;
    for (int r = 0; r < initialBoard.getSize(); ++r) {
        for (int c = 0; c < initialBoard.getSize(); ++c) {
            if (initialBoard.getStone(r, c) == opponent) initialOpponentStones++;
            if (nextBoard.getStone(r, c) == opponent) finalOpponentStones++;
        }
    }
    score += (initialOpponentStones - finalOpponentStones) * 50; // Bắt quân là rất quan trọng

    // 2. Atari Score
    for (const auto& neighbor : getNeighbors(move.row, move.col, nextBoard.getSize())) {
        if (nextBoard.getStone(neighbor.first, neighbor.second) == opponent) {
            if (nextBoard.countLiberties(neighbor.first, neighbor.second, opponent) == 1) {
                score += 25; // Dồn đối phương vào thế atari
            }
        }
    }

    // 3. Blocking Score (giảm tự do của đối thủ)
    int initialOpponentLiberties = 0;
    int finalOpponentLiberties = 0;
    // (Logic này có thể phức tạp, tạm thời ước tính)
    // score += ...

    // 4. Synergy Score (kết nối với quân mình)
    for (const auto& neighbor : getNeighbors(move.row, move.col, nextBoard.getSize())) {
        if (nextBoard.getStone(neighbor.first, neighbor.second) == player) {
            score += 5;
        }
    }

    // 5. Corner/Edge Score
    int edgeDist = std::min({ move.row, move.col, initialBoard.getSize() - 1 - move.row, initialBoard.getSize() - 1 - move.col });
    if (edgeDist == 0) score += 2; // Cạnh
    if (edgeDist == 1) score += 1;
    // Các nước đi vào trung tâm được ưu tiên hơn ở đầu game
    if (getGameStage(initialBoard) == 1 && edgeDist > 2) score += 3;

    // 6. Liberty Score (tăng tự do cho chính mình)
    score += nextBoard.countLiberties(move.row, move.col, player);

    return score;
}


// === CHIẾN LƯỢG AI CẤP ĐỘ KHÓ (HARD) (từ evaluate_board.py) ===

Move AI::findHardMove(const Board& board, Stone player) {
    auto validMoves = getValidMoves(board, player);
    if (validMoves.empty()) return { -1, -1, player };

    int bestScore = std::numeric_limits<int>::min();
    Move bestMove = validMoves[0];
    int depth = 2; // Giữ độ sâu nhỏ để đảm bảo hiệu suất

    for (const auto& move : validMoves) {
        Board tempBoard = board;
        tempBoard.placeStone(move.row, move.col, player);
        int currentScore = minimax(tempBoard, depth - 1, false, player, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        if (currentScore > bestScore) {
            bestScore = currentScore;
            bestMove = move;
        }
    }
    return bestMove;
}

int AI::minimax(Board currentBoard, int depth, bool isMaximizing, Stone aiPlayer, int alpha, int beta) {
    if (depth == 0) {
        return evaluateBoard(currentBoard, aiPlayer);
    }

    Stone currentPlayer = isMaximizing ? aiPlayer : (aiPlayer == Stone::Black ? Stone::White : Stone::Black);
    auto moves = getValidMoves(currentBoard, currentPlayer);
    if (moves.empty()) {
        return evaluateBoard(currentBoard, aiPlayer);
    }

    if (isMaximizing) {
        int maxEval = std::numeric_limits<int>::min();
        for (const auto& move : moves) {
            Board tempBoard = currentBoard;
            tempBoard.placeStone(move.row, move.col, currentPlayer);
            int eval = minimax(tempBoard, depth - 1, false, aiPlayer, alpha, beta);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    }
    else {
        int minEval = std::numeric_limits<int>::max();
        for (const auto& move : moves) {
            Board tempBoard = currentBoard;
            tempBoard.placeStone(move.row, move.col, currentPlayer);
            int eval = minimax(tempBoard, depth - 1, true, aiPlayer, alpha, beta);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

// Hàm đánh giá trạng thái bàn cờ, dựa trên logic của evaluate_board.py
int AI::evaluateBoard(const Board& board, Stone player) {
    Stone opponent = (player == Stone::Black) ? Stone::White : Stone::Black;
    int score = 0;

    // Trọng số cho các yếu tố
    const int ALIVE_GROUP_WEIGHT = 200;
    const int TERRITORY_WEIGHT = 10;
    const int DEFENSIVE_STRUCTURE_WEIGHT = 5;

    score += countAliveGroups(board, player) * ALIVE_GROUP_WEIGHT;
    score -= countAliveGroups(board, opponent) * ALIVE_GROUP_WEIGHT;

    score += countTerritory(board, player) * TERRITORY_WEIGHT;
    score -= countTerritory(board, opponent) * TERRITORY_WEIGHT;

    score += countDefensiveStructures(board, player) * DEFENSIVE_STRUCTURE_WEIGHT;
    score -= countDefensiveStructures(board, opponent) * DEFENSIVE_STRUCTURE_WEIGHT;

    return score;
}

// Chuyển đổi từ `defensive_structures`
int AI::countDefensiveStructures(const Board& board, Stone player) const {
    int highLibertyGroups = 0;
    std::vector<std::vector<bool>> visited(board.getSize(), std::vector<bool>(board.getSize(), false));
    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) == player && !visited[r][c]) {
                if (board.countLiberties(r, c, player) >= 3) { // 3 tự do được coi là an toàn
                    highLibertyGroups++;
                }
                // Đánh dấu đã duyệt cả nhóm
                std::vector<std::pair<int, int>> group;
                findGroup(board, r, c, player, group, visited);
            }
        }
    }
    return highLibertyGroups;
}

// Chuyển đổi từ `count_captured_spaces`
int AI::countTerritory(const Board& board, Stone player) const {
    int territory = 0;
    std::vector<std::vector<bool>> visited(board.getSize(), std::vector<bool>(board.getSize(), false));
    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) == Stone::None && !visited[r][c]) {
                std::vector<std::pair<int, int>> area;
                std::set<Stone> borderColors;
                std::queue<std::pair<int, int>> q;

                q.push({ r, c });
                visited[r][c] = true;
                bool isBordered = true;

                while (!q.empty()) {
                    auto current = q.front();
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

// Chuyển đổi từ `count_alive_groups`
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

// Hàm trợ giúp để tìm một nhóm quân
void AI::findGroup(const Board& board, int r, int c, Stone player,
    std::vector<std::pair<int, int>>& group,
    std::vector<std::vector<bool>>& visited) const {
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

// Hàm trợ giúp để đếm mắt cho một nhóm, chuyển đổi từ `count_eyes` và `count_total_eyes`
int AI::countEyesForGroup(const Board& board, const std::vector<std::pair<int, int>>& group) const {
    int eyeCount = 0;
    std::set<std::pair<int, int>> potentialEyes;
    // Tìm các ô trống kề cận nhóm
    for (const auto& stone : group) {
        for (const auto& neighbor : getNeighbors(stone.first, stone.second, board.getSize())) {
            if (board.getStone(neighbor.first, neighbor.second) == Stone::None) {
                potentialEyes.insert(neighbor);
            }
        }
    }
    // Kiểm tra xem mỗi ô trống có phải là một "mắt" thực sự không
    for (const auto& eye : potentialEyes) {
        bool isRealEye = true;
        for (const auto& eyeNeighbor : getNeighbors(eye.first, eye.second, board.getSize())) {
            bool isPartOfGroup = false;
            for (const auto& stone : group) {
                if (stone.first == eyeNeighbor.first && stone.second == eyeNeighbor.second) {
                    isPartOfGroup = true;
                    break;
                }
            }
            if (!isPartOfGroup) {
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