#include "../include/UI.h"
#include "../include/Board.h"
#include "../include/Theme.h" // Đảm bảo bạn đã tạo file này như hướng dẫn trước

#include <iostream>
#include <fstream>
#include <sstream>   
#include <iomanip>   
#include <memory>
#include <algorithm> 
#include <tuple>     
#include <thread>
#include <chrono> 

const float LeftborderSize = 0.0f;

UI::UI() : window(sf::VideoMode(baseWindowX, baseWindowY), "Go Game", sf::Style::Default), currentGameState(GameState::MainMenu), isSoundEnabled(true) {
    ShowWindow(window.getSystemHandle(), SW_MAXIMIZE);

    view.setSize((float)baseWindowX, (float)baseWindowY);
    view.setCenter((float)baseWindowX / 2.0f, (float)baseWindowY / 2.0f);
    resizeView(window, view);
    window.setView(view);

    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Error loading font\n";
    }

    if (!placeSoundBuffer.loadFromFile("assets/sounds/clicksound.mp3")) std::cerr << "Error loading place sound\n";
    // if (!captureSoundBuffer.loadFromFile("assets/sounds/capture.wav")) std::cerr << "Error loading capture sound\n";
    if (!backgroundSound.openFromFile("assets/sounds/background.ogg")) std::cerr << "Error loading background sound\n";
    if (!mainmenuBackgroundTexture.loadFromFile("assets/images/background1.png")) std::cerr << "Error loading main menu background image\n";

    if (isSoundEnabled) {
        playMusic();
    }

    placeSound.setBuffer(placeSoundBuffer);
    captureSound.setBuffer(captureSoundBuffer);

    turnIndicatorText.setFont(font);
    turnIndicatorText.setCharacterSize(24);
    turnIndicatorText.setFillColor(sf::Color::Black);
    turnIndicatorText.setPosition(baseCellSize, baseWindowY - 50);

    mainmenuBackgroundSprite.setTexture(mainmenuBackgroundTexture);
    sf::Vector2u bgSize = mainmenuBackgroundTexture.getSize();
    mainmenuBackgroundSprite.setScale(
        (float)baseWindowX / bgSize.x,
        (float)baseWindowY / bgSize.y
    );

    gameOverOverlay.setSize(sf::Vector2f((float)baseWindowX, (float)baseWindowY));
    gameOverOverlay.setFillColor(sf::Color(0, 0, 0, 80));

    gameOverTitleText.setFont(font);
    gameOverTitleText.setString("GAME OVER");
    gameOverTitleText.setCharacterSize(70.0f);
    gameOverTitleText.setFillColor(sf::Color::Red);
    gameOverTitleText.setStyle(sf::Text::Bold);

    finalScoresText.setFont(font);
    finalScoresText.setCharacterSize(30.0f);
    finalScoresText.setFillColor(sf::Color::White);

    winnerMessageText.setFont(font);
    winnerMessageText.setCharacterSize(45.0f);
    winnerMessageText.setFillColor(sf::Color::Yellow);
    winnerMessageText.setStyle(sf::Text::Bold);

    playAgainButton.setFont(font);
    playAgainButton.setString("Play Again");
    playAgainButton.setCharacterSize(40.0f);
    playAgainButton.setFillColor(sf::Color::White);

    mainMenuButton.setFont(font);
    mainMenuButton.setString("Back to Main Menu");
    mainMenuButton.setCharacterSize(40.0f);
    mainMenuButton.setFillColor(sf::Color::White);

    updateTheme(BoardTheme::Classic);

    setupMainMenu();
    setupSettingsMenu();

    menuBackground.setSize(sf::Vector2f(window.getSize()));
    menuBackground.setFillColor(sf::Color(50, 50, 50, 180));
    menuBackground.setSize(sf::Vector2f((float)baseWindowX, (float)baseWindowY));

    if (!blackStoneTexture.loadFromFile("assets/images/black_stone.png")) std::cerr << "Error loading black stone texture\n";
    if (!whiteStoneTexture.loadFromFile("assets/images/white_stone.png")) std::cerr << "Error loading white stone texture\n";

    blackStoneTexture.setSmooth(true);
    whiteStoneTexture.setSmooth(true);

    blackStoneSprite.setTexture(blackStoneTexture);
    whiteStoneSprite.setTexture(whiteStoneTexture);

    float panelWidth = 750.0f;
    float panelHeight = 150.0f;
    float panelX = baseWindowX - panelWidth - 50.0f; // Cách lề phải 100px
    float panelY_Black = 200.0f; // Vị trí Y của bảng Đen
    float panelY_White = 400.0f; // Vị trí Y của bảng Trắng

    // 1. Cấu hình Panel Đen
    blackPanel.setSize(sf::Vector2f(panelWidth, panelHeight));
    blackPanel.setPosition(panelX, panelY_Black);
    // Style: Nền đen, Viền trắng (để nổi trên background tối)
    blackPanel.setFillColor(sf::Color(30, 30, 30));
    blackPanel.setOutlineThickness(3.0f);
    blackPanel.setOutlineColor(sf::Color::White);

    blackPanelText.setFont(font);
    blackPanelText.setCharacterSize(24);
    blackPanelText.setFillColor(sf::Color::White); // Chữ trắng

    // 2. Cấu hình Panel Trắng
    whitePanel.setSize(sf::Vector2f(panelWidth, panelHeight));
    whitePanel.setPosition(panelX, panelY_White);
    // Style: Nền trắng, Viền đen
    whitePanel.setFillColor(sf::Color(240, 240, 240));
    whitePanel.setOutlineThickness(3.0f);
    whitePanel.setOutlineColor(sf::Color::Black);

    whitePanelText.setFont(font);
    whitePanelText.setCharacterSize(24);
    whitePanelText.setFillColor(sf::Color::Black); // Chữ đen
}

