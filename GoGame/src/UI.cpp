#include "../include/UI.h"
#include "../include/Board.h"

#include <iostream>
#include <fstream>
#include <sstream>   
#include <iomanip>   
#include <memory>
#include <algorithm> 
#include <tuple>     
#include <thread> // Add this at the top of the file for std::this_thread::sleep_for
#include <chrono> // Add this for std::chrono::milliseconds

const int BoardSize = 19; // size of game board
//const int MaxBoardSize = 19; // maximum size of game board
const int CellSize = 50; // base on size of (c).png
const float notificationSize = 20;
const float totalBoardPixelSize = BoardSize * CellSize;

const sf::Color darkGreen(0, 100, 0);

UI::UI() : window(sf::VideoMode(1920, 1080), "Go Game", sf::Style::Default), currentGameState(GameState::MainMenu), isSoundEnabled(true) {
    // 1. Tạo cửa sổ với kích thước bất kỳ (ví dụ 800x600), dùng Style::Default
    // window.create(sf::VideoMode(1200, 800), "Go Game", sf::Style::Default);

    // 2. Gọi lệnh của Windows để phóng to (Maximize) cửa sổ
    ShowWindow(window.getSystemHandle(), SW_MAXIMIZE);

    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Error loading font\n";
    }

    // Tải âm thanh
    if (!placeSoundBuffer.loadFromFile("assets/sounds/place_stone.wav")) std::cerr << "Error loading place sound\n";
    if (!captureSoundBuffer.loadFromFile("assets/sounds/capture.wav")) std::cerr << "Error loading capture sound\n";
    if (!mainmenuBackgroundTexture.loadFromFile("assets/images/mainmenu_background.png")) std::cerr << "Error loading main menu background image\n";

    placeSound.setBuffer(placeSoundBuffer);
    captureSound.setBuffer(captureSoundBuffer);

    // Cài đặt ban đầu cho các text UI
    turnIndicatorText.setFont(font);
    turnIndicatorText.setCharacterSize(24);
    turnIndicatorText.setFillColor(sf::Color::Black);
    turnIndicatorText.setPosition(CellSize, window.getSize().y - 50);

    // Main menu
	mainmenuBackgroundSprite.setTexture(mainmenuBackgroundTexture);

    // KHỞI TẠO UI CHO MÀN HÌNH GAME OVER 
    // Lớp phủ
    gameOverOverlay.setSize(sf::Vector2f(window.getSize()));
    gameOverOverlay.setFillColor(sf::Color(0, 0, 0, 80)); // Màu đen, trong suốt nhẹ

    // Tiêu đề "GAME OVER"
    gameOverTitleText.setFont(font);
    gameOverTitleText.setString("GAME OVER");
    gameOverTitleText.setCharacterSize(70.0f);
    gameOverTitleText.setFillColor(sf::Color::Red);
    gameOverTitleText.setStyle(sf::Text::Bold);

    // Text điểm số
    finalScoresText.setFont(font);
    finalScoresText.setCharacterSize(30.0f);
    finalScoresText.setFillColor(sf::Color::White);

    // Text người thắng
    winnerMessageText.setFont(font);
    winnerMessageText.setCharacterSize(45.0f);
    winnerMessageText.setFillColor(sf::Color::Yellow);
    winnerMessageText.setStyle(sf::Text::Bold);

    // Nút "Chơi lại"
    playAgainButton.setFont(font);
    playAgainButton.setString("Play Again");
    playAgainButton.setCharacterSize(40.0f);
    playAgainButton.setFillColor(sf::Color::White);

    // Nút "Về Menu chính"
    mainMenuButton.setFont(font);
    mainMenuButton.setString("Back to Main Menu");
    mainMenuButton.setCharacterSize(40.0f);
    mainMenuButton.setFillColor(sf::Color::White);
    // =======================================================

    // Setup menu
    setupMainMenu();
    setupSettingsMenu(); // Khởi tạo lần đầu

    // Khởi tạo nền menu (có thể thay bằng hình ảnh)
    menuBackground.setSize(sf::Vector2f(window.getSize()));
    menuBackground.setFillColor(sf::Color(50, 50, 50, 180)); // Nền mờ

    // Tải assets - bạn cần đảm bảo các tệp này tồn tại
    if (!BottomboardTexture.loadFromFile("assets/images/b.png")) std::cerr << "Error loading board texture\n";
    if (!BottomLeftboardTexture.loadFromFile("assets/images/bl.png")) std::cerr << "Error loading board texture\n";
    if (!BottomRightboardTexture.loadFromFile("assets/images/br.png")) std::cerr << "Error loading board texture\n";
    if (!UpperboardTexture.loadFromFile("assets/images/u.png")) std::cerr << "Error loading board texture\n";
    if (!UpperLeftboardTexture.loadFromFile("assets/images/ul.png")) std::cerr << "Error loading board texture\n";
    if (!UpperRightboardTexture.loadFromFile("assets/images/ur.png")) std::cerr << "Error loading board texture\n";
    if (!LeftboardTexture.loadFromFile("assets/images/l.png")) std::cerr << "Error loading board texture\n";
    if (!RightboardTexture.loadFromFile("assets/images/r.png")) std::cerr << "Error loading board texture\n";
    if (!CenterboardTexture.loadFromFile("assets/images/c.png")) std::cerr << "Error loading board texture\n";
    if (!BackGroundTexture.loadFromFile("assets/images/background.png")) std::cerr << "Error loading board texture\n";
    if (!SpotTexture.loadFromFile("assets/images/spot.png")) std::cerr << "Error loading board texture\n";

    if (!blackStoneTexture.loadFromFile("assets/images/black_stone.png")) std::cerr << "Error loading black stone texture\n";
    if (!whiteStoneTexture.loadFromFile("assets/images/white_stone.png")) std::cerr << "Error loading white stone texture\n";

    BottomboardSprite.setTexture(BottomboardTexture);
    BottomLeftboardSprite.setTexture(BottomLeftboardTexture);
    BottomRightboardSprite.setTexture(BottomRightboardTexture);
    UpperboardSprite.setTexture(UpperboardTexture);
    UpperLeftboardSprite.setTexture(UpperLeftboardTexture);
    UpperRightboardSprite.setTexture(UpperRightboardTexture);
    LeftboardSprite.setTexture(LeftboardTexture);
    RightboardSprite.setTexture(RightboardTexture);
    CenterboardSprite.setTexture(CenterboardTexture);
    SpotSprite.setTexture(SpotTexture);
    BackGroundSprite.setTexture(BackGroundTexture);

    blackStoneSprite.setTexture(blackStoneTexture);
    whiteStoneSprite.setTexture(whiteStoneTexture);
}

