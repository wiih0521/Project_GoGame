#include "../include/Theme.h" 

namespace ThemeManager {

    ThemeData getThemeData(BoardTheme theme) {
        ThemeData data;

        data.starRadius = 4.0f;
        data.lineThickness = 2.0f;

        switch (theme) {
        case BoardTheme::Classic:
            data.boardColor = sf::Color(229, 192, 126); 
            data.borderColor = sf::Color(80, 50, 20);   
            data.lineColor = sf::Color(20, 20, 20);    
            data.starColor = sf::Color(20, 20, 20);
            break;

        case BoardTheme::Dark:
            data.boardColor = sf::Color(20, 25, 35, 230);
            data.borderColor = sf::Color(0, 255, 255);  
            data.lineColor = sf::Color(0, 255, 255, 120);
            data.starColor = sf::Color(0, 255, 255);
            break;

        case BoardTheme::Minimalist:
            data.boardColor = sf::Color(245, 245, 245);
            data.borderColor = sf::Color(150, 150, 150);
            data.lineColor = sf::Color(50, 50, 50);
            data.starColor = sf::Color(50, 50, 50);
            break;

        case BoardTheme::Blueprint:
            data.boardColor = sf::Color(10, 50, 100);    
            data.borderColor = sf::Color(255, 255, 255);
            data.lineColor = sf::Color(255, 255, 255, 100);
            data.starColor = sf::Color(255, 255, 255);
            break;

        default:
            break;
        }
        return data;
    }

    std::string getThemeName(BoardTheme theme) {
        switch (theme) {
        case BoardTheme::Classic: return "Classic Wood";
        case BoardTheme::Dark: return "Cyberpunk";
        case BoardTheme::Minimalist: return "Minimalist";
        case BoardTheme::Blueprint: return "Blueprint";
        default: return "Unknown";
        }
    }

    BoardTheme getNextTheme(BoardTheme current) {
        int nextIndex = (static_cast<int>(current) + 1) % static_cast<int>(BoardTheme::MAX_COUNT);
        return static_cast<BoardTheme>(nextIndex);
    }
}