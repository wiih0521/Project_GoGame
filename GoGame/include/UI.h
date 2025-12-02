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
#include "Theme.h"

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
    const float LeftborderSize = 40;

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

    sf::SoundBuffer placeSoundBuffer, captureSoundBuffer;
    sf::Music backgroundSound;
    sf::Sound placeSound, captureSound;
    bool isSoundEnabled;

    void setupMainMenu();
    void setupSettingsMenu();
    void drawMenu(sf::RenderWindow& window, const std::vector<MenuItem>& menuItems);
    void highlightMenuItem(std::vector<MenuItem>& menuItems, const sf::Vector2f& mousePos);
    void activateMenuItem(std::vector<MenuItem>& menuItems, const sf::Vector2f& mousePos);

    sf::Texture blackStoneTexture, whiteStoneTexture;
    sf::Sprite blackStoneSprite, whiteStoneSprite;

    sf::CircleShape starPointShape;

    BoardTheme currentTheme;
    ThemeData currentThemeData; 

    void updateTheme(BoardTheme newTheme);

    sf::RectangleShape blackPanel;
    sf::Text blackPanelText;

    sf::RectangleShape whitePanel;
    sf::Text whitePanelText;
    void drawScorePanel(sf::RenderWindow& window, sf::RectangleShape& panel, sf::Text& text, bool isActive, bool isBlackStyle);
};

#endif // UI_H