void UI::draw(sf::RenderWindow& window) {
    window.draw(mainmenuBackgroundSprite);

    Board &board = game.board;
	const int boardSize = board.getSize();

    // window.draw(boardSprite);

    float spacing = CenterboardSprite.getGlobalBounds().width; // Khoảng cách giữa các giao điểm
    float offset = 50.0f; // Lề

    // std::cerr << CenterboardSprite.getGlobalBounds().width << std::endl;

    for (int r = 0; r < boardSize; ++r) {
        for (int c = 0; c < boardSize; ++c) {
            sf::Sprite* partSprite = nullptr;
            if (r == 0 && c == 0) partSprite = &UpperLeftboardSprite;
            else if (r == 0 && c == boardSize - 1) partSprite = &UpperRightboardSprite;
            else if (r == boardSize - 1 && c == 0) partSprite = &BottomLeftboardSprite;
            else if (r == boardSize - 1 && c == boardSize - 1) partSprite = &BottomRightboardSprite;
            else if (r == 0) partSprite = &UpperboardSprite;
            else if (r == boardSize - 1) partSprite = &BottomboardSprite;
            else if (c == 0) partSprite = &LeftboardSprite;
            else if (c == boardSize - 1) partSprite = &RightboardSprite;
            else if ((boardSize == 19 && ((r == 3 || r == 9 || r == 15) && (c == 3 || c == 9 || c == 15))) ||
                     (boardSize == 13 && ((r == 3 || r == 6 || r == 9) && (c == 3 || c == 6 || c == 9))) ||
                     (boardSize == 9 && ((r == 2 || r == 4 || r == 6) && (c == 2 || c == 4 || c == 6)))) {
                partSprite = &SpotSprite;
			}
            else partSprite = &CenterboardSprite;
            if (partSprite) {
                partSprite->setPosition(c * spacing + offset - (*partSprite).getGlobalBounds().width / 2 + LeftborderSize,
                    r * spacing + offset - (*partSprite).getGlobalBounds().height / 2 + LeftborderSize);
                window.draw(*partSprite);
            }
        }
    }

    for (int r = 0; r < boardSize; ++r) {
        for (int c = 0; c < boardSize; ++c) {
            if (board.grid[r][c] != Stone::None) {
                sf::Sprite& stoneSprite = (board.grid[r][c] == Stone::Black) ? blackStoneSprite : whiteStoneSprite;
                stoneSprite.setPosition(c * spacing + offset - stoneSprite.getGlobalBounds().width / 2 + LeftborderSize,
                    r * spacing + offset - stoneSprite.getGlobalBounds().height / 2 + LeftborderSize);
                window.draw(stoneSprite);
            }
        }
    }
}

