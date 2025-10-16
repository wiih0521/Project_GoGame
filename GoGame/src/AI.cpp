#include "../include/AI.h"
#include "../include/Board.h" // Đảm bảo include Board.h
#include <vector>
#include <random>
#include <algorithm> // Để dùng std::shuffle, std::min, std::max
#include <limits>    // Để dùng std::numeric_limits
#include <iostream>  // Để dùng std::cerr
#include <set>       // Để dùng std::set trong countTerritory
#include <queue>     // Để dùng std::queue trong BFS

// Khởi tạo engine cho random move (chỉ một lần và toàn cục)
std::random_device rd;
std::mt19937 g(rd());

AI::AI(Difficulty level) : difficulty(level) {}

// Hàm mô phỏng việc đặt quân cho AI, bỏ qua luật Ko và không sửa đổi lastGrid
// TRẢ VỀ một bản sao của Board sau khi đặt quân.
Board AI::placeStoneSimulated(const Board& currentBoard, int row, int col, Stone player) const {
    Board tempBoard = currentBoard; // Tạo một bản sao của bàn cờ hiện tại

    // Kiểm tra vị trí hợp lệ và ô trống
    if (!tempBoard.isWithinBounds(row, col) || tempBoard.getStone(row, col) != Stone::None) {
        return currentBoard; // Nước đi không hợp lệ, trả về bàn cờ gốc (không thay đổi)
    }

    tempBoard.grid[row][col] = player; // Tạm thời đặt quân

    Stone opponent = (player == Stone::Black) ? Stone::White : Stone::Black;
    bool capturedAny = false;
    int numCapture = 0; // Số quân bị bắt trong nước đi này

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
                numCapture += tempBoard.removeGroup(nr, nc, opponent); // removeGroup sửa đổi tempBoard.grid
                capturedAny = true;
            }
        }
    }

    // Kiểm tra nước đi tự sát cho quân vừa đặt (nếu không bắt được gì)
    if (!capturedAny && tempBoard.countLiberties(row, col, player) == 0) {
        return currentBoard; // Nước đi tự sát, trả về bàn cờ gốc (không thay đổi)
    }

    // Cập nhật số quân bắt được trong tempBoard
    // Các biến blackCapture/whiteCapture trong tempBoard sẽ được cập nhật
    if (player == Stone::Black) {
        tempBoard.blackCapture += numCapture;
    }
    else {
        tempBoard.whiteCapture += numCapture;
    }

    return tempBoard; // Trả về bản sao của bàn cờ sau nước đi mô phỏng hợp lệ
}


Move AI::findBestMove(const Board& board, Stone player) const { // Đảm bảo const ở đây
    // Logic gợi ý nước đi đầu game (từ helper_functions.py)
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
        return { -1, -1, player }; // Trả về nước đi "pass" nếu không có nước đi hợp lệ
    }

    // Sử dụng g đã được khởi tạo toàn cục
    std::shuffle(validMoves.begin(), validMoves.end(), g);
    return validMoves[0];
}

