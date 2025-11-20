#ifndef UI_H
#define UI_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> // Thêm cho âm thanh
#include <stack>
#include <vector> // Cần thiết cho các trạng thái bàn cờ
#include <memory> // Cần thiết cho std::unique_ptr
#include "Board.h"
#include "Game.h"
#include "AI.h"

// Định nghĩa các trạng thái của gamezzz
enum class GameState {
    MainMenu,
    DifficultySelect, // THÊM MỚI: Trạng thái chọn độ khó
    Playing,
    Settings,
    GameOver
};

// Để hiển thị các mục menu
struct MenuItem {
    std::string text;
    sf::Text sfText;
    sf::RectangleShape backgroundRect; // THÊM MỚI: Khung nền cho mục menu

    GameState targetState;
    GameMode targetMode;
    Difficulty targetDifficulty;
    bool isSelected;

    MenuItem() : text(""), targetState(GameState::MainMenu), targetMode(GameMode::PlayerVsPlayer), targetDifficulty(Difficulty::Easy), isSelected(false) {
        // Khởi tạo mặc định cho backgroundRect
        backgroundRect.setFillColor(sf::Color(0, 0, 0, 150)); // Màu đen bán trong suốt
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
    void processEvents();
    void update();
    void render();

    // Các hàm xử lý trạng thái
    void handleMainMenuEvents(const sf::Event& event);
    void handlePlayingEvents(const sf::Event& event);
    void handleSettingsEvents(const sf::Event& event);

    //void updateMainMenu();
    void updatePlaying();
    //void updateSettings();

    void renderMainMenu();
    void renderPlaying();
    void renderSettings();

    // Hàm chung cho việc xử lý input của người chơi (khi đang chơi)
    void handlePlayerInput(const sf::Vector2i& mousePos);
    void startNewGame(GameMode mode, Difficulty diff = Difficulty::Easy);
    bool isWithinBounds(int row, int col) const;
    void saveGame();
    void loadGame();
    void undoMove();
    void redoMove();
    void passTurn();
    void handleEndGame();
    void setupDifficultySelectMenu(); // Hàm thiết lập menu chọn độ khó
    std::pair<float, float> calculateFinalScores() const;

    void toggleSound(); // Hàm bật/tắt âm thanh
    void updateNotification(const std::string& message);

    // Thêm các vector menu item cho menu chọn độ khó
    std::vector<MenuItem> difficultySelectButtons;

    sf::RenderWindow window;
    Game game;
    std::unique_ptr<AI> ai;

    // Trạng thái game hiện tại
    GameState currentGameState;

    // Stacks for Undo/Redo
    /*std::stack<std::vector<std::vector<Stone>>> moveHistory;
    std::stack<std::vector<std::vector<Stone>>> redoHistory;*/

    // UI elements
    sf::Font font;
    sf::Text turnIndicatorText;
    sf::Text notificationText;
    sf::Text EndGameText;
    sf::Text keyblindguideText;

	// Game Over Screen Elements
    sf::RectangleShape gameOverOverlay; // Lớp phủ mờ
    sf::Text gameOverTitleText;      // Chữ "GAME OVER"
    sf::Text finalScoresText;        // Text hiển thị điểm
    sf::Text winnerMessageText;      // Text thông báo người thắng
    sf::Text playAgainButton;        // Nút "Chơi lại"
    sf::Text mainMenuButton;         // Nút "Về Menu chính"

    // -- Các thành phần mới cho Menu và Settings --
	sf::Texture mainmenuBackgroundTexture;
	sf::Sprite mainmenuBackgroundSprite;
    std::vector<MenuItem> mainMenuButtons;
    std::vector<MenuItem> settingsButtons; // Menu items for settings
    sf::RectangleShape menuBackground; // Hình nền cho menu

    // Âm thanh
    sf::SoundBuffer placeSoundBuffer, captureSoundBuffer;
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

    sf::Texture BackGroundTexture;
    sf::Sprite BackGroundSprite;
};

#endif // UI_H