void UI::run() {
    while (window.isOpen()) {
        processEvents();
        render();
        update();
        // render();
    }
}

void UI::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        switch (currentGameState) {
        case GameState::MainMenu: handleMainMenuEvents(event); break;
        case GameState::Playing: handlePlayingEvents(event); break;
        case GameState::Settings: handleSettingsEvents(event); break;
        case GameState::DifficultySelect: // Xử lý sự kiện cho menu chọn độ khó
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                activateMenuItem(difficultySelectButtons, window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
            }
            else if (event.type == sf::Event::MouseMoved) {
                highlightMenuItem(difficultySelectButtons, window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)));
            }
            break;

        // THÊM MỚI: Xử lý input cho màn hình GameOver
        case GameState::GameOver:
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                currentGameState = GameState::MainMenu;
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = { event.mouseButton.x, event.mouseButton.y };
                if (playAgainButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    game.startNewGame(); // Bắt đầu game mới với cài đặt cũ
                    currentGameState = GameState::Playing; // Quay lại trạng thái chơi
                }
                if (mainMenuButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    currentGameState = GameState::MainMenu;
                }
            }
            break;
        }
    }
}

void UI::update() {
    switch (currentGameState) {
    case GameState::Playing:
        updatePlaying();
        break;

    case GameState::DifficultySelect: // Xử lý highlight cho menu chọn độ khó
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        highlightMenuItem(difficultySelectButtons, static_cast<sf::Vector2f>(mousePos));
    }
    break;

    // Highlight cho nút ở màn hình GameOver
    case GameState::GameOver:
    { // Thêm scope để khai báo biến cục bộ
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        playAgainButton.setFillColor(playAgainButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)) ? sf::Color::Yellow : sf::Color::White);
        mainMenuButton.setFillColor(mainMenuButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)) ? sf::Color::Yellow : sf::Color::White);
    }
    break;

    default:
        break;
    }
}

