#ifndef THEME_H
#define THEME_H

#include <SFML/Graphics.hpp>
#include <string>

enum class BoardTheme {
    Classic,
    Dark,
    Minimalist,
    Blueprint,
    MAX_COUNT 
};

enum class StoneTheme {
    Standard,
    Realistic, // Bộ black_03 + white_05
    Flat,      // Bộ black_00 + white_00
    MAX_COUNT
};

struct BoardData {
    sf::Color boardColor;
    sf::Color borderColor;
    sf::Color lineColor;
    sf::Color starColor;
    float lineThickness;
    float starRadius;
};

struct StoneData {
    sf::Texture blackStoneTexture, whiteStoneTexture;
};

namespace ThemeManager {
    BoardData getThemeData(BoardTheme theme);
    std::string getThemeName(BoardTheme theme);
    BoardTheme getNextTheme(BoardTheme current);

    StoneData getStoneData(StoneTheme theme);
    std::string getStoneName(StoneTheme style);
    StoneTheme getNextStone(StoneTheme current);
}

#endif // THEME_H