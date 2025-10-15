#include "Game.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm> // Cho std::max_element
#include <tuple>     // Cho std::tuple trong setupMainMenu

const int BoardSize = 9; // size of game board
const int CellSize = 90; // base on size of (c).png
const float notificationSize = 40;
Game::Game() : window(sf::VideoMode(BoardSize * CellSize, BoardSize* CellSize + notificationSize), "Go Game"), board(BoardSize), isGameOver(false), currentGameState(GameState::MainMenu), isSoundEnabled(true) {
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Error loading font\n";
    }

    // Tải âm thanh
    if (!placeSoundBuffer.loadFromFile("assets/sounds/place_stone.wav")) std::cerr << "Error loading place sound\n";
    if (!captureSoundBuffer.loadFromFile("assets/sounds/capture.wav")) std::cerr << "Error loading capture sound\n";
    placeSound.setBuffer(placeSoundBuffer);
    captureSound.setBuffer(captureSoundBuffer);

    // Cài đặt ban đầu cho các text UI
    turnIndicatorText.setFont(font);
    turnIndicatorText.setCharacterSize(24);
    turnIndicatorText.setFillColor(sf::Color::Black);
    turnIndicatorText.setPosition(CellSize, window.getSize().y - 30);

    notificationText.setFont(font);
    notificationText.setCharacterSize(20);
    notificationText.setFillColor(sf::Color::Red);
    notificationText.setPosition(window.getSize().x / 2.0f, window.getSize().y - 50);

    // Setup menu
    setupMainMenu();
    setupSettingsMenu(); // Khởi tạo lần đầu

    // Khởi tạo nền menu (có thể thay bằng hình ảnh)
    menuBackground.setSize(sf::Vector2f(window.getSize()));
    menuBackground.setFillColor(sf::Color(50, 50, 50, 180)); // Nền mờ
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        switch (currentGameState) {
        case GameState::MainMenu: handleMainMenuEvents(event); break;
        case GameState::Playing: handlePlayingEvents(event); break;
        case GameState::Settings: handleSettingsEvents(event); break;
        case GameState::GameOver: // Có thể thêm xử lý riêng cho Game Over
            // Hiện tại, chỉ cho phép quay về menu chính
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                currentGameState = GameState::MainMenu;
            }
            break;
        }
    }
}

void Game::update() {
    switch (currentGameState) {
        // CHỈ GỌI updatePlaying KHI Ở TRẠNG THÁI PLAYING
    case GameState::Playing:
        updatePlaying();
        break;
    default:
        // Không có logic cập nhật phức tạp cho các trạng thái khác
        break;
    }
}

void Game::render() {
    window.clear(sf::Color(200, 150, 100)); // Màu nền chung (có thể đổi)

    switch (currentGameState) {
    case GameState::MainMenu:
        renderMainMenu();
        break;
    case GameState::Playing:
        renderPlaying(); // <== Đảm bảo gọi hàm vẽ khi đang chơi
        break;
    case GameState::Settings:
        renderSettings();
        break;
    case GameState::GameOver:
        renderPlaying(); // Vẫn vẽ bàn cờ khi game over
        // Thêm text hiển thị kết quả game over
        sf::Text gameOverText("GAME OVER!\nPress ESC to Main Menu", font, 40);
        gameOverText.setFillColor(sf::Color::Red);
        sf::FloatRect textRect = gameOverText.getLocalBounds();
        gameOverText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        gameOverText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
        window.draw(gameOverText);
        break;
    }

    window.display();
}

// Cải tiến hàm này để căn giữa text động
void Game::updateNotification(const std::string& message) {
    notificationText.setString(message);
    sf::FloatRect textRect = notificationText.getLocalBounds();
    notificationText.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
}

// === Xử lý sự kiện cho từng trạng thái ===
void Game::handleMainMenuEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        highlightMenuItem(mainMenuButtons, window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)));
    }
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        activateMenuItem(mainMenuButtons, window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
    }
}