void UI::render() {
    window.clear(sf::Color(200, 150, 100));

    switch (currentGameState) {
    case GameState::MainMenu:
        renderMainMenu();
        break;

    case GameState::Playing:
        renderPlaying();
        break;

    case GameState::Settings:
        renderSettings();
        break;

    case GameState::DifficultySelect: // Vẽ menu chọn độ khó
        window.draw(mainmenuBackgroundSprite);
        drawMenu(window, difficultySelectButtons);
        break;
        
    case GameState::GameOver:
        renderPlaying(); // Vẽ bàn cờ cuối cùng ở nền
        window.draw(gameOverOverlay); // Vẽ lớp phủ mờ lên trên
        // Vẽ các text của màn hình kết thúc
        window.draw(gameOverTitleText);
        window.draw(finalScoresText);
        window.draw(winnerMessageText);
        window.draw(playAgainButton);
        window.draw(mainMenuButton);
        break;
    }

    window.display();
}

// Cải tiến hàm này để căn giữa text động
void UI::updateNotification(const std::string& message) {
    notificationText.setFont(font);
    notificationText.setStyle(sf::Text::Bold);
    notificationText.setCharacterSize(20);
    notificationText.setFillColor(sf::Color::Red);
    notificationText.setPosition(window.getSize().x / 2.0f, window.getSize().y - 30);

    notificationText.setString(message);
    sf::FloatRect textRect = notificationText.getLocalBounds();
    notificationText.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
}

// === Xử lý sự kiện cho từng trạng thái ===
void UI::handleMainMenuEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        highlightMenuItem(mainMenuButtons, window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)));
    }
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        activateMenuItem(mainMenuButtons, window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
    }
}

void UI::handlePlayingEvents(const sf::Event& event) {
    if (game.isGameOver) {
        return;
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        handlePlayerInput(sf::Mouse::getPosition(window));
    }
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::S) { saveGame(); } // saveGame đã có updateNotification
        if (event.key.code == sf::Keyboard::L) { loadGame(); } // loadGame đã có updateNotification
        if (event.key.code == sf::Keyboard::Z) { undoMove(); } // undoMove đã có updateNotification
        if (event.key.code == sf::Keyboard::Y) { redoMove(); } // redoMove đã có updateNotification
        if (event.key.code == sf::Keyboard::R) { startNewGame(game.currentMode, game.currentDifficulty); } // startNewGame đã có
        if (event.key.code == sf::Keyboard::P) { passTurn(); } // startNewGame đã có
        if (event.key.code == sf::Keyboard::Escape) { // Trở về menu chính từ game
            currentGameState = GameState::MainMenu;
            updateNotification("Returning to Main Menu.");
        }
    }
}

void UI::handleSettingsEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        highlightMenuItem(settingsButtons, window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)));
    }
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        activateMenuItem(settingsButtons, window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        currentGameState = GameState::MainMenu; // Quay về Main Menu
        updateNotification("Settings saved.");
    }
}

void UI::updatePlaying() {
    if (game.isGameOver) {
        handleEndGame();
    }

    turnIndicatorText.setString(std::string("Turn: ") + (game.currentPlayer == Stone::Black ? "Black" : "White"));
    if (game.currentMode == GameMode::PlayerVsAI) {
        turnIndicatorText.setString("");
	}

    if (!game.isGameOver && game.currentMode == GameMode::PlayerVsAI && game.currentPlayer == Stone::White) { // Giả sử AI luôn là quân trắng
        if (game.AI_move()) {
            updateNotification("AI has made its move.");
        }
        else {
			updateNotification("AI passed the move.");
            if (++game.consecutivePasses >= 2) {
                game.isGameOver = true;
				handleEndGame();
			}
        }
    }
}

void UI::renderMainMenu() {
	window.draw(mainmenuBackgroundSprite); // Vẽ nền menu | Optional: menuBackground
    drawMenu(window, mainMenuButtons);
}

void UI::renderPlaying() {
    // window.draw(mainmenuBackgroundSprite);
    draw(window);
    window.draw(turnIndicatorText); 
    window.draw(notificationText);
    window.draw(keyblindguideText);
}

