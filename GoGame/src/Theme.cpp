#include "../include/Theme.h" 
#include <iostream>

namespace ThemeManager {

    BoardData getThemeData(BoardTheme theme) {
        BoardData data;

        data.starRadius = 5.0f;
        data.lineThickness = 2.5f;

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

    StoneData getStoneData(StoneTheme style) {
        StoneData data;
        float stoneTargetSize = 40;

        std::string prefix = "assets/images/stones/";
        std::string bPath, wPath;

        switch (style) {
        case StoneTheme::Standard:
            bPath = "black_standard.png";
            wPath = "white_standard.png";
            break;
        case StoneTheme::Realistic:
            bPath = "black_realistic.png";
            wPath = "white_realistic.png";
            break;
        case StoneTheme::Flat:
            bPath = "black_flat.png";
            wPath = "white_flat.png";
            break;
        default:
            bPath = "black_standard.png";
            wPath = "white_standard.png";
            break;
        }

        if (!data.blackStoneTexture.loadFromFile(prefix + bPath)) {
            std::cerr << "FAILED to load: " << prefix + bPath << "\n";
        }
        else {
            data.blackStoneTexture.setSmooth(true);
        }

        if (!data.whiteStoneTexture.loadFromFile(prefix + wPath)) {
            std::cerr << "FAILED to load: " << prefix + wPath << "\n";
        }
        else {
            data.whiteStoneTexture.setSmooth(true);
        }

		return data;
    }

    std::string getStoneName(StoneTheme style) {
        switch (style) {
		case StoneTheme::Standard: return "Standard";
        case StoneTheme::Realistic: return "Slate & Shell";
        case StoneTheme::Flat: return "Minimalist";
        default: return "Unknown";
        }
    }

    StoneTheme getNextStone(StoneTheme current) {
        int nextIndex = (static_cast<int>(current) + 1) % static_cast<int>(StoneTheme::MAX_COUNT);
        return static_cast<StoneTheme>(nextIndex);
    }
}