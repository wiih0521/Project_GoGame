#include "../include/Board.h"
#include <vector>
#include <iostream>
#include <queue>
#include <set>

Board::Board(int size) : boardSize(size), grid(size, std::vector<Stone>(size, Stone::None)), lastGrid(size, std::vector<Stone>(size, Stone::None)) {
    whiteCapture = 0;
    blackCapture = 0;
    consecutivePasses = 0;
}

bool Board::isWithinBounds(int row, int col) const {
    return row >= 0 && row < Board::boardSize && col >= 0 && col < Board::boardSize;
}


bool Board::placeStone(int row, int col, Stone player) {
    if (!isWithinBounds(row, col) || grid[row][col] != Stone::None) {
        return false; 
    }
    std::vector<std::vector<Stone>> originalGrid = grid;
    grid[row][col] = player;

    Stone opponent = (player == Stone::Black) ? Stone::White : Stone::Black;
    bool capturedAny = false; 

    int dr[] = { -1, 1, 0, 0 };
    int dc[] = { 0, 0, -1, 1 };
    int numCapture = 0;
    for (int i = 0; i < 4; ++i) {
        int nr = row + dr[i];
        int nc = col + dc[i];
        if (isWithinBounds(nr, nc) && grid[nr][nc] == opponent) {
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
        grid = originalGrid; 
        return false;
    }
    if(lastGrid == grid) {
        //std::cerr << "KO";
        grid = originalGrid;
		//std::cerr << "Reverted to original state due to KO rule violation.\n";
        return false;
	}
    lastGrid = originalGrid; 
    if (player == Stone::Black) {
        blackCapture += numCapture;
    }
    else {
        whiteCapture += numCapture;
    }
    return true;
}

// Hàm đếm tự do cho một nhóm quân bằng BFS
int Board::countLiberties(int startRow, int startCol, Stone player) const {
    if (!isWithinBounds(startRow, startCol) || grid[startRow][startCol] != player) {
        return 0; 
    }

    std::queue<std::pair<int, int>> q;
    std::set<std::pair<int, int>> visitedGroupStones;
    std::set<std::pair<int, int>> libertiesSet;      

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
                    if (libertiesSet.find({ nr, nc }) == libertiesSet.end()) {
                        libertiesSet.insert({ nr, nc });
                    }
                }
            }
        }
    }
    return libertiesSet.size(); 
}

void Board::printToConsole() const {
    std::cerr << "--- BOARD STATE (" << boardSize << "x" << boardSize << ") ---\n";

    std::cerr << "   ";
    for (int c = 0; c < boardSize; ++c) {
        std::cerr << c % 10 << " "; 
    }
    std::cerr << "\n";

    for (int r = 0; r < boardSize; ++r) {
        std::cerr << r % 10 << " |";
        for (int c = 0; c < boardSize; ++c) {
            switch (grid[r][c]) {
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
    std::cerr << "--------------------------\n\n";
}

void Board::printToConsole(const std::vector<std::vector<Stone>>& boardToPrint, const std::string& title) const {
    std::cerr << "--- " << title << " (" << boardSize << "x" << boardSize << ") ---\n";
    std::cerr << "   ";
    for (int c = 0; c < boardSize; ++c) {
        std::cerr << c % 10 << " ";
    }
    std::cerr << "\n";

    for (int r = 0; r < boardSize; ++r) {
        std::cerr << r % 10 << " |";
        for (int c = 0; c < boardSize; ++c) {
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

int Board::removeGroup(int startRow, int startCol, Stone playerToRemove) {
    if (!isWithinBounds(startRow, startCol) || grid[startRow][startCol] != playerToRemove) {
        return 0; 
    }

    std::queue<std::pair<int, int>> q;
    q.push({ startRow, startCol });
    grid[startRow][startCol] = Stone::None;

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
                grid[nr][nc] = Stone::None;
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
            if (grid[r][c] == Stone::None && !visited[r][c]) {
                std::queue<std::pair<int, int>> q;
                std::vector<std::pair<int, int>> territoryPoints; 
                bool touchesBlack = false;
                bool touchesWhite = false;

                q.push({ r, c });
                visited[r][c] = true;
                territoryPoints.push_back({ r, c });

                while (!q.empty()) {
                    std::pair<int, int> current = q.front();
                    q.pop();

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

    return { blackScore, whiteScore };
}

const std::vector<std::vector<Stone>>& Board::getLastBoardState() const {
    return lastGrid;
}

Stone Board::getStone(int row, int col) const { return grid[row][col]; }
int Board::getSize() const { return boardSize; }
void Board::reset()
{
    grid.assign(boardSize, std::vector<Stone>(boardSize, Stone::None));
    lastGrid.assign(boardSize, std::vector<Stone>(boardSize, Stone::None));
    consecutivePasses = 0;
    whiteCapture = 0;
    blackCapture = 0;
}

const std::pair<std::vector<std::vector<Stone>>, std::pair<int, int>> Board::getBoardState() const { return std::make_pair(grid, std::make_pair(whiteCapture, blackCapture)); }

void Board::setBoardState(Board new_state) {
    grid = new_state.grid;
	lastGrid = new_state.lastGrid;
    whiteCapture = new_state.whiteCapture;
    blackCapture = new_state.blackCapture;
	consecutivePasses = new_state.consecutivePasses;
}

void Board::setBoardState(const std::pair<std::vector<std::vector<Stone>>, std::pair<int, int>>& state) {
    grid = state.first;
    lastGrid.assign(boardSize, std::vector<Stone>(boardSize, Stone::None));
    whiteCapture = state.second.first;
    blackCapture = state.second.second;
    consecutivePasses = 0;
}