void UI::renderSettings() {
    window.draw(mainmenuBackgroundSprite); // Vẽ nền menu
    // Có thể vẽ thêm tiêu đề "Settings"
    sf::Text settingsTitle("Settings", font, 50);
    settingsTitle.setFillColor(sf::Color::White);
    sf::FloatRect textRect = settingsTitle.getLocalBounds();
    settingsTitle.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    settingsTitle.setPosition(window.getSize().x / 2.0f, 100);
    window.draw(settingsTitle);

    drawMenu(window, settingsButtons);
}

// === Các hàm hỗ trợ Menu ===
void UI::setupMainMenu() {
    // Cấu trúc menu mới
    // Item: Text, TargetState, TargetMode (nếu có), Difficulty (nếu có)
    // Lưu ý: TargetMode và Difficulty chỉ quan trọng khi TargetState là Playing hoặc DifficultySelect

    // Load Game sẽ cần logic riêng, tạm thời đặt TargetState là Playing (sẽ xử lý trong activateMenuItem)

    const std::vector<std::tuple<std::string, GameState, GameMode>> buttonData = {
        {"Load Game", GameState::Playing, GameMode::PlayerVsPlayer}, // Mode không quan trọng vì sẽ load từ file
        {"Player vs Player", GameState::Playing, GameMode::PlayerVsPlayer},
        {"Player vs Computer", GameState::DifficultySelect, GameMode::PlayerVsAI}, // Chuyển sang chọn độ khó
        {"Settings", GameState::Settings, GameMode::PlayerVsPlayer},
        {"Exit", GameState::GameOver, GameMode::PlayerVsPlayer}
    };

    float CharSize = 30.f;
    float SpaceSize = 75.f;
    float ButtonWidth = 400.f;
    float ButtonHeight = 50.f;

    float totalHeight = buttonData.size() * SpaceSize;
    float startY = window.getSize().y / 2.0f - totalHeight / 2.0f + 50.f;

    mainMenuButtons.clear();

    for (size_t i = 0; i < buttonData.size(); ++i) {
        const auto& data = buttonData[i];
        MenuItem item;
        item.text = std::get<0>(data);
        item.targetState = std::get<1>(data);
        item.targetMode = std::get<2>(data);
        // Difficulty mặc định là Easy, sẽ không dùng ở đây

        // ... (Thiết lập Text và Background Rect như cũ) ...
        item.sfText.setFont(font);
        item.sfText.setCharacterSize(static_cast<unsigned int>(CharSize));
        item.sfText.setFillColor(sf::Color::White);
        item.sfText.setString(item.text);

        sf::FloatRect textRect = item.sfText.getLocalBounds();
        item.sfText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

        item.backgroundRect.setSize(sf::Vector2f(ButtonWidth, ButtonHeight));
        item.backgroundRect.setOrigin(ButtonWidth / 2.0f, ButtonHeight / 2.0f);
        item.backgroundRect.setPosition(window.getSize().x / 2.0f, startY + i * SpaceSize);
        item.backgroundRect.setFillColor(sf::Color(0, 0, 0, 180));
        item.backgroundRect.setOutlineColor(sf::Color::White);
        item.backgroundRect.setOutlineThickness(2.0f);

        item.sfText.setPosition(item.backgroundRect.getPosition());

        mainMenuButtons.push_back(item);
    }
}

