#pragma once
#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <SFML/Graphics.hpp>

// Định nghĩa các trạng thái trên ô cờ
enum class Stone { None, Black, White };

// Struct để lưu một nước đi
struct Move {
    int row, col;
    Stone player;
};

const int dr[] = { -1, 1, 0, 0 };
const int dc[] = { 0, 0, -1, 1 };

const float LeftborderSize = 40;
class Board {
public:
    Board(const int size = 19);

    bool placeStone(int row, int col, Stone player);
    Stone getStone(int row, int col) const;
    int getSize() const;
    void reset();
    void setBoardState(const std::vector<std::vector<Stone>>& state);
    const std::vector<std::vector<Stone>>& getBoardState() const;
    bool isWithinBounds(int row, int col) const;
    int boardSize;
    int whiteCapture, blackCapture;
    void printToConsole() const;
    void printToConsole(const std::vector<std::vector<Stone>>& boardToPrint, const std::string& title) const;
    std::pair<int, int> calculateScores() const;

    std::vector<std::vector<Stone>> grid;
    std::vector<std::vector<Stone>> lastGrid;

    /*BoardAssets() {
        if (!boardTexture.loadFromFile("assets/images/board.png")) {
            std::cerr << "Error loading board texture\n";
        }
        boardSprite.setTexture(boardTexture);
    }*/


    int countLiberties(int row, int col, Stone player) const;
    // void findAndRemoveCapturedStones(int row, int col, Stone opponentPlayer); // Không còn dùng
    // bool isSuicideMove(int row, int col, Stone player); // Không còn dùng

    // THÊM DÒNG NÀY VÀO Board.h
    int removeGroup(int startRow, int startCol, Stone playerToRemove);
    const std::vector<std::vector<Stone>>& getLastBoardState() const;
};

#endif // BOARD_H