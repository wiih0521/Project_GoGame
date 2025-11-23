#ifndef UI_H
#define UI_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> 
#include <stack>
#include <vector>
#include <memory> 
#include "Board.h"
#include "Game.h"
#include "AI.h"

enum class GameState {
    MainMenu,
    DifficultySelect, 
    Playing,
    Settings,
    GameOver
};

struct MenuItem {
    std::string text;
    sf::Text sfText;
    sf::RectangleShape backgroundRect; 

    GameState targetState;
    GameMode targetMode;
    Difficulty targetDifficulty;
    bool isSelected;

    MenuItem() : text(""), targetState(GameState::MainMenu), targetMode(GameMode::PlayerVsPlayer), targetDifficulty(Difficulty::Easy), isSelected(false) {
        backgroundRect.setFillColor(sf::Color(0, 0, 0, 150)); 
        backgroundRect.setOutlineThickness(2);
        backgroundRect.setOutlineColor(sf::Color::White);
    }
};


class UI {
public:
    
    UI();
    void run();
    void draw(sf::RenderWindow& window);

private:
    const int BoardSize = 19;
    const int baseCellSize = 50;
    const float notificationSize = 20;
    const float totalBoardPixelSize = BoardSize * baseCellSize;
	const int baseWindowY = 1080;
	const int baseWindowX = 1920;

    float scaleRatio;
    float cellSize;

    void processEvents();
    void update();
    void render();

    sf::View view; 
    void resizeView(const sf::RenderWindow& window, sf::View& view); 

    void handleMainMenuEvents(const sf::Event& event);
    void handlePlayingEvents(const sf::Event& event);
    void handleSettingsEvents(const sf::Event& event);

    //void updateMainMenu();
    void updatePlaying();
    //void updateSettings();

    void renderMainMenu();
    void renderPlaying();
    void renderSettings();

    void handlePlayerInput(const sf::Vector2i& mousePos);
    void startNewGame(GameMode mode, Difficulty diff = Difficulty::Easy);
    bool isWithinBounds(int row, int col) const;
    void saveGame();
    void loadGame();
    void undoMove();
    void redoMove();
    void passTurn();
    void handleEndGame();
    void setupDifficultySelectMenu(); 
    std::pair<float, float> calculateFinalScores() const;

    void toggleSound(); 
    void playMusic();
	void stopMusic();
    void updateNotification(const std::string& message);

    std::vector<MenuItem> difficultySelectButtons;

    sf::RenderWindow window;
    Game game;
    std::unique_ptr<AI> ai;

    GameState currentGameState;

    sf::Font font;
    sf::Text turnIndicatorText;
    sf::Text notificationText;
    sf::Text EndGameText;
    sf::Text keyblindguideText;

    sf::RectangleShape gameOverOverlay; 
    sf::Text gameOverTitleText;     
    sf::Text finalScoresText;        
    sf::Text winnerMessageText;     
    sf::Text playAgainButton;      
    sf::Text mainMenuButton;        

	sf::Texture mainmenuBackgroundTexture;
	sf::Sprite mainmenuBackgroundSprite;
    std::vector<MenuItem> mainMenuButtons;
    std::vector<MenuItem> settingsButtons; 
    sf::RectangleShape menuBackground; 

    // Âm thanh
    sf::SoundBuffer placeSoundBuffer, captureSoundBuffer;
    sf::Music backgroundSound;
    sf::Sound placeSound, captureSound;
    bool isSoundEnabled;

    void setupMainMenu();
    void setupSettingsMenu();
    void drawMenu(sf::RenderWindow& window, const std::vector<MenuItem>& menuItems);
    void highlightMenuItem(std::vector<MenuItem>& menuItems, const sf::Vector2f& mousePos);
    void activateMenuItem(std::vector<MenuItem>& menuItems, const sf::Vector2f& mousePos);

	// Textures and Sprites for board and stones
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

    sf::Texture SpotTexture;
    sf::Sprite SpotSprite;

    sf::Texture BackGroundTexture;
    sf::Sprite BackGroundSprite;
};

#endif // UI_H