// === THÊM MỚI setupDifficultySelectMenu ===
void UI::setupDifficultySelectMenu() {
    const std::vector<std::tuple<std::string, Difficulty>> difficultyData = {
        {"Easy Mode", Difficulty::Easy},
        {"Medium Mode", Difficulty::Medium},
        {"Hard Mode", Difficulty::Hard},
        {"Back", Difficulty::Easy} // Difficulty không quan trọng cho nút Back
    };

    float CharSize = 30.f;
    float SpaceSize = 75.f;
    float ButtonWidth = 400.f;
    float ButtonHeight = 50.f;

    float totalHeight = difficultyData.size() * SpaceSize;
    float startY = window.getSize().y / 2.0f - totalHeight / 2.0f + 50.f;

    difficultySelectButtons.clear();

    for (size_t i = 0; i < difficultyData.size(); ++i) {
        const auto& data = difficultyData[i];
        MenuItem item;
        item.text = std::get<0>(data);
        item.targetDifficulty = std::get<1>(data);

        if (item.text == "Back") {
            item.targetState = GameState::MainMenu;
        }
        else {
            item.targetState = GameState::Playing;
            item.targetMode = GameMode::PlayerVsAI;
        }

        // ... (Thiết lập Text và Background Rect tương tự setupMainMenu) ...
        item.sfText.setFont(font);
        item.sfText.setCharacterSize(static_cast<unsigned int>(CharSize));
        item.sfText.setFillColor(sf::Color::White);
        item.sfText.setString(item.text);

        sf::FloatRect textRect = item.sfText.getLocalBounds();
        item.sfText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

        item.backgroundRect.setSize(sf::Vector2f(ButtonWidth, ButtonHeight));
        item.backgroundRect.setOrigin(ButtonWidth / 2.0f, ButtonHeight / 2.0f);
        item.backgroundRect.setPosition(window.getSize().x / 2.0f, startY + i * SpaceSize);
        item.backgroundRect.setFillColor(sf::Color(0, 0, 0, 180));
        item.backgroundRect.setOutlineColor(sf::Color::White);
        item.backgroundRect.setOutlineThickness(2.0f);

        item.sfText.setPosition(item.backgroundRect.getPosition());

        difficultySelectButtons.push_back(item);
    }
}

// === SỬA ĐỔI setupSettingsMenu ===
void UI::setupSettingsMenu() {
    float startY = window.getSize().y / 2.0f - 50.0f; // Điều chỉnh vị trí bắt đầu
    float spacing = 80.0f;
    float ButtonWidth = 350.f;
    float ButtonHeight = 50.f;

    settingsButtons.clear();

    // Helper lambda để tạo nút settings
    auto createButton = [&](const std::string& text, int index) -> MenuItem {
        MenuItem item;
        item.text = text;
        item.sfText.setFont(font);
        item.sfText.setCharacterSize(30);
        item.sfText.setFillColor(sf::Color::White);
        item.sfText.setString(text);

        sf::FloatRect textRect = item.sfText.getLocalBounds();
        item.sfText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

        item.backgroundRect.setSize(sf::Vector2f(ButtonWidth, ButtonHeight));
        item.backgroundRect.setOrigin(ButtonWidth / 2.0f, ButtonHeight / 2.0f);
        item.backgroundRect.setPosition(window.getSize().x / 2.0f, startY + index * spacing);
        item.backgroundRect.setFillColor(sf::Color(0, 0, 0, 180));
        item.backgroundRect.setOutlineColor(sf::Color::White);
        item.backgroundRect.setOutlineThickness(2.0f);

        item.sfText.setPosition(item.backgroundRect.getPosition());
        return item;
        };

    // Sound Toggle
    MenuItem soundItem = createButton("Sound: " + std::string(isSoundEnabled ? "ON" : "OFF"), 0);
    settingsButtons.push_back(soundItem);

    // Back to Main Menu
    MenuItem backItem = createButton("Back to Main Menu", 1);
    backItem.targetState = GameState::MainMenu;
    settingsButtons.push_back(backItem);
}

// === SỬA ĐỔI drawMenu ===
void UI::drawMenu(sf::RenderWindow& window, const std::vector<MenuItem>& menuItems) {
    for (const auto& item : menuItems) {
        window.draw(item.backgroundRect); // Vẽ khung trước
        window.draw(item.sfText);         // Vẽ chữ sau
    }
}

