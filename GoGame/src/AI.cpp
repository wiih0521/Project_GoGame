#include "../include/AI.h"
#include "../include/Board.h"
#include <vector>
#include <random>
#include <algorithm>
#include <limits>
#include <iostream>
#include <set>
#include <queue>

// Khởi tạo engine cho random move (chỉ một lần và toàn cục)
std::random_device rd;
std::mt19937 g(rd());

AI::AI(Difficulty level) : difficulty(level) {}

// Hàm mô phỏng có kiểm tra luật Ko
Board AI::placeStoneSimulated(const Board& currentBoard, int row, int col, Stone player) const {
    Board tempBoard = currentBoard;

    // Kiểm tra vị trí hợp lệ và ô trống
    if (!tempBoard.isWithinBounds(row, col) || tempBoard.getStone(row, col) != Stone::None) {
        return currentBoard;
    }

    // Lấy trạng thái trước đó TỪ BẢN GỐC để kiểm tra Ko
    const auto& lastStateFromOriginalBoard = currentBoard.getLastBoardState();

    tempBoard.grid[row][col] = player;

    Stone opponent = (player == Stone::Black) ? Stone::White : Stone::Black;
    bool capturedAny = false;
    int numCapture = 0;

    // Kiểm tra và bắt quân đối phương xung quanh
    int dr[] = { -1, 1, 0, 0 };
    int dc[] = { 0, 0, -1, 1 };

    for (int i = 0; i < 4; ++i) {
        int nr = row + dr[i];
        int nc = col + dc[i];
        if (tempBoard.isWithinBounds(nr, nc) && tempBoard.getStone(nr, nc) == opponent) {
            // Kiểm tra xem nhóm quân đối thủ này có 0 tự do sau khi đặt quân của mình không
            // Lưu ý: countLiberties sẽ tự động tìm các quân liên kết
            if (tempBoard.countLiberties(nr, nc, opponent) == 0) {
                numCapture += tempBoard.removeGroup(nr, nc, opponent);
                capturedAny = true;
            }
        }
    }

    // Kiểm tra nước đi tự sát cho quân vừa đặt (nếu không bắt được gì)
    if (!capturedAny && tempBoard.countLiberties(row, col, player) == 0) {
        return currentBoard;
    }

    // SỬA LỖI KO: So sánh trạng thái mới với trạng thái của lượt đi trước đó
    if (tempBoard.getBoardState() == lastStateFromOriginalBoard) {
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

// Sửa lại getValidMoves để sử dụng hàm mô phỏng đã sửa
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
    int moveCount = 0;
    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) != Stone::None) moveCount++;
        }
    }

    // Nếu là 1-2 nước đi đầu tiên, ưu tiên trung tâm
    // (chỉ kiểm tra nếu bàn cờ trống hoặc có ít quân)
    if (moveCount < 2) {
        int center = board.getSize() / 2;
        if (board.getStone(center, center) == Stone::None) {
            // Tạo bản sao để kiểm tra nước đi có hợp lệ trên bàn cờ thật không (kiểm tra tự sát/Ko)
            Board tempCheckBoard = board;
            // Sử dụng placeStone của Board (có kiểm tra Ko)
            if (tempCheckBoard.placeStone(center, center, player)) {
                // Nếu hợp lệ, trả về nước đi trung tâm
                return { center, center, player };
            }
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

// **Đã sửa:** getValidMoves sử dụng placeStoneSimulated và là const
std::vector<Move> AI::getValidMoves(const Board& board, Stone player) const {
    std::vector<Move> validMoves;
    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) == Stone::None) {
                // Sử dụng placeStoneSimulated để kiểm tra nước đi hợp lệ (chỉ kiểm tra ô trống và tự sát)
                Board simulatedBoard = placeStoneSimulated(board, r, c, player);
                // Nếu simulatedBoard KHÔNG giống currentBoard, tức là nước đi hợp lệ
                if (!(simulatedBoard.getBoardState() == board.getBoardState())) {
                    validMoves.push_back({ r, c, player });
                }
            }
        }
    }
    return validMoves;
}

