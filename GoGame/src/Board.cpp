#include "../include/Board.h"
#include <vector>
#include <iostream>
#include <queue>
#include <set>

Board::Board(int size) : boardSize(size), grid(size, std::vector<Stone>(size, Stone::None)), lastGrid(size, std::vector<Stone>(size, Stone::None)) {
    whiteCapture = 0;
    blackCapture = 0;
}

bool Board::isWithinBounds(int row, int col) const {
    return row >= 0 && row < Board::boardSize && col >= 0 && col < Board::boardSize;
}


bool Board::placeStone(int row, int col, Stone player) {
    if (!isWithinBounds(row, col) || grid[row][col] != Stone::None) {
        return false; // Vị trí không hợp lệ
    }
    std::vector<std::vector<Stone>> originalGrid = grid;
    // Tạm thời đặt quân cờ
    grid[row][col] = player;

    // Lưu trạng thái trước khi bắt quân để kiểm tra Ko và tự sát
    

    // Tìm và bắt quân đối phương xung quanh nước đi mới
    Stone opponent = (player == Stone::Black) ? Stone::White : Stone::Black;
    bool capturedAny = false; // Cờ báo hiệu có quân nào bị bắt không

    // Kiểm tra các ô xung quanh nước đi vừa rồi
    int dr[] = { -1, 1, 0, 0 };
    int dc[] = { 0, 0, -1, 1 };
    int numCapture = 0;
    for (int i = 0; i < 4; ++i) {
        int nr = row + dr[i];
        int nc = col + dc[i];
        if (isWithinBounds(nr, nc) && grid[nr][nc] == opponent) {
            // Chỉ kiểm tra nếu nhóm quân đối phương còn sống sau nước đi này
            if (countLiberties(nr, nc, opponent) == 0) {
                numCapture += removeGroup(nr, nc, opponent);
                capturedAny = true;
            }
        }
    }

    // Kiểm tra nước đi tự sát cho quân vừa đặt
    // Nếu nước đi của mình không bắt được quân nào VÀ không có tự do, đó là tự sát.
    /*printToConsole(grid, "BOARD STATE ");
    printToConsole(originalGrid, "ORIGINAL STATE (REVERTING TO)");
    printToConsole(lastGrid, "LAST STATE (KO)");*/
    
    if (!capturedAny && countLiberties(row, col, player) == 0) {
        grid = originalGrid; // Hoàn tác nước đi
        return false;
    }
    if(lastGrid == grid) {
        //std::cerr << "KO";
        grid = originalGrid; // Hoàn tác nước đi
		//std::cerr << "Reverted to original state due to KO rule violation.\n";
        return false;
	}
    lastGrid = originalGrid; // Cập nhật trạng thái trước đó để kiểm tra Ko
    if (player == Stone::Black) {
        // Đen đi, bắt quân của Trắng
        blackCapture += numCapture;
    }
    else {
        // Trắng đi, bắt quân của Đen
        whiteCapture += numCapture;
    }
    return true;
}

// Hàm isWithinBounds không đổi


// Hàm đếm tự do cho một nhóm quân bằng BFS
int Board::countLiberties(int startRow, int startCol, Stone player) const {
    if (!isWithinBounds(startRow, startCol) || grid[startRow][startCol] != player) {
        return 0; // Không phải quân của người chơi hoặc ô trống
    }

    std::queue<std::pair<int, int>> q;
    std::set<std::pair<int, int>> visitedGroupStones; // Theo dõi các quân trong nhóm đã thăm
    std::set<std::pair<int, int>> libertiesSet;       // Theo dõi các tự do đã đếm (để tránh đếm trùng)

    q.push({ startRow, startCol });
    visitedGroupStones.insert({ startRow, startCol });

    int dr[] = { -1, 1, 0, 0 };
    int dc[] = { 0, 0, -1, 1 };

    while (!q.empty()) {
        std::pair<int, int> current = q.front();
        q.pop();

        for (int i = 0; i < 4; ++i) {
            int nr = current.first + dr[i];
            int nc = current.second + dc[i];

            if (isWithinBounds(nr, nc)) {
                if (grid[nr][nc] == player) {
                    if (visitedGroupStones.find({ nr, nc }) == visitedGroupStones.end()) {
                        visitedGroupStones.insert({ nr, nc });
                        q.push({ nr, nc });
                    }
                }
                else if (grid[nr][nc] == Stone::None) {
                    // Nếu là ô trống và chưa được đếm là tự do cho nhóm này
                    if (libertiesSet.find({ nr, nc }) == libertiesSet.end()) {
                        libertiesSet.insert({ nr, nc });
                    }
                }
            }
        }
    }
    return libertiesSet.size(); // Trả về số lượng tự do duy nhất
}

void Board::printToConsole() const {
    std::cerr << "--- BOARD STATE (" << boardSize << "x" << boardSize << ") ---\n";
    // In chỉ số cột ở trên cùng để dễ nhìn
    std::cerr << "   ";
    for (int c = 0; c < boardSize; ++c) {
        std::cerr << c % 10 << " "; // In chữ số cuối của cột
    }
    std::cerr << "\n";

    for (int r = 0; r < boardSize; ++r) {
        // In chỉ số hàng ở đầu mỗi dòng
        std::cerr << r % 10 << " |";
        for (int c = 0; c < boardSize; ++c) {
            switch (grid[r][c]) {
            case Stone::None:
                std::cerr << " ."; // Dấu chấm cho ô trống
                break;
            case Stone::Black:
                std::cerr << " X"; // X cho quân Đen
                break;
            case Stone::White:
                std::cerr << " O"; // O cho quân Trắng
                break;
            }
        }
        std::cerr << " |" << std::endl; // Xuống dòng sau mỗi hàng
    }
    std::cerr << "--------------------------\n\n";
}
// Thêm hàm này vào cuối file Board.cpp

