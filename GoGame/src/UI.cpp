#include "../include/UI.h"
#include "../include/Board.h"

#include <iostream>
#include <fstream>
#include <sstream>   
#include <iomanip>   
#include <memory>
#include <algorithm> 
#include <tuple>     
#include <thread>
#include <chrono> 

const sf::Color darkGreen(0, 100, 0);

UI::UI() : window(sf::VideoMode(baseWindowX, baseWindowY), "Go Game", sf::Style::Default), currentGameState(GameState::MainMenu), isSoundEnabled(true) {
    ShowWindow(window.getSystemHandle(), SW_MAXIMIZE);

    view.setSize((float)baseWindowX, (float)baseWindowY);
    view.setCenter((float)baseWindowX / 2.0f, (float)baseWindowY / 2.0f);
    resizeView(window, view);
    window.setView(view);

    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Error loading font\n";
    }

    // Tải âm thanh
    if (!placeSoundBuffer.loadFromFile("assets/sounds/clicksound.mp3")) std::cerr << "Error loading place sound\n";
    // if (!captureSoundBuffer.loadFromFile("assets/sounds/capture.wav")) std::cerr << "Error loading capture sound\n";
    if (!backgroundSound.openFromFile("assets/sounds/background.ogg")) std::cerr << "Error loading background sound\n";
    if (!mainmenuBackgroundTexture.loadFromFile("assets/images/mainmenu_background.png")) std::cerr << "Error loading main menu background image\n";

    if (isSoundEnabled) {
        playMusic();
    }

    placeSound.setBuffer(placeSoundBuffer);
    captureSound.setBuffer(captureSoundBuffer);
    // backgroundSound.setBuffer(backgroundSoundBuffer);

    turnIndicatorText.setFont(font);
    turnIndicatorText.setCharacterSize(24);
    turnIndicatorText.setFillColor(sf::Color::Black);
    turnIndicatorText.setPosition(baseCellSize, baseWindowY - 50);

    // Main menu
	mainmenuBackgroundSprite.setTexture(mainmenuBackgroundTexture);
    sf::Vector2u bgSize = mainmenuBackgroundTexture.getSize();
    mainmenuBackgroundSprite.setScale(
        (float)baseWindowX / bgSize.x,
        (float)baseWindowY / bgSize.y
    );

    gameOverOverlay.setSize(sf::Vector2f((float)baseWindowX, (float)baseWindowY));
    gameOverOverlay.setFillColor(sf::Color(0, 0, 0, 80)); // Màu đen, trong suốt nhẹ

    gameOverTitleText.setFont(font);
    gameOverTitleText.setString("GAME OVER");
    gameOverTitleText.setCharacterSize(70.0f);
    gameOverTitleText.setFillColor(sf::Color::Red);
    gameOverTitleText.setStyle(sf::Text::Bold);

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
    menuBackground.setSize(sf::Vector2f((float)baseWindowX, (float)baseWindowY));

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

    // smooth
    blackStoneTexture.setSmooth(true);
    whiteStoneTexture.setSmooth(true);

    blackStoneSprite.setTexture(blackStoneTexture);
    whiteStoneSprite.setTexture(whiteStoneTexture);
}

void UI::draw(sf::RenderWindow& window) {
    window.draw(mainmenuBackgroundSprite);

    Board &board = game.board;
	const int boardSize = board.getSize();

    // window.draw(boardSprite);

    float spacing = CenterboardSprite.getGlobalBounds().width; 
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

        if (event.type == sf::Event::Resized) {
            resizeView(window, view);
            window.setView(view);
        }

        switch (currentGameState) {
        case GameState::MainMenu: handleMainMenuEvents(event); break;
        case GameState::Playing: handlePlayingEvents(event); break;
        case GameState::Settings: handleSettingsEvents(event); break;
        case GameState::DifficultySelect: 
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                activateMenuItem(difficultySelectButtons, window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), view));
            }
            else if (event.type == sf::Event::MouseMoved) {
                highlightMenuItem(difficultySelectButtons, window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), view));
            }
            break;

        case GameState::GameOver:
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                currentGameState = GameState::MainMenu;
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = { event.mouseButton.x, event.mouseButton.y };
                if (playAgainButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    game.startNewGame();
                    currentGameState = GameState::Playing; 
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

    case GameState::DifficultySelect: 
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        highlightMenuItem(difficultySelectButtons, static_cast<sf::Vector2f>(mousePos));
    }
    break;

    case GameState::GameOver:
    { 
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
    window.clear(sf::Color(20, 20, 20));

    window.setView(view);

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

    case GameState::DifficultySelect: 
        window.draw(mainmenuBackgroundSprite);
        drawMenu(window, difficultySelectButtons);
        break;
        
    case GameState::GameOver:
        renderPlaying(); 
        window.draw(gameOverOverlay);
        window.draw(gameOverTitleText);
        window.draw(finalScoresText);
        window.draw(winnerMessageText);
        window.draw(playAgainButton);
        window.draw(mainMenuButton);
        break;
    }

    window.display();
}