void UI::updateTheme(BoardTheme newTheme) {
    currentTheme = newTheme;
    currentThemeData = ThemeManager::getThemeData(newTheme);
}

void UI::draw(sf::RenderWindow& window) {
    window.draw(mainmenuBackgroundSprite);

    Board& board = game.board;
    const int boardSize = board.getSize();

    sf::Color boardColor = currentThemeData.boardColor;
    sf::Color borderColor = currentThemeData.borderColor;
    sf::Color lineColor = currentThemeData.lineColor;
    sf::Color starColor = currentThemeData.starColor;
    float lineThickness = currentThemeData.lineThickness;
    float starRadius = currentThemeData.starRadius;

    float spacing = (float)baseCellSize;
    float offset = 50.0f;

    float startX = offset + LeftborderSize;
    float startY = offset + LeftborderSize;

    float gridLength = (boardSize - 1) * spacing;
    float boardMargin = spacing / 2.0f;
    sf::RectangleShape boardArea(sf::Vector2f(gridLength + spacing, gridLength + spacing));
    boardArea.setPosition(startX - boardMargin, startY - boardMargin);

    boardArea.setFillColor(boardColor);
    boardArea.setOutlineThickness(4.0f);
    boardArea.setOutlineColor(borderColor);

    sf::RectangleShape shadow = boardArea;
    shadow.move(10.0f, 10.0f);
    shadow.setFillColor(sf::Color(0, 0, 0, 80));
    window.draw(shadow);

    window.draw(boardArea);

    for (int i = 0; i < boardSize; ++i) {
        sf::RectangleShape hLine(sf::Vector2f(gridLength + lineThickness, lineThickness));
        hLine.setOrigin(lineThickness / 2, lineThickness / 2);
        hLine.setPosition(startX, startY + i * spacing);
        hLine.setFillColor(lineColor);
        window.draw(hLine);

        sf::RectangleShape vLine(sf::Vector2f(lineThickness, gridLength + lineThickness));
        vLine.setOrigin(lineThickness / 2, lineThickness / 2);
        vLine.setPosition(startX + i * spacing, startY);
        vLine.setFillColor(lineColor);
        window.draw(vLine);
    }

    sf::CircleShape starPoint(starRadius);
    starPoint.setOrigin(starRadius, starRadius);
    starPoint.setFillColor(starColor);

    std::vector<int> starCoords;
    if (boardSize == 19) starCoords = { 3, 9, 15 };
    else if (boardSize == 13) starCoords = { 3, 6, 9 };
    else if (boardSize == 9) starCoords = { 2, 4, 6 };

    for (int r : starCoords) {
        for (int c : starCoords) {
            starPoint.setPosition(startX + c * spacing, startY + r * spacing);
            window.draw(starPoint);
        }
    }

    for (int r = 0; r < boardSize; ++r) {
        for (int c = 0; c < boardSize; ++c) {
            if (board.grid[r][c] != Stone::None) {
                sf::Sprite& stoneSprite = (board.grid[r][c] == Stone::Black) ? blackStoneSprite : whiteStoneSprite;

                stoneSprite.setPosition(
                    startX + c * spacing - stoneSprite.getGlobalBounds().width / 2,
                    startY + r * spacing - stoneSprite.getGlobalBounds().height / 2
                );
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
        if (event.key.code == sf::Keyboard::Z) { undoMove(); /*renderPlaying();*/ }
        if (event.key.code == sf::Keyboard::Y) { redoMove(); /*renderPlaying();*/ }
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

    std::string blackStr = "BLACK\nCaptured: " + std::to_string(game.getBlackCaptured());
    blackPanelText.setString(blackStr);

    std::string whiteStr = "WHITE\nCaptured: " + std::to_string(game.getWhiteCaptured());
    whitePanelText.setString(whiteStr);

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
    draw(window);
    // window.draw(turnIndicatorText);
    
    if (currentGameState != GameState::GameOver) {
        window.draw(notificationText);
        window.draw(keyblindguideText);

        bool isBlackTurn = (game.currentPlayer == Stone::Black);
        drawScorePanel(window, blackPanel, blackPanelText, isBlackTurn, true);
        drawScorePanel(window, whitePanel, whitePanelText, !isBlackTurn, false);
	}
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
    float startY = baseWindowY / 2.0f - 100.0f; // Dịch lên một chút
    float spacing = 80.0f;
    float ButtonWidth = 450.f; // Rộng hơn để chứa tên Theme
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

    // 1. Sound Button
    MenuItem soundItem = createButton("Sound: " + std::string(isSoundEnabled ? "ON" : "OFF"), 0);
    settingsButtons.push_back(soundItem);

    // 2. Theme Button (MỚI)
    MenuItem themeItem = createButton("Theme: " + ThemeManager::getThemeName(currentTheme), 1);
    settingsButtons.push_back(themeItem);

    // 3. Back Button
    MenuItem backItem = createButton("Back to Main Menu", 2);
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
                // Handle Sound Toggle
                if (menuItems[i].text.rfind("Sound:", 0) == 0) {
                    toggleSound();

                    menuItems[i].text = "Sound: " + std::string(isSoundEnabled ? "ON" : "OFF");
                    menuItems[i].sfText.setString(menuItems[i].text);

                    sf::FloatRect textRect = menuItems[i].sfText.getLocalBounds();
                    menuItems[i].sfText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                    menuItems[i].sfText.setPosition(menuItems[i].backgroundRect.getPosition());
                }
                // Handle Theme Toggle (MỚI)
                else if (menuItems[i].text.rfind("Theme:", 0) == 0) {
                    BoardTheme nextTheme = ThemeManager::getNextTheme(currentTheme);
                    updateTheme(nextTheme);

                    menuItems[i].text = "Theme: " + ThemeManager::getThemeName(currentTheme);
                    menuItems[i].sfText.setString(menuItems[i].text);

                    sf::FloatRect textRect = menuItems[i].sfText.getLocalBounds();
                    menuItems[i].sfText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                    menuItems[i].sfText.setPosition(menuItems[i].backgroundRect.getPosition());
                }
                // Handle Back
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
    // Không cần gọi setupSettingsMenu() ở đây để tránh reset vị trí nút
}

void UI::playMusic() {
    backgroundSound.setLoop(true);
    backgroundSound.play();
}

void UI::stopMusic() {
    backgroundSound.stop();
}

void UI::resizeView(const sf::RenderWindow& window, sf::View& view) {
    float targetAspectRatio = (float)baseWindowX / (float)baseWindowY;

    sf::Vector2u size = window.getSize();
    float windowAspectRatio = (float)size.x / (float)size.y;

    if (windowAspectRatio >= targetAspectRatio) {
        view.setSize((float)baseWindowX, (float)baseWindowY);
        float viewportWidth = size.y * targetAspectRatio / size.x;
        float viewportLeft = (1.0f - viewportWidth) / 2.0f;
        view.setViewport(sf::FloatRect(viewportLeft, 0.0f, viewportWidth, 1.0f));
    }
    else {
        view.setSize((float)baseWindowX, (float)baseWindowY);
        float viewportHeight = size.x / targetAspectRatio / size.y;
        float viewportTop = (1.0f - viewportHeight) / 2.0f;
        view.setViewport(sf::FloatRect(0.0f, viewportTop, 1.0f, viewportHeight));
    }
}

void UI::drawScorePanel(sf::RenderWindow& window, sf::RectangleShape& panel, sf::Text& text, bool isActive, bool isBlackStyle) {

    // 1. Vẽ Bóng đổ (Chỉ vẽ nếu đang là lượt chơi - isActive)
    if (isActive) {
        sf::RectangleShape shadow = panel;
        shadow.move(10.0f, 10.0f); // Dịch chuyển xuống dưới phải

        // Màu bóng: Nếu là bảng đen thì bóng sáng (Glow), bảng trắng thì bóng tối
        if (isBlackStyle) {
            shadow.setFillColor(sf::Color(255, 255, 255, 100)); // Glow trắng mờ
        }
        else {
            shadow.setFillColor(sf::Color(0, 0, 0, 150)); // Bóng đen
        }

        // Loại bỏ viền của bóng
        shadow.setOutlineThickness(0);
        window.draw(shadow);
    }

    // 2. Hiệu ứng active (Làm panel to ra một chút hoặc viền sáng hơn)
    if (isActive) {
        panel.setOutlineThickness(5.0f); // Viền dày hơn
        if (isBlackStyle) panel.setOutlineColor(sf::Color::Yellow); // Viền vàng báo hiệu
        else panel.setOutlineColor(sf::Color::Blue);
    }
    else {
        panel.setOutlineThickness(3.0f); // Viền bình thường
        if (isBlackStyle) panel.setOutlineColor(sf::Color::White);
        else panel.setOutlineColor(sf::Color::Black);
    }

    // 3. Vẽ Panel chính
    window.draw(panel);

    // 4. Căn giữa Text vào trong Panel
    sf::FloatRect textRect = text.getLocalBounds();
    sf::FloatRect panelRect = panel.getGlobalBounds();

    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(
        panelRect.left + panelRect.width / 2.0f,
        panelRect.top + panelRect.height / 2.0f
    );

    // 5. Vẽ Text
    window.draw(text);
}