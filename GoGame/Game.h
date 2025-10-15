#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> // Thêm cho âm thanh
#include <stack>
#include <vector> // Cần thiết cho các trạng thái bàn cờ
#include <memory> // Cần thiết cho std::unique_ptr
#include "Board.h"
#include "AI.h"

// Định nghĩa các trạng thái của game
enum class GameState {
    MainMenu,
    Playing,
    Settings,
    GameOver
};

enum class GameMode { PlayerVsPlayer, PlayerVsAI };
// enum class Difficulty { Easy, Medium, Hard }; // Đã định nghĩa trong AI.h, có thể bỏ ở đây nếu chỉ dùng AI.h

// Để hiển thị các mục menu
struct MenuItem {
    std::string text;
    sf::Text sfText;
    GameState targetState; // Trạng thái game khi chọn mục này
    GameMode targetMode;   // Chế độ chơi khi chọn mục này
    Difficulty targetDifficulty; // Độ khó khi chọn mục này
    // Cờ để đánh dấu mục nào đang được chọn (cho menu settings)
    bool isSelected;

    // Constructor mặc định cho MenuItem
    MenuItem() : text(""), targetState(GameState::MainMenu), targetMode(GameMode::PlayerVsPlayer), targetDifficulty(Difficulty::Easy), isSelected(false) {}
};


class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update();
    void render();

    // Các hàm xử lý trạng thái
    void handleMainMenuEvents(const sf::Event& event);
    void handlePlayingEvents(const sf::Event& event);
    void handleSettingsEvents(const sf::Event& event);

    void updateMainMenu();
    void updatePlaying();
    void updateSettings();

    void renderMainMenu();
    void renderPlaying();
    void renderSettings();

    // Hàm chung cho việc xử lý input của người chơi (khi đang chơi)
    void handlePlayerInput(const sf::Vector2i& mousePos);
    void startNewGame(GameMode mode, Difficulty diff = Difficulty::Easy);
    void saveGame();
    void loadGame();
    void undoMove();
    void redoMove();
    void toggleSound(); // Hàm bật/tắt âm thanh
    void updateNotification(const std::string& message);

    sf::RenderWindow window;
    Board board;
    std::unique_ptr<AI> ai;

    // Trạng thái game hiện tại
    GameState currentGameState;

    GameMode currentMode;
    Difficulty currentDifficulty;
    Stone currentPlayer;
    bool isGameOver;

    // Stacks for Undo/Redo
    std::stack<std::vector<std::vector<Stone>>> moveHistory;
    std::stack<std::vector<std::vector<Stone>>> redoHistory;

    // UI elements
    sf::Font font;
    sf::Text turnIndicatorText;
    sf::Text notificationText;

    // -- Các thành phần mới cho Menu và Settings --
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
};

#endif // GAME_H