void UI::updateNotification(const std::string& message) {
    notificationText.setFont(font);
    notificationText.setStyle(sf::Text::Bold);
    notificationText.setCharacterSize(20);
    notificationText.setFillColor(sf::Color::Red);
    notificationText.setPosition(baseWindowX / 2.0f, baseWindowY - 30);

    notificationText.setString(message);
    sf::FloatRect textRect = notificationText.getLocalBounds();
    notificationText.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
}

void UI::handleMainMenuEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        highlightMenuItem(mainMenuButtons, window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), view));
    }
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        activateMenuItem(mainMenuButtons, window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), view));
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
        if (event.key.code == sf::Keyboard::S) { saveGame(); }
        if (event.key.code == sf::Keyboard::L) { loadGame(); } 
        if (event.key.code == sf::Keyboard::Z) { undoMove(); }
        if (event.key.code == sf::Keyboard::Y) { redoMove(); } 
        if (event.key.code == sf::Keyboard::R) { startNewGame(game.currentMode, game.currentDifficulty); } 
        if (event.key.code == sf::Keyboard::P) { passTurn(); }
        if (event.key.code == sf::Keyboard::Escape) { 
            currentGameState = GameState::MainMenu;
            updateNotification("Returning to Main Menu.");
        }
    }
}

void UI::handleSettingsEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        highlightMenuItem(settingsButtons, window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), view));
    }
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        activateMenuItem(settingsButtons, window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), view));
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        currentGameState = GameState::MainMenu;
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

    if (!game.isGameOver && game.currentMode == GameMode::PlayerVsAI && game.currentPlayer == Stone::White) { 
        if (game.AI_move()) {
            updateNotification("AI has made its move.");
        }
        else {
			updateNotification("AI passed the move.");
            if (game.consecutivePasses >= 2) {
                game.isGameOver = true;
				handleEndGame();
			}
        }
    }
}

void UI::renderMainMenu() {
	window.draw(mainmenuBackgroundSprite); 
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
    window.draw(mainmenuBackgroundSprite);
    sf::Text settingsTitle("Settings", font, 50);
    settingsTitle.setFillColor(sf::Color::White);
    sf::FloatRect textRect = settingsTitle.getLocalBounds();
    settingsTitle.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    settingsTitle.setPosition(baseWindowX / 2.0f, 100);
    window.draw(settingsTitle);

    drawMenu(window, settingsButtons);
}

void UI::setupMainMenu() {
    const std::vector<std::tuple<std::string, GameState, GameMode>> buttonData = {
        {"Load Game", GameState::Playing, GameMode::PlayerVsPlayer}, 
        {"Player vs Player", GameState::Playing, GameMode::PlayerVsPlayer},
        {"Player vs Computer", GameState::DifficultySelect, GameMode::PlayerVsAI}, 
        {"Settings", GameState::Settings, GameMode::PlayerVsPlayer},
        {"Exit", GameState::GameOver, GameMode::PlayerVsPlayer}
    };

    float CharSize = 30.f;
    float SpaceSize = 75.f;
    float ButtonWidth = 400.f;
    float ButtonHeight = 50.f;

    float totalHeight = buttonData.size() * SpaceSize;
    float startY = baseWindowY / 2.0f - totalHeight / 2.0f + 50.f;

    mainMenuButtons.clear();

    for (size_t i = 0; i < buttonData.size(); ++i) {
        const auto& data = buttonData[i];
        MenuItem item;
        item.text = std::get<0>(data);
        item.targetState = std::get<1>(data);
        item.targetMode = std::get<2>(data);

        item.sfText.setFont(font);
        item.sfText.setCharacterSize(static_cast<unsigned int>(CharSize));
        item.sfText.setFillColor(sf::Color::White);
        item.sfText.setString(item.text);

        sf::FloatRect textRect = item.sfText.getLocalBounds();
        item.sfText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

        item.backgroundRect.setSize(sf::Vector2f(ButtonWidth, ButtonHeight));
        item.backgroundRect.setOrigin(ButtonWidth / 2.0f, ButtonHeight / 2.0f);
        item.backgroundRect.setPosition(baseWindowX / 2.0f, startY + i * SpaceSize);
        item.backgroundRect.setFillColor(sf::Color(0, 0, 0, 180));
        item.backgroundRect.setOutlineColor(sf::Color::White);
        item.backgroundRect.setOutlineThickness(2.0f);

        item.sfText.setPosition(item.backgroundRect.getPosition());

        mainMenuButtons.push_back(item);
    }
}

