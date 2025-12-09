#pragma once
#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <utility>
#include <SFML/Graphics.hpp>

enum class Stone { None, Black, White };

struct Move {
    int row, col;
    Stone player;
};

const int dr[] = { -1, 1, 0, 0 };
const int dc[] = { 0, 0, -1, 1 };

class Board {
public:
    Board(const int size = 19);

    int boardSize;
    int whiteCapture, blackCapture;
    int consecutivePasses;

    std::vector<std::vector<Stone>> grid;
    std::vector<std::vector<Stone>> lastGrid;

    int getSize() const;
    Stone getStone(int row, int col) const;
    bool placeStone(int row, int col, Stone player);
    int countLiberties(int row, int col, Stone player) const;
    int removeGroup(int startRow, int startCol, Stone playerToRemove);
    bool isWithinBounds(int row, int col) const;
    
    
    void reset();
    std::pair<int, int> calculateScores() const;
    const std::pair<std::vector<std::vector<Stone>>, std::pair<int, int>> getBoardState() const;
    const std::vector<std::vector<Stone>>& getLastBoardState() const;

    void printToConsole() const;
    void printToConsole(const std::vector<std::vector<Stone>>& boardToPrint, const std::string& title) const;

    void setBoardState(Board new_state);
	void setBoardState(const std::pair<std::vector<std::vector<Stone>>, std::pair<int, int>>& state);
};

#endif // BOARD_H