void Board::printToConsole(const std::vector<std::vector<Stone>>& boardToPrint, const std::string& title) const {
    std::cerr << "--- " << title << " (" << boardSize << "x" << boardSize << ") ---\n";
    // In chỉ số cột
    std::cerr << "   ";
    for (int c = 0; c < boardSize; ++c) {
        std::cerr << c % 10 << " ";
    }
    std::cerr << "\n";

    for (int r = 0; r < boardSize; ++r) {
        // In chỉ số hàng
        std::cerr << r % 10 << " |";
        for (int c = 0; c < boardSize; ++c) {
            // SỬ DỤNG 'boardToPrint' THAY VÌ 'grid'
            switch (boardToPrint[r][c]) {
            case Stone::None:
                std::cerr << " .";
                break;
            case Stone::Black:
                std::cerr << " X";
                break;
            case Stone::White:
                std::cerr << " O";
                break;
            }
        }
        std::cerr << " |" << std::endl;
    }
    std::cerr << "-------------------------------------\n\n";
}

// Hàm loại bỏ một nhóm quân cờ
int Board::removeGroup(int startRow, int startCol, Stone playerToRemove) {
    if (!isWithinBounds(startRow, startCol) || grid[startRow][startCol] != playerToRemove) {
        return 0; // Không phải quân cần xóa hoặc ô trống
    }

    std::queue<std::pair<int, int>> q;
    q.push({ startRow, startCol });
    grid[startRow][startCol] = Stone::None; // Xóa quân đầu tiên

    int dr[] = { -1, 1, 0, 0 };
    int dc[] = { 0, 0, -1, 1 };
    int numCapture = 0;
    while (!q.empty()) {
        std::pair<int, int> current = q.front();
        q.pop();
        ++numCapture;

        for (int i = 0; i < 4; ++i) {
            int nr = current.first + dr[i];
            int nc = current.second + dc[i];

            if (isWithinBounds(nr, nc) && grid[nr][nc] == playerToRemove) {
                grid[nr][nc] = Stone::None; // Xóa quân trong nhóm
                q.push({ nr, nc });
            }
        }
    }
    return numCapture;
}

std::pair<int, int> Board::calculateScores() const {
    int blackScore = 0;
    int whiteScore = 0;
    std::vector<std::vector<bool>> visited(boardSize, std::vector<bool>(boardSize, false));

    for (int r = 0; r < boardSize; ++r) {
        for (int c = 0; c < boardSize; ++c) {
            // Nếu là ô trống và chưa được thăm, bắt đầu khám phá lãnh thổ
            if (grid[r][c] == Stone::None && !visited[r][c]) {
                std::queue<std::pair<int, int>> q;
                std::vector<std::pair<int, int>> territoryPoints; // Lưu các ô trong vùng lãnh thổ hiện tại
                bool touchesBlack = false;
                bool touchesWhite = false;

                q.push({ r, c });
                visited[r][c] = true;
                territoryPoints.push_back({ r, c });

                while (!q.empty()) {
                    std::pair<int, int> current = q.front();
                    q.pop();

                    // Kiểm tra 4 hướng xung quanh
                    for (int i = 0; i < 4; ++i) {
                        int nr = current.first + dr[i];
                        int nc = current.second + dc[i];

                        if (isWithinBounds(nr, nc)) {
                            if (grid[nr][nc] == Stone::None && !visited[nr][nc]) {
                                visited[nr][nc] = true;
                                q.push({ nr, nc });
                                territoryPoints.push_back({ nr, nc });
                            }
                            else if (grid[nr][nc] == Stone::Black) {
                                touchesBlack = true;
                            }
                            else if (grid[nr][nc] == Stone::White) {
                                touchesWhite = true;
                            }
                        }
                    }
                }
                if (touchesBlack && !touchesWhite) {
                    blackScore += territoryPoints.size();
                }
                else if (!touchesBlack && touchesWhite) {
                    whiteScore += territoryPoints.size();
                }
            }
        }
    }

    // Theo luật chơi, điểm cuối cùng còn bao gồm cả số quân đã bắt được.
    // Hiện tại, hàm này chỉ đếm lãnh thổ. Bạn cần cộng thêm số quân đã bắt được trong lớp Game.
    return { blackScore, whiteScore };
}

// THÊM HÀM NÀY VÀO CUỐI FILE Board.cpp
const std::vector<std::vector<Stone>>& Board::getLastBoardState() const {
    return lastGrid;
}


Stone Board::getStone(int row, int col) const { return grid[row][col]; }
int Board::getSize() const { return boardSize; }
void Board::reset()
{
    grid.assign(boardSize, std::vector<Stone>(boardSize, Stone::None));
    lastGrid.assign(boardSize, std::vector<Stone>(boardSize, Stone::None));
    whiteCapture = 0;
    blackCapture = 0;
}
void Board::setBoardState(const std::vector<std::vector<Stone>>& state) { grid = state; }
const std::vector<std::vector<Stone>>& Board::getBoardState() const { return grid; }