void UI::setupDifficultySelectMenu() {
    const std::vector<std::tuple<std::string, Difficulty>> difficultyData = {
        {"Easy Mode", Difficulty::Easy},
        {"Medium Mode", Difficulty::Medium},
        {"Hard Mode", Difficulty::Hard},
        {"Back", Difficulty::Easy} 
    };

    float CharSize = 30.f;
    float SpaceSize = 75.f;
    float ButtonWidth = 400.f;
    float ButtonHeight = 50.f;

    float totalHeight = difficultyData.size() * SpaceSize;
    float startY = baseWindowY / 2.0f - totalHeight / 2.0f + 50.f;

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

        item.sfText.setFont(font);
        item.sfText.setCharacterSize(static_cast<unsigned int>(CharSize));
        item.sfText.setFillColor(sf::Color::White);
        item.sfText.setString(item.text);

        sf::FloatRect textRect = item.sfText.getLocalBounds();
        item.sfText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

        item.backgroundRect.setSize(sf::Vector2f(ButtonWidth, ButtonHeight));
        item.backgroundRect.setOrigin(ButtonWidth / 2.0f, ButtonHeight / 2.0f);
        item.backgroundRect.setPosition(baseWindowX / 2.0f, startY + i * SpaceSize);
        item.backgroundRect.setFillColor(sf::Color(0, 0, 0, 180));
        item.backgroundRect.setOutlineColor(sf::Color::White);
        item.backgroundRect.setOutlineThickness(2.0f);

        item.sfText.setPosition(item.backgroundRect.getPosition());

        difficultySelectButtons.push_back(item);
    }
}

void UI::setupSettingsMenu() {
    float startY = baseWindowY / 2.0f - 50.0f; 
    float spacing = 80.0f;
    float ButtonWidth = 350.f;
    float ButtonHeight = 50.f;

    settingsButtons.clear();

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
        item.backgroundRect.setPosition(baseWindowX / 2.0f, startY + index * spacing);
        item.backgroundRect.setFillColor(sf::Color(0, 0, 0, 180));
        item.backgroundRect.setOutlineColor(sf::Color::White);
        item.backgroundRect.setOutlineThickness(2.0f);

        item.sfText.setPosition(item.backgroundRect.getPosition());
        return item;
        };

    MenuItem soundItem = createButton("Sound: " + std::string(isSoundEnabled ? "ON" : "OFF"), 0);
    settingsButtons.push_back(soundItem);

    MenuItem backItem = createButton("Back to Main Menu", 1);
    backItem.targetState = GameState::MainMenu;
    settingsButtons.push_back(backItem);
}

void UI::drawMenu(sf::RenderWindow& window, const std::vector<MenuItem>& menuItems) {
    for (const auto& item : menuItems) {
        window.draw(item.backgroundRect); 
        window.draw(item.sfText);        
    }
}

void UI::highlightMenuItem(std::vector<MenuItem>& menuItems, const sf::Vector2f& mousePos) {
    for (auto& item : menuItems) {
        if (item.backgroundRect.getGlobalBounds().contains(mousePos)) {
            item.sfText.setFillColor(sf::Color::Yellow);
            item.backgroundRect.setOutlineColor(sf::Color::Yellow);
            item.backgroundRect.setFillColor(sf::Color(50, 50, 50, 200)); 
        }
        else {
            item.sfText.setFillColor(sf::Color::White);
            item.backgroundRect.setOutlineColor(sf::Color::White);
            item.backgroundRect.setFillColor(sf::Color(0, 0, 0, 180)); 
        }
    }
}