// === SỬA ĐỔI highlightMenuItem ===
void UI::highlightMenuItem(std::vector<MenuItem>& menuItems, const sf::Vector2f& mousePos) {
    for (auto& item : menuItems) {
        // Kiểm tra va chạm với backgroundRect thay vì sfText để vùng chọn lớn hơn
        if (item.backgroundRect.getGlobalBounds().contains(mousePos)) {
            item.sfText.setFillColor(sf::Color::Yellow);
            item.backgroundRect.setOutlineColor(sf::Color::Yellow); // Highlight viền
            item.backgroundRect.setFillColor(sf::Color(50, 50, 50, 200)); // Làm sáng nền một chút
        }
        else {
            item.sfText.setFillColor(sf::Color::White);
            item.backgroundRect.setOutlineColor(sf::Color::White);
            item.backgroundRect.setFillColor(sf::Color(0, 0, 0, 180)); // Màu gốc
        }
    }
}

// === SỬA ĐỔI activateMenuItem ===
void UI::activateMenuItem(std::vector<MenuItem>& menuItems, const sf::Vector2f& mousePos) {
    for (size_t i = 0; i < menuItems.size(); ++i) {
        // Kiểm tra xem chuột có click vào khung hình chữ nhật của nút không
        if (menuItems[i].backgroundRect.getGlobalBounds().contains(mousePos)) {

            // --- XỬ LÝ CHUNG CHO NÚT EXIT ---
            if (menuItems[i].text == "Exit") {
                window.close();
                return;
            }

            // --- XỬ LÝ LOGIC TẠI MAIN MENU ---
            if (currentGameState == GameState::MainMenu) {
                if (menuItems[i].text == "Load Game") {
                    loadGame(); // Hàm này sẽ tự chuyển state sang Playing nếu load thành công
                }
                else if (menuItems[i].text == "Player vs Player") {
                    startNewGame(GameMode::PlayerVsPlayer, Difficulty::Easy);
                    currentGameState = GameState::Playing;
                }
                else if (menuItems[i].text == "Player vs Computer") {
                    setupDifficultySelectMenu(); // Tạo menu con chọn độ khó
                    currentGameState = GameState::DifficultySelect;
                }
                else if (menuItems[i].text == "Settings") {
                    setupSettingsMenu();
                    currentGameState = GameState::Settings;
                }
            }
            // --- XỬ LÝ LOGIC TẠI MENU CHỌN ĐỘ KHÓ (DIFFICULTY SELECT) ---
            else if (currentGameState == GameState::DifficultySelect) {
                if (menuItems[i].text == "Back") {
                    setupMainMenu(); // Quay lại menu chính
                    currentGameState = GameState::MainMenu;
                }
                else {
                    // Các nút Easy/Medium/Hard đã được gán targetDifficulty trong setupDifficultySelectMenu
                    startNewGame(GameMode::PlayerVsAI, menuItems[i].targetDifficulty);
                    currentGameState = GameState::Playing;
                }
            }
            // --- XỬ LÝ LOGIC TẠI MENU SETTINGS ---
            else if (currentGameState == GameState::Settings) {
                // Kiểm tra nút Sound (bắt đầu bằng chuỗi "Sound:")
                if (menuItems[i].text.rfind("Sound:", 0) == 0) {
                    toggleSound();

                    // Cập nhật văn bản trên nút ngay lập tức
                    menuItems[i].text = "Sound: " + std::string(isSoundEnabled ? "ON" : "OFF");
                    menuItems[i].sfText.setString(menuItems[i].text);

                    // Căn giữa lại văn bản vì độ dài chuỗi có thể thay đổi
                    sf::FloatRect textRect = menuItems[i].sfText.getLocalBounds();
                    menuItems[i].sfText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                    menuItems[i].sfText.setPosition(menuItems[i].backgroundRect.getPosition());
                }
                // Kiểm tra nút Back (dựa trên targetState đã gán)
                else if (menuItems[i].targetState == GameState::MainMenu) {
                    setupMainMenu();
                    currentGameState = GameState::MainMenu;
                }
            }

            break; // Đã xử lý xong sự kiện click cho nút này, thoát vòng lặp
        }
    }
}

