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

class Board {
public:
    Board(int size = 19);

    void draw(sf::RenderWindow& window);
    bool placeStone(int row, int col, Stone player);
    Stone getStone(int row, int col) const;
    int getSize() const;
    void reset();
    void setBoardState(const std::vector<std::vector<Stone>>& state);
    const std::vector<std::vector<Stone>>& getBoardState() const;
    bool isWithinBounds(int row, int col) const;
    int boardSize;
    void printToConsole() const;
    void printToConsole(const std::vector<std::vector<Stone>>& boardToPrint, const std::string& title) const;

    std::vector<std::vector<Stone>> grid;
    std::vector<std::vector<Stone>> lastGrid;
    sf::Texture blackStoneTexture, whiteStoneTexture;
    sf::Sprite blackStoneSprite, whiteStoneSprite;
    
    sf::Texture BottomboardTexture;
    sf::Sprite BottomboardSprite;

    sf::Texture BottomLeftboardTexture;
    sf::Sprite BottomLeftboardSprite;

    sf::Texture BottomRightboardTexture;
    sf::Sprite BottomRightboardSprite;

    sf::Texture UpperboardTexture;
    sf::Sprite UpperboardSprite;

    sf::Texture UpperLeftboardTexture;
    sf::Sprite UpperLeftboardSprite;

    sf::Texture UpperRightboardTexture;
    sf::Sprite UpperRightboardSprite;

    sf::Texture LeftboardTexture;
    sf::Sprite LeftboardSprite;

    sf::Texture RightboardTexture;
    sf::Sprite RightboardSprite;

    sf::Texture CenterboardTexture;
    sf::Sprite CenterboardSprite;

    sf::Texture BackGroundTexture;
    sf::Sprite BackGroundSprite;

        /*BoardAssets() {
            if (!boardTexture.loadFromFile("assets/images/board.png")) {
                std::cerr << "Error loading board texture\n";
            }
            boardSprite.setTexture(boardTexture);
        }*/

    
    int countLiberties(int row, int col, Stone player);
    // void findAndRemoveCapturedStones(int row, int col, Stone opponentPlayer); // Không còn dùng
    // bool isSuicideMove(int row, int col, Stone player); // Không còn dùng

    // THÊM DÒNG NÀY VÀO Board.h
    void removeGroup(int startRow, int startCol, Stone playerToRemove);
};

#endif // BOARD_H