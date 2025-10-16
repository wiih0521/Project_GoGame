#ifndef AI_H
#define AI_H

#include "Board.h" // Đảm bảo Board.h được include, nơi struct Move được định nghĩa
#include <utility>   // Để dùng std::pair
#include <vector>
#include <map>       // Để dùng std::map, mặc dù hiện tại không dùng trực tiếp trong AI.h

// KHÔNG CẦN định nghĩa struct Move ở đây nữa, vì nó đã có trong Board.h
// struct Move {
//     int row;
//     int col;
//     Stone player;
//
//     // Default constructor cho Move
//     Move(int r = -1, int c = -1, Stone p = Stone::None) : row(r), col(c), player(p) {}
//
//     // So sánh bằng (để kiểm tra nếu nước đi là "pass" hoặc không hợp lệ)
//     bool operator==(const Move& other) const {
//         return row == other.row && col == other.col && player == other.player;
//     }
// };

enum class Difficulty { Easy, Medium, Hard };

class AI {
public:
    AI(Difficulty level);

    // findBestMove KHÔNG NÊN là const vì nó cần gọi các hàm AI khác cũng là const
    // và nó sẽ làm việc với một bản sao Board nếu cần, không thay đổi board gốc.
    // Tuy nhiên, việc truyền Board& board thay vì const Board& board cho phép
    // AI tương tác với Board gốc (ví dụ: in debug, hoặc nếu Board có hàm không const mà AI cần).
    // Với mục đích hiện tại, giữ const Board& board là tốt nhất để đảm bảo AI không thay đổi trạng thái game.
    Move findBestMove(const Board& board, Stone player) const;

private:
    Difficulty difficulty;

    // --- Các chiến lược chính ---
    Move findRandomMove(const Board& board, Stone player) const;
    Move findMediumMove(const Board& board, Stone player) const;
    Move findHardMove(const Board& board, Stone player) const;

    // --- Các hàm trợ giúp chung ---
    std::vector<std::pair<int, int>> getNeighbors(int r, int c, int boardSize) const;

    // Hàm này sẽ dùng placeStoneSimulated
    std::vector<Move> getValidMoves(const Board& board, Stone player) const;

    int getGameStage(const Board& board) const;

    // --- Logic cho AI Medium ---
    int evaluateMove(const Board& initialBoard, const Move& move) const;

    // --- Logic cho AI Hard ---
    int minimax(Board currentBoard, int depth, bool isMaximizing, Stone aiPlayer, int alpha, int beta) const;
    int evaluateBoard(const Board& board, Stone player) const;

    // Các hàm đánh giá trạng thái bàn cờ
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
    Board placeStoneSimulated(const Board& currentBoard, int row, int col, Stone player) const;
};

#endif // AI_H