void UI::handlePlayerInput(const sf::Vector2i& mousePos) {
    if (game.isGameOver || (game.currentMode == GameMode::PlayerVsAI && game.currentPlayer == Stone::White)) return;

    float spacing = CellSize;
    float offset = 50;

    int col = static_cast<int>((mousePos.x - offset - LeftborderSize + spacing / 2) / spacing);
    int row = static_cast<int>((mousePos.y - offset - LeftborderSize + spacing / 2) / spacing);

    if (!game.board.isWithinBounds(row, col)) {
        updateNotification("Invalid position click.");
        return;
    }

    if (game.placeStone(row, col, game.currentPlayer)) {
        updateNotification("");
        if (isSoundEnabled) placeSound.play();
    }
    else {
        updateNotification("Invalid move: spot taken or suicide move.");
    }
}

void UI::startNewGame(GameMode mode, Difficulty diff) {
    game.startNewGame(mode, diff);
    updateNotification("New Game Started!");
}

void UI::saveGame() {
    if (game.saveGame()) {
        updateNotification("Game saved!");
    }
    else {
        updateNotification("Failed to save game!");
    }
}

void UI::loadGame() {
    if (game.loadGame()) {
        currentGameState = GameState::Playing;
        updateNotification("Game loaded!");
    }
    else {
        updateNotification("Failed to load game!");
    }
}

void UI::undoMove() {
    if (game.undoMove()) {
        updateNotification("Undo successful.");

        std::cerr << "Undo" << std::endl;
        const auto& state = game.board.getBoardState();
        for (int r = 0; r < game.board.getSize(); ++r) {
            for (int c = 0; c < game.board.getSize(); ++c) {
                std::cerr << static_cast<int>(state[r][c]) << " ";
            }
            std::cerr << "\n";
        }
    }
    else {
        updateNotification("Cannot undo further.");
    }
}

void UI::redoMove() {
    if (game.redoMove()) {
        updateNotification("Redo successful.");
    }
    else {
        updateNotification("Cannot redo further.");
    }
}

void UI::handleEndGame() {
	std::cerr << "Game Over triggered.\n";
    currentGameState = GameState::GameOver;

    std::pair<float, float> finalScores = game.calculateFinalScores();
    float blackScore = finalScores.first;
    float whiteScore = finalScores.second;

    std::string winnerStr;
    if (blackScore > whiteScore) {
        winnerStr = "BLACK WINS!";
    }
    else if (whiteScore > blackScore) {
        winnerStr = "WHITE WINS!";
    }
    else {
        winnerStr = "IT'S A DRAW!";
    }

    // Cập nhật nội dung cho các Text object
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    ss << "Black Score: " << blackScore << "\n"
        << "White Score: " << whiteScore;
    finalScoresText.setString(ss.str());
    winnerMessageText.setString(winnerStr);

    // Căn giữa và định vị các text trên màn hình
    float centerX = window.getSize().x / 2.0f;

    // Hàm trợ giúp để căn giữa text
    auto centerText = [](sf::Text& text) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.0f - (totalBoardPixelSize - LeftborderSize) / 2, bounds.top + bounds.height / 2.0f);
        };

    centerText(gameOverTitleText);
    gameOverTitleText.setPosition(centerX, 150.0f);

    centerText(winnerMessageText);
    winnerMessageText.setPosition(centerX, 250.0f);

    centerText(finalScoresText);
    finalScoresText.setPosition(centerX, 350.0f);

    centerText(playAgainButton);
    playAgainButton.setPosition(centerX, 500.0f);

    centerText(mainMenuButton);
    mainMenuButton.setPosition(centerX, 580.0f);
}

void UI::passTurn() {
    game.passTurn();
    if (game.isGameOver) {
        handleEndGame();
    }
    else {
        updateNotification("Player passed turn.");
    }
}

void UI::toggleSound() {
    isSoundEnabled = !isSoundEnabled;
    updateNotification("Sound " + (isSoundEnabled ? std::string("ON") : std::string("OFF")));
    setupSettingsMenu(); // Gọi lại để cập nhật text và vị trí của nút Sound
}