// Chuyển đổi từ `get_game_stage` và là const
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
// **Đã sửa:** Thêm const
Move AI::findRandomMove(const Board& board, Stone player) const {
    auto validMoves = getValidMoves(board, player);
    if (validMoves.empty()) {
        std::cerr << "AI (Easy) could not find valid move, passing.\n";
        return { -1, -1, player };
    }

    // Sử dụng g đã được khởi tạo toàn cục
    std::shuffle(validMoves.begin(), validMoves.end(), g);
    return validMoves[0];
}

// ==============================================================================
// ==                          REFACTORED AI LOGIC                             ==
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
    }
    return bestMove;
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

    // 1. Capture Score - Bắt quân là quan trọng nhất, giữ trọng số cao
    int initialOpponentStones = 0;
    int finalOpponentStones = 0;
    for (int r = 0; r < initialBoard.getSize(); ++r) {
        for (int c = 0; c < initialBoard.getSize(); ++c) {
            if (initialBoard.getStone(r, c) == opponent) initialOpponentStones++;
            if (nextBoard.getStone(r, c) == opponent) finalOpponentStones++;
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

    // 3. Synergy Score - **GIẢM MẠNH** trọng số kết nối
    for (const auto& neighbor : getNeighbors(move.row, move.col, nextBoard.getSize())) {
        if (nextBoard.getStone(neighbor.first, neighbor.second) == player) {
            score += 1; // Giảm từ 5 xuống 1, chỉ là một điểm cộng nhỏ
        }
    }

// AI Medium: Đánh giá nước đi tức thì, không cần Minimax để đảm bảo tốc độ.
// Giống với logic Medium cũ của bạn nhưng đã được làm gọn.
// THAY THẾ HÀM NÀY TRONG AI.cpp
// AI Medium giờ sẽ so sánh nước đi của mình với việc bỏ lượt.
Move AI::findMediumMove(const Board& board, Stone player) const {
    auto validMoves = getValidMoves(board, player);

    // MỚI: Tính điểm nếu AI quyết định bỏ lượt.
    // Điểm này là điểm của bàn cờ hiện tại, không thay đổi.
    int passScore = evaluateBoard(board, player);

    // MỚI: Mặc định nước đi tốt nhất là "pass" với điểm số tương ứng.
    int bestScore = passScore;
    Move bestMove = { -1, -1, player }; // Nước đi "pass"

    // Bây giờ, duyệt qua các nước đi thực tế để xem có nước nào tốt hơn việc pass không.
    for (const auto& move : validMoves) {
        Board tempBoard = placeStoneSimulated(board, move.row, move.col, player);
        if (tempBoard.getBoardState() == board.getBoardState()) continue;

        // Đánh giá trạng thái ngay sau nước đi.
        int currentScore = evaluateBoard(tempBoard, player);

        // Nếu điểm của nước đi này tốt hơn điểm của việc pass (hoặc nước tốt nhất trước đó), cập nhật nó.
        if (currentScore > bestScore) {
            bestScore = currentScore;
            bestMove = move;
        }
    }

    if (bestMove.row == -1) {
        std::cerr << "AI (Medium) chose to pass strategically.\n";
}

    return bestMove;
}

// === CHIẾN LƯỢG AI CẤP ĐỘ KHÓ (HARD) (từ evaluate_board.py) ===

// THAY THẾ HÀM NÀY TRONG AI.cpp
// AI Hard cũng sẽ so sánh các lựa chọn của mình với việc bỏ lượt.
Move AI::findHardMove(const Board& board, Stone player) const {
    auto validMoves = getValidMoves(board, player);

    int depth = 1; // Giữ độ sâu thấp để đảm bảo hiệu suất

    // MỚI: Tính điểm nếu AI quyết định bỏ lượt.
    // Đây là điểm của bàn cờ SAU KHI đối thủ đã trả lời nước đi "pass" của AI.
    // isMaximizing = false vì đây là lượt của đối thủ.
    int passScore = minimax(board, depth, false, player, true,
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max());

    // MỚI: Mặc định nước đi tốt nhất là "pass" với điểm số tương ứng.
    int bestScore = passScore;
    Move bestMove = { -1, -1, player }; // Nước đi "pass"

    // Duyệt qua các nước đi thực tế để xem có nước nào tốt hơn việc pass không.
    for (const auto& move : validMoves) {
        // **Đã sửa:** Sử dụng placeStoneSimulated
        Board tempBoard = placeStoneSimulated(board, move.row, move.col, player);
        if (tempBoard.getBoardState() == board.getBoardState()) continue;

        // Tính điểm cho nước đi này bằng cách nhìn vào phản ứng của đối thủ.
        int currentScore = minimax(tempBoard, depth, false, player, true,
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max());

        // Nếu điểm của nước đi này tốt hơn điểm của việc pass, cập nhật nó.
        if (currentScore > bestScore) {
            bestScore = currentScore;
            bestMove = move;
        }
    }
    return bestMove;
}

// **Đã sửa:** Thêm const
int AI::minimax(Board currentBoard, int depth, bool isMaximizing, Stone aiPlayer, int alpha, int beta) const {
    // Trường hợp cơ bản: Đạt đến độ sâu tối đa hoặc không còn nước đi
    if (depth == 0) {
        return evaluateBoard(currentBoard, aiPlayer);
    }

    Stone currentPlayer = isMaximizing ? aiPlayer : (aiPlayer == Stone::Black ? Stone::White : Stone::Black);
    auto moves = getValidMoves(currentBoard, currentPlayer);

    // Nếu không có nước đi hợp lệ, coi như pass và đánh giá bàn cờ
    if (moves.empty()) {
        // Có thể thêm logic pass vào đây
        return evaluateBoard(currentBoard, aiPlayer);
    }

    if (isMaximizing) {
        int maxEval = std::numeric_limits<int>::min();
        for (const auto& move : moves) {
            // **Đã sửa:** Sử dụng placeStoneSimulated
            Board tempBoard = placeStoneSimulated(currentBoard, move.row, move.col, currentPlayer);

            // Nếu nước đi giả định không hợp lệ, bỏ qua
            if (tempBoard.getBoardState() == currentBoard.getBoardState()) {
                continue;
            }

            int eval = minimax(tempBoard, depth - 1, false, aiPlayer, alpha, beta);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break; // Alpha-beta pruning
        }
        return maxEval;
    }
    else { // Minimizing player (opponent)
        int minEval = std::numeric_limits<int>::max();
        for (const auto& move : moves) {
            // **Đã sửa:** Sử dụng placeStoneSimulated
            Board tempBoard = placeStoneSimulated(currentBoard, move.row, move.col, currentPlayer);

    if (bestMove.row == -1) {
        std::cerr << "AI (Hard) chose to pass strategically.\n";
            }

    return bestMove;
}

// ==============================================================================
// == CÁC HÀM ĐÁNH GIÁ VÀ HÀM HỖ TRỢ - GIỮ NGUYÊN                               ==
// ==============================================================================
// ... Dán tất cả các hàm helper của bạn (evaluateBoard, countTerritory, 
// countAliveGroups, findGroup, countEyesForGroup, getNeighbors, v.v...) vào đây ...
// Ví dụ:
int AI::evaluateBoard(const Board& board, Stone player) const {
    Stone opponent = (player == Stone::Black) ? Stone::White : Stone::Black;
    int score = 0;

    // Trọng số cho các yếu tố (có thể tinh chỉnh)
    const int ALIVE_GROUP_WEIGHT = 200;
    const int TERRITORY_WEIGHT = 10;
    const int DEFENSIVE_STRUCTURE_WEIGHT = 5;
    const int CAPTURE_WEIGHT = 100;

    // Cân nhắc điểm số quân bắt được (số quân mình bắt được trừ số quân đối thủ bắt được)
    if (player == Stone::Black) {
        score += board.blackCapture * CAPTURE_WEIGHT;
        score -= board.whiteCapture * CAPTURE_WEIGHT;
    }
    else {
        score += board.whiteCapture * CAPTURE_WEIGHT;
        score -= board.blackCapture * CAPTURE_WEIGHT;
    }

    // Cộng điểm cho các nhóm quân sống của mình, trừ điểm của đối thủ
    score += countAliveGroups(board, player) * ALIVE_GROUP_WEIGHT;
    score -= countAliveGroups(board, opponent) * ALIVE_GROUP_WEIGHT;

    // Cộng điểm cho lãnh thổ của mình, trừ điểm của đối thủ
    score += countTerritory(board, player) * TERRITORY_WEIGHT;
    score -= countTerritory(board, opponent) * TERRITORY_WEIGHT;

    // Cộng điểm cho cấu trúc phòng thủ của mình, trừ điểm của đối thủ
    score += countDefensiveStructures(board, player) * DEFENSIVE_STRUCTURE_WEIGHT;
    score -= countDefensiveStructures(board, opponent) * DEFENSIVE_STRUCTURE_WEIGHT;

    return score;
}
// ... và các hàm còn lại ...
#pragma region HelperFunctions
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
int AI::countDefensiveStructures(const Board& board, Stone player) const {
    int highLibertyGroups = 0;
    std::vector<std::vector<bool>> visited(board.getSize(), std::vector<bool>(board.getSize(), false));
    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            if (board.getStone(r, c) == player && !visited[r][c]) {
                // Sử dụng BFS để tìm tất cả các quân trong nhóm
                std::vector<std::pair<int, int>> currentGroup;
                std::queue<std::pair<int, int>> q;

                q.push({ r, c });
                visited[r][c] = true;

                while (!q.empty()) {
                    auto current = q.front();
                    q.pop();
                    currentGroup.push_back(current);

                    for (const auto& neighbor : getNeighbors(current.first, current.second, board.getSize())) {
                        if (board.getStone(neighbor.first, neighbor.second) == player && !visited[neighbor.first][neighbor.second]) {
                            visited[neighbor.first][neighbor.second] = true;
                            q.push(neighbor);
                        }
                    }
                }

                // Sau khi tìm được nhóm, đếm tự do của nó
                // board.countLiberties đã làm việc này cho một quân trong nhóm,
                // và sẽ tự động tìm các tự do của cả nhóm liên kết.
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

// Chuyển đổi từ `count_captured_spaces` và là const
int AI::countTerritory(const Board& board, Stone player) const {
    int territory = 0;
    std::vector<std::vector<bool>> visited(board.getSize(), std::vector<bool>(board.getSize(), false));

    for (int r = 0; r < board.getSize(); ++r) {
        for (int c = 0; c < board.getSize(); ++c) {
            // Nếu là ô trống và chưa được thăm, bắt đầu khám phá lãnh thổ
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

                    // Kiểm tra 4 hướng xung quanh
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
                // Nếu vùng lãnh thổ chỉ được bao quanh bởi quân của một người chơi (và đó là 'player')
                if (borderColors.size() == 1 && *borderColors.begin() == player) {
                    territory += area.size();
                }
            }
        }
    }
    return territory;
}

// Chuyển đổi từ `count_alive_groups` và là const
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

// Hàm trợ giúp để tìm một nhóm quân và là const
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

// Hàm trợ giúp để đếm mắt cho một nhóm và là const
int AI::countEyesForGroup(const Board& board, const std::vector<std::pair<int, int>>& group) const {
    int eyeCount = 0;
    std::set<std::pair<int, int>> potentialEyes;
    Stone player = group.empty() ? Stone::None : board.getStone(group[0].first, group[0].second);

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
        // Một mắt thật phải được bao quanh hoàn toàn bởi quân của nhóm và không có đối thủ kề cận
        for (const auto& eyeNeighbor : getNeighbors(eye.first, eye.second, board.getSize())) {
            Stone neighborStone = board.getStone(eyeNeighbor.first, eyeNeighbor.second);

            // Nếu có quân đối thủ kề cận mắt, thì đây không phải là mắt thật
            if (neighborStone != Stone::None && neighborStone != player) {
                isRealEye = false;
                break;
            }
            // Nếu có ô trống kề cận mà không phải là một phần của nhóm mình
            // (Điều này phức tạp hơn và có thể cần các kiểm tra sâu hơn cho mắt giả)
            // Hiện tại, giữ đơn giản: nếu nó được bao quanh bởi quân của mình và không có đối thủ, coi là mắt.
        }
        if (isRealEye) {
            eyeCount++;
        }
    }
    return eyeCount;
}

#pragma endregion