// === CHIẾN LƯỢC AI CẤP ĐỘ TRUNG BÌNH (MEDIUM) (từ evaluate_move.py) ===
// **Đã sửa:** Thêm const
Move AI::findMediumMove(const Board& board, Stone player) const {
    auto validMoves = getValidMoves(board, player);
    if (validMoves.empty()) {
        std::cerr << "AI (Medium) could not find valid move, passing.\n";
        return { -1, -1, player };
    }

    Move bestMove = { -1, -1, player }; // Khởi tạo với nước đi "pass"
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

// Hàm tổng hợp điểm cho một nước đi, dựa trên logic của evaluate_move.py và là const
int AI::evaluateMove(const Board& initialBoard, const Move& move) const {
    int score = 0;
    Stone player = move.player;
    Stone opponent = (player == Stone::Black) ? Stone::White : Stone::Black;

    Board nextBoard = placeStoneSimulated(initialBoard, move.row, move.col, player);

    if (nextBoard.getBoardState() == initialBoard.getBoardState()) {
        return std::numeric_limits<int>::min();
    }

    // 1. Capture Score - Bắt quân là quan trọng nhất, giữ trọng số cao
    int initialOpponentStones = 0;
    int finalOpponentStones = 0;
    for (int r = 0; r < initialBoard.getSize(); ++r) {
        for (int c = 0; c < initialBoard.getSize(); ++c) {
            if (initialBoard.getStone(r, c) == opponent) initialOpponentStones++;
            if (nextBoard.getStone(r, c) == opponent) finalOpponentStones++;
        }
    }
    score += (initialOpponentStones - finalOpponentStones) * 100; // **Tăng trọng số bắt quân**

    // 2. Atari Score - Dồn đối thủ vào thế nguy hiểm cũng rất quan trọng
    for (const auto& neighbor : getNeighbors(move.row, move.col, nextBoard.getSize())) {
        if (nextBoard.getStone(neighbor.first, neighbor.second) == opponent) {
            if (nextBoard.countLiberties(neighbor.first, neighbor.second, opponent) == 1) {
                score += 35; // **Tăng trọng số atari**
            }
        }
    }

    // 3. Synergy Score - **GIẢM MẠNH** trọng số kết nối
    for (const auto& neighbor : getNeighbors(move.row, move.col, nextBoard.getSize())) {
        if (nextBoard.getStone(neighbor.first, neighbor.second) == player) {
            score += 1; // Giảm từ 5 xuống 1, chỉ là một điểm cộng nhỏ
        }
    }

    // 4. Corner/Edge Score - Tăng cường ưu tiên chiếm góc và trung tâm ở đầu game
    int edgeDist = std::min({ move.row, move.col, initialBoard.getSize() - 1 - move.row, initialBoard.getSize() - 1 - move.col });
    if (getGameStage(initialBoard) == 1) { // Giai đoạn đầu game
        if (edgeDist == 2 || edgeDist == 3) score += 5; // Ưu tiên các đường 3, 4
        if (edgeDist > 3) score += 3; // Ưu tiên trung tâm
        if (edgeDist < 2) score -= 5; // **Phạt nặng** các nước đi ở đường 1, 2
    }
    else { // Giữa và cuối game
        if (edgeDist == 0) score += 1;
    }


    // 5. Liberty Score - **GIẢM** trọng số của việc tăng khí
    int libertiesAfterMove = nextBoard.countLiberties(move.row, move.col, player);
    score += libertiesAfterMove; // Giảm từ *2 xuống còn *1

    // **THÊM MỚI:** Phạt các hình cờ "ngu ngốc" (Stupid Shapes)
    // Nếu một quân mới đặt vào mà nhóm của nó có nhiều quân nhưng chỉ có 2 khí (rất nguy hiểm)
    // Đây là một heuristic đơn giản để phạt hình cờ xấu như đường thẳng dài.
    if (libertiesAfterMove <= 2) {
        std::vector<std::pair<int, int>> group;
        std::vector<std::vector<bool>> visited(nextBoard.getSize(), std::vector<bool>(nextBoard.getSize(), false));
        findGroup(nextBoard, move.row, move.col, player, group, visited);
        if (group.size() > 3) {
            score -= 20; // Phạt nặng nếu tạo ra một nhóm lớn nhưng yếu (ít khí)
        }
    }

    return score;
}


// === CHIẾN LƯỢG AI CẤP ĐỘ KHÓ (HARD) (từ evaluate_board.py) ===

// **Đã sửa:** Thêm const
Move AI::findHardMove(const Board& board, Stone player) const {
    auto validMoves = getValidMoves(board, player);
    if (validMoves.empty()) {
        std::cerr << "AI (Hard) could not find valid move, passing.\n";
        return { -1, -1, player };
    }

    int bestScore = std::numeric_limits<int>::min();
    Move bestMove = { -1, -1, player }; // Khởi tạo với nước đi "pass"
    int depth = 2; // Giữ độ sâu nhỏ để đảm bảo hiệu suất

    for (const auto& move : validMoves) {
        // **Đã sửa:** Sử dụng placeStoneSimulated
        Board tempBoard = placeStoneSimulated(board, move.row, move.col, player);

        // Nếu nước đi giả định không hợp lệ (ví dụ: tự sát), bỏ qua
        if (tempBoard.getBoardState() == board.getBoardState()) {
            continue;
        }

        // Gọi minimax với trạng thái bàn cờ sau nước đi giả định
        // isMaximizing = false vì nước đi tiếp theo là của đối thủ (minimizing player)
        int currentScore = minimax(tempBoard, depth - 1, false, player,
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max());
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

            // Nếu nước đi giả định không hợp lệ, bỏ qua
            if (tempBoard.getBoardState() == currentBoard.getBoardState()) {
                continue;
            }

            int eval = minimax(tempBoard, depth - 1, true, aiPlayer, alpha, beta);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break; // Alpha-beta pruning
        }
        return minEval;
    }
}

// Hàm đánh giá trạng thái bàn cờ, dựa trên logic của evaluate_board.py và là const
int AI::evaluateBoard(const Board& board, Stone player) const {
    Stone opponent = (player == Stone::Black) ? Stone::White : Stone::Black;
    int score = 0;

    // Trọng số cho các yếu tố (có thể tinh chỉnh)
    const int ALIVE_GROUP_WEIGHT = 200;
    const int TERRITORY_WEIGHT = 10;
    const int DEFENSIVE_STRUCTURE_WEIGHT = 5;
    const int CAPTURE_WEIGHT = 100; // Trọng số cho quân đã bắt được

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

// Chuyển đổi từ `defensive_structures` và là const
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
                    if (board.countLiberties(currentGroup[0].first, currentGroup[0].second, player) >= 3) { // 3 tự do được coi là an toàn
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
                std::vector<std::pair<int, int>> area; // Lưu các ô trong vùng lãnh thổ hiện tại
                std::set<Stone> borderColors; // Lưu các màu quân cờ bao quanh vùng này

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
                            borderColors.insert(neighborStone); // Lưu màu quân cờ ở biên
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
                findGroup(board, r, c, player, group, visited); // Tìm và đánh dấu cả nhóm đã thăm
                if (countEyesForGroup(board, group) >= 2) { // Một nhóm "sống" cần ít nhất 2 mắt
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
    Stone player = group.empty() ? Stone::None : board.getStone(group[0].first, group[0].second); // Màu của nhóm

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