void UI::activateMenuItem(std::vector<MenuItem>& menuItems, const sf::Vector2f& mousePos) {
    for (size_t i = 0; i < menuItems.size(); ++i) {
        if (menuItems[i].backgroundRect.getGlobalBounds().contains(mousePos)) {

            if (menuItems[i].text == "Exit") {
                window.close();
                return;
            }

            if (currentGameState == GameState::MainMenu) {
                if (menuItems[i].text == "Load Game") {
                    loadGame(); 
                }
                else if (menuItems[i].text == "Player vs Player") {
                    startNewGame(GameMode::PlayerVsPlayer, Difficulty::Easy);
                    currentGameState = GameState::Playing;
                }
                else if (menuItems[i].text == "Player vs Computer") {
                    setupDifficultySelectMenu();
                    currentGameState = GameState::DifficultySelect;
                }
                else if (menuItems[i].text == "Settings") {
                    setupSettingsMenu();
                    currentGameState = GameState::Settings;
                }
            }
            else if (currentGameState == GameState::DifficultySelect) {
                if (menuItems[i].text == "Back") {
                    setupMainMenu(); 
                    currentGameState = GameState::MainMenu;
                }
                else {
                    startNewGame(GameMode::PlayerVsAI, menuItems[i].targetDifficulty);
                    currentGameState = GameState::Playing;
                }
            }
            else if (currentGameState == GameState::Settings) {
                if (menuItems[i].text.rfind("Sound:", 0) == 0) {
                    toggleSound();

                    menuItems[i].text = "Sound: " + std::string(isSoundEnabled ? "ON" : "OFF");
                    menuItems[i].sfText.setString(menuItems[i].text);

                    sf::FloatRect textRect = menuItems[i].sfText.getLocalBounds();
                    menuItems[i].sfText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                    menuItems[i].sfText.setPosition(menuItems[i].backgroundRect.getPosition());
                }

                else if (menuItems[i].targetState == GameState::MainMenu) {
                    setupMainMenu();
                    currentGameState = GameState::MainMenu;
                }
            }

            break; 
        }
    }
}

void UI::handlePlayerInput(const sf::Vector2i& mousePos) {
    if (game.isGameOver || (game.currentMode == GameMode::PlayerVsAI && game.currentPlayer == Stone::White)) return;

    float spacing = baseCellSize;
    float offset = 50;

    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, view);

    int col = static_cast<int>((worldPos.x - offset - LeftborderSize + spacing / 2) / spacing);
    int row = static_cast<int>((worldPos.y - offset - LeftborderSize + spacing / 2) / spacing);

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

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    ss << "Black Score: " << blackScore << "\n"
        << "White Score: " << whiteScore;
    finalScoresText.setString(ss.str());
    winnerMessageText.setString(winnerStr);

    float centerX = baseWindowX / 2.0f;
    auto centerText = [&](sf::Text& text) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.0f - (totalBoardPixelSize - LeftborderSize) / 2, bounds.top + bounds.height / 2.0f);
        };

    gameOverOverlay.setSize(sf::Vector2f((float)baseWindowX, (float)baseWindowY));

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
    if (isSoundEnabled) {
        playMusic();
    }
    else {
        stopMusic();
	}

    updateNotification("Sound " + (isSoundEnabled ? std::string("ON") : std::string("OFF")));
    setupSettingsMenu(); 
}

void UI::playMusic() {
    backgroundSound.setLoop(true); 
    backgroundSound.play();
}

void UI::stopMusic() {
    backgroundSound.stop();
}

void UI::resizeView(const sf::RenderWindow& window, sf::View& view) {
    // Tỷ lệ khung hình logic (Base resolution)
    float targetAspectRatio = (float)baseWindowX / (float)baseWindowY;

    sf::Vector2u size = window.getSize();
    float windowAspectRatio = (float)size.x / (float)size.y;

    if (windowAspectRatio >= targetAspectRatio) {
        // Cửa sổ bè hơn -> Thêm dải đen trái phải
        view.setSize((float)baseWindowX, (float)baseWindowY);
        float viewportWidth = size.y * targetAspectRatio / size.x;
        float viewportLeft = (1.0f - viewportWidth) / 2.0f;
        view.setViewport(sf::FloatRect(viewportLeft, 0.0f, viewportWidth, 1.0f));
    }
    else {
        // Cửa sổ cao hơn (thường gặp khi Maximize có thanh tiêu đề) -> Thêm dải đen trên dưới
        view.setSize((float)baseWindowX, (float)baseWindowY);
        float viewportHeight = size.x / targetAspectRatio / size.y;
        float viewportTop = (1.0f - viewportHeight) / 2.0f;
        view.setViewport(sf::FloatRect(0.0f, viewportTop, 1.0f, viewportHeight));
    }
}