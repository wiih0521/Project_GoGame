#ifndef AI_H
#define AI_H

#include "Board.h" // Nơi định nghĩa struct Move, Stone, và class Board
#include <vector>
#include <utility>

// Các enum vẫn giữ nguyên
enum class Difficulty { Easy, Medium, Hard };

class AI {
public:
    AI(Difficulty level);

    // Hàm public chính để tìm nước đi, không thay đổi
    Move findBestMove(const Board& board, Stone player) const;

private:
    Difficulty difficulty;

    // --- Các chiến lược chính ---
    Move findRandomMove(const Board& board, Stone player) const;
    Move findMediumMove(const Board& board, Stone player) const;
    Move findHardMove(const Board& board, Stone player) const;

    // --- Cập nhật thuật toán Minimax ---
    // Thay đổi: Thêm tham số 'usePruning' và truyền Board bằng tham chiếu hằng (&)
    // để tăng hiệu suất.
    int minimax(const Board& board, int depth, bool isMaximizing, Stone aiPlayer, bool usePruning, int alpha, int beta) const;

    // --- Các hàm trợ giúp và đánh giá ---

    // LOẠI BỎ: Hàm evaluateMove không còn được sử dụng.
    // int evaluateMove(const Board& initialBoard, const Move& move) const;

    // Hàm mô phỏng, chữ ký không đổi
    Board placeStoneSimulated(const Board& currentBoard, int row, int col, Stone player) const;

    std::vector<Move> getValidMoves(const Board& board, Stone player) const;

    // Các hàm đánh giá và helper khác không thay đổi
    int evaluateBoard(const Board& board, Stone player) const;
    std::vector<std::pair<int, int>> getNeighbors(int r, int c, int boardSize) const;
    int getGameStage(const Board& board) const;
    int countDefensiveStructures(const Board& board, Stone player) const;
    int countTerritory(const Board& board, Stone player) const;
    int countAliveGroups(const Board& board, Stone player) const;

    // Hàm trợ giúp cho việc tìm nhóm quân và mắt
    void findGroup(const Board& board, int r, int c, Stone player,
        std::vector<std::pair<int, int>>& group,
        std::vector<std::vector<bool>>& visited) const;
    int countEyesForGroup(const Board& board, const std::vector<std::pair<int, int>>& group) const;

    // **MỚI:** Hàm mô phỏng đặt quân cho AI mà không xử lý Ko và không sửa đổi lastGrid
    // Phải được khai báo ở đây!
};

#endif // AI_H