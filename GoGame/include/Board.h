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

    bool placeStone(int row, int col, Stone player);
    Stone getStone(int row, int col) const;
    int getSize() const;
    void reset();
    void setBoardState(const std::pair<std::vector<std::vector<Stone>>, std::pair<int, int>> &state);
    const std::pair<std::vector<std::vector<Stone>>, std::pair<int, int>> getBoardState() const;
    bool isWithinBounds(int row, int col) const;
    int boardSize;
    int whiteCapture, blackCapture;
    void printToConsole() const;
    void printToConsole(const std::vector<std::vector<Stone>>& boardToPrint, const std::string& title) const;
    std::pair<int, int> calculateScores() const;

    std::vector<std::vector<Stone>> grid;
    std::vector<std::vector<Stone>> lastGrid;

    int countLiberties(int row, int col, Stone player) const;

    int removeGroup(int startRow, int startCol, Stone playerToRemove);
    const std::vector<std::vector<Stone>>& getLastBoardState() const;
};

#endif // BOARD_H