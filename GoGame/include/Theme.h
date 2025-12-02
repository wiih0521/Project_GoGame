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

struct ThemeData {
    sf::Color boardColor;
    sf::Color borderColor;
    sf::Color lineColor;
    sf::Color starColor;
    float lineThickness;
    float starRadius;
};

namespace ThemeManager {
    ThemeData getThemeData(BoardTheme theme);

    std::string getThemeName(BoardTheme theme);
    BoardTheme getNextTheme(BoardTheme current);
}

#endif // THEME_H