void Game::handlePlayingEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        handlePlayerInput(sf::Mouse::getPosition(window));
    }
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::S) { saveGame(); } // saveGame đã có updateNotification
        if (event.key.code == sf::Keyboard::L) { loadGame(); } // loadGame đã có updateNotification
        if (event.key.code == sf::Keyboard::Z) { undoMove(); } // undoMove đã có updateNotification
        if (event.key.code == sf::Keyboard::Y) { redoMove(); } // redoMove đã có updateNotification
        if (event.key.code == sf::Keyboard::R) { startNewGame(currentMode, currentDifficulty); } // startNewGame đã có
        if (event.key.code == sf::Keyboard::Escape) { // Trở về menu chính từ game
            currentGameState = GameState::MainMenu;
            updateNotification("Returning to Main Menu.");
        }
    }
}

void Game::handleSettingsEvents(const sf::Event& event) {
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

// === Cập nhật cho từng trạng thái ===
// Không có update cho MainMenu/Settings vì chúng không có animation/logic liên tục
// updateMainMenu() và updateSettings() đã bị xóa khỏi Game::update()

void Game::updatePlaying() {
    // Cập nhật text hiển thị lượt đi (Đây là nơi nó nên được cập nhật liên tục)
    turnIndicatorText.setString(std::string("Turn: ") + (currentPlayer == Stone::Black ? "Black" : "White"));
	window.draw(turnIndicatorText); // VẼ LẠI Ở ĐÂY ĐỂ ĐẢM BẢO NÓ LUÔN HIỂN THỊ

    // Logic lượt đi của AI (giữ nguyên)
    if (!isGameOver && currentMode == GameMode::PlayerVsAI && currentPlayer == Stone::White) { // Giả sử AI luôn là quân trắng
        Move aiMove = ai->findBestMove(board, Stone::White);
        if (aiMove.row != -1 && board.placeStone(aiMove.row, aiMove.col, aiMove.player)) {
            // Nước đi của AI hợp lệ
            moveHistory.push(board.getBoardState());
            currentPlayer = Stone::Black;
            while (!redoHistory.empty()) redoHistory.pop();
            updateNotification("AI played.");
            if (isSoundEnabled) placeSound.play(); // Play sound
        }
        else {
            std::cerr << "AI could not find a valid move or game is over.\n";
            updateNotification("AI could not move.");
            // Nếu AI không thể di chuyển, có thể coi là pass hoặc game over tùy luật
            // Vẫn là lượt của AI, AI có thể thử lại ở update tiếp theo, hoặc game có thể kết thúc.
            // Để đơn giản, ở đây AI cứ thử lại.
        }
    }
}


// === Vẽ cho từng trạng thái ===
void Game::renderMainMenu() {
    window.draw(menuBackground); // Vẽ nền menu
    drawMenu(window, mainMenuButtons);
}

void Game::renderPlaying() {
    board.draw(window);
    window.draw(turnIndicatorText); // <== THÊM LẠI DÒNG NÀY ĐỂ VẼ TEXT LƯỢT ĐI
    window.draw(notificationText);
}

void Game::renderSettings() {
    window.draw(menuBackground); // Vẽ nền menu
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
void Game::setupMainMenu() {
    float startY = window.getSize().y / 2.0f - 150;
    float spacing = 60.0f;
    mainMenuButtons.clear();

    const std::vector<std::tuple<std::string, GameState, GameMode, Difficulty>> buttonData = {
        {"Play (Player vs Player)", GameState::Playing, GameMode::PlayerVsPlayer, Difficulty::Easy},
        {"Play (AI Easy)", GameState::Playing, GameMode::PlayerVsAI, Difficulty::Easy},
        {"Play (AI Medium)", GameState::Playing, GameMode::PlayerVsAI, Difficulty::Medium},
        {"Play (AI Hard)", GameState::Playing, GameMode::PlayerVsAI, Difficulty::Hard},
        {"Settings", GameState::Settings, GameMode::PlayerVsPlayer, Difficulty::Easy},
        {"Exit", GameState::GameOver, GameMode::PlayerVsPlayer, Difficulty::Easy}
    };

    for (const auto& data : buttonData) {
        MenuItem item;
        item.text = std::get<0>(data);
        item.targetState = std::get<1>(data);
        item.targetMode = std::get<2>(data);
        item.targetDifficulty = std::get<3>(data);
        item.sfText.setFont(font);
        item.sfText.setCharacterSize(30);
        item.sfText.setFillColor(sf::Color::White);
        item.sfText.setString(item.text);
        sf::FloatRect textRect = item.sfText.getLocalBounds();
        item.sfText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        mainMenuButtons.push_back(item);
    }

    for (size_t i = 0; i < mainMenuButtons.size(); ++i) {
        mainMenuButtons[i].sfText.setPosition(window.getSize().x / 2.0f, startY + i * spacing);
    }
}

void Game::setupSettingsMenu() {
    float startY = window.getSize().y / 2.0f - 50;
    float spacing = 50.0f;
    settingsButtons.clear();

    // Sound Toggle
    MenuItem soundItem;
    soundItem.text = "Sound: " + (isSoundEnabled ? std::string("ON") : std::string("OFF"));
    soundItem.sfText.setFont(font);
    soundItem.sfText.setCharacterSize(25);
    soundItem.sfText.setFillColor(sf::Color::White);
    settingsButtons.push_back(soundItem);

    // Back to Main Menu
    MenuItem backItem;
    backItem.text = "Back to Main Menu";
    backItem.targetState = GameState::MainMenu;
    backItem.sfText.setFont(font);
    backItem.sfText.setCharacterSize(25);
    backItem.sfText.setFillColor(sf::Color::White);
    settingsButtons.push_back(backItem);

    // Căn giữa và đặt vị trí cho các nút
    for (size_t i = 0; i < settingsButtons.size(); ++i) {
        settingsButtons[i].sfText.setString(settingsButtons[i].text);
        sf::FloatRect textRect = settingsButtons[i].sfText.getLocalBounds();
        settingsButtons[i].sfText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        settingsButtons[i].sfText.setPosition(window.getSize().x / 2.0f, startY + i * spacing);
    }
}

void Game::drawMenu(sf::RenderWindow& window, const std::vector<MenuItem>& menuItems) {
    for (const auto& item : menuItems) {
        window.draw(item.sfText);
    }
}

void Game::highlightMenuItem(std::vector<MenuItem>& menuItems, const sf::Vector2f& mousePos) {
    for (auto& item : menuItems) {
        if (item.sfText.getGlobalBounds().contains(mousePos)) {
            item.sfText.setFillColor(sf::Color::Yellow);
        }
        else {
            item.sfText.setFillColor(sf::Color::White);
        }
    }
}

void Game::activateMenuItem(std::vector<MenuItem>& menuItems, const sf::Vector2f& mousePos) {
    for (size_t i = 0; i < menuItems.size(); ++i) {
        if (menuItems[i].sfText.getGlobalBounds().contains(mousePos)) {
            if (menuItems[i].text.rfind("Sound:", 0) == 0) {
                toggleSound();
                // setupSettingsMenu() sẽ cập nhật lại text và vị trí của nút Sound
            }
            else if (menuItems[i].text == "Exit") {
                window.close();
            }
            else {
                currentGameState = menuItems[i].targetState;
                if (currentGameState == GameState::Playing) {
                    startNewGame(menuItems[i].targetMode, menuItems[i].targetDifficulty);
                }
            }
            break;
        }
    }
}

void Game::handlePlayerInput(const sf::Vector2i& mousePos) {
    if (isGameOver || (currentMode == GameMode::PlayerVsAI && currentPlayer == Stone::White)) return;

    float spacing = CellSize;
    float offset = 50;

    int col = static_cast<int>((mousePos.x - offset + spacing / 2) / spacing);
    int row = static_cast<int>((mousePos.y - offset + spacing / 2) / spacing);

    if (!board.isWithinBounds(row, col)) {
        updateNotification("Invalid position click.");
        return;
    }

    if (board.placeStone(row, col, currentPlayer)) {
        moveHistory.push(board.getBoardState());
        currentPlayer = (currentPlayer == Stone::Black) ? Stone::White : Stone::Black;
        while (!redoHistory.empty()) redoHistory.pop();
        updateNotification("");
        if (isSoundEnabled) placeSound.play();
    }
    else {
        updateNotification("Invalid move: spot taken or suicide move.");
    }
}

void Game::startNewGame(GameMode mode, Difficulty diff) {
    currentMode = mode;
    currentDifficulty = diff;
    if (mode == GameMode::PlayerVsAI) {
        ai = std::make_unique<AI>(diff);
    }
    else {
        ai.reset();
    }
    board.reset();
    currentPlayer = Stone::Black;
    isGameOver = false;
    while (!moveHistory.empty()) moveHistory.pop();
    while (!redoHistory.empty()) redoHistory.pop();
    updateNotification("New Game Started!");
}

void Game::saveGame() {
    std::ofstream saveFile("savegame.txt");
    if (saveFile.is_open()) {
        saveFile << static_cast<int>(currentMode) << "\n";
        saveFile << static_cast<int>(currentDifficulty) << "\n";
        saveFile << static_cast<int>(currentPlayer) << "\n";
        saveFile << static_cast<int>(isSoundEnabled) << "\n";
        const auto& state = board.getBoardState();
        for (int r = 0; r < board.getSize(); ++r) {
            for (int c = 0; c < board.getSize(); ++c) {
                saveFile << static_cast<int>(state[r][c]) << " ";
            }
            saveFile << "\n";
        }
        saveFile.close();
        updateNotification("Game saved!");
    }
    else {
        updateNotification("Failed to save game!");
    }
}

void Game::loadGame() {
    std::ifstream loadFile("savegame.txt");
    if (loadFile.is_open()) {
        int modeInt, diffInt, playerInt, soundInt;
        loadFile >> modeInt >> diffInt >> playerInt >> soundInt;

        isSoundEnabled = static_cast<bool>(soundInt);
        setupSettingsMenu(); // Gọi lại để cập nhật text và vị trí của nút Sound

        currentMode = static_cast<GameMode>(modeInt);
        currentDifficulty = static_cast<Difficulty>(diffInt);
        startNewGame(currentMode, currentDifficulty); // Hàm này cũng gọi updateNotification

        currentPlayer = static_cast<Stone>(playerInt);

        std::vector<std::vector<Stone>> loadedState(board.getSize(), std::vector<Stone>(board.getSize()));
        for (int r = 0; r < board.getSize(); ++r) {
            for (int c = 0; c < board.getSize(); ++c) {
                int stoneVal;
                loadFile >> stoneVal;
                loadedState[r][c] = static_cast<Stone>(stoneVal);
            }
        }
        board.setBoardState(loadedState);
        loadFile.close();
        updateNotification("Game loaded!");
        currentGameState = GameState::Playing;
    }
    else {
        updateNotification("Failed to load game!");
    }
}

void Game::undoMove() {
    if (!moveHistory.empty()) {
        redoHistory.push(board.getBoardState());
        board.setBoardState(moveHistory.top());
        moveHistory.pop();
        currentPlayer = (currentPlayer == Stone::Black) ? Stone::White : Stone::Black;
        updateNotification("Undo successful.");
    }
    else {
        updateNotification("Cannot undo further.");
    }
}

void Game::redoMove() {
    if (!redoHistory.empty()) {
        moveHistory.push(board.getBoardState());
        board.setBoardState(redoHistory.top());
        redoHistory.pop();
        currentPlayer = (currentPlayer == Stone::Black) ? Stone::White : Stone::Black;
        updateNotification("Redo successful.");
    }
    else {
        updateNotification("Cannot redo further.");
    }
}

void Game::toggleSound() {
    isSoundEnabled = !isSoundEnabled;
    updateNotification("Sound " + (isSoundEnabled ? std::string("ON") : std::string("OFF")));
    setupSettingsMenu(); // Gọi lại để cập nhật text và vị trí của nút Sound
}