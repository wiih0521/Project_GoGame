#include "Game.h"
#include "Board.h"
#include <iostream>
#include <fstream>

const int BoardSize = 9; // Kích thước bàn cờ 
int CellSize = 90; // Kích thước mỗi ô trên bàn cờ
Game::Game() : window(sf::VideoMode(BoardSize * CellSize, BoardSize * CellSize), "Go Game"), board(BoardSize), isGameOver(false) {
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Error loading font\n";
    }
    turnIndicatorText.setFont(font);
    turnIndicatorText.setCharacterSize(24);
    turnIndicatorText.setFillColor(sf::Color::Black);
    turnIndicatorText.setPosition(50, 950);

    startNewGame(GameMode::PlayerVsPlayer);
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

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                handlePlayerInput(sf::Mouse::getPosition(window));
            }
        }

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::S) saveGame();
            if (event.key.code == sf::Keyboard::L) loadGame();
            if (event.key.code == sf::Keyboard::Z) undoMove();
            if (event.key.code == sf::Keyboard::Y) redoMove();
            if (event.key.code == sf::Keyboard::R) startNewGame(currentMode, currentDifficulty);
        }
    }
}

bool Game::isWithinBounds(int row, int col) const {
    return board.isWithinBounds(row, col);
}

void Game::update() {
    turnIndicatorText.setString(std::string("Turn: ") + (currentPlayer == Stone::Black ? "Black" : "White"));

    if (!isGameOver && currentMode == GameMode::PlayerVsAI && currentPlayer == Stone::White) { // AI là quân trắng
        Move aiMove = ai->findBestMove(board, Stone::White);
        if (aiMove.row != -1) {
            moveHistory.push(board.getBoardState());
            board.placeStone(aiMove.row, aiMove.col, aiMove.player);
            currentPlayer = Stone::Black;
            while (!redoHistory.empty()) redoHistory.pop(); // Xóa redo stack
        }
    }
}

void Game::render() {
    window.clear(sf::Color(200, 150, 100)); // Màu nền
    board.draw(window);
    window.draw(turnIndicatorText);
    window.display();
}

void Game::handlePlayerInput(const sf::Vector2i& mousePos) {
    if (isGameOver || (currentMode == GameMode::PlayerVsAI && currentPlayer == Stone::White)) {
        // Không xử lý input nếu game kết thúc hoặc đang là lượt của AI
        return;
    }

    float spacing = CellSize;
    float offset = 50.0f;

    // Chuyển đổi tọa độ chuột sang tọa độ hàng/cột trên bàn cờ
    int col = static_cast<int>((mousePos.x - offset + spacing / 2) / spacing);
    int row = static_cast<int>((mousePos.y - offset + spacing / 2) / spacing);

    // Kiểm tra tính hợp lệ của tọa độ
    if (!board.isWithinBounds(row, col)) { // Cần thêm hàm isWithinBounds vào Board.h nếu chưa có
        notificationText.setString("Invalid position click.");
        return;
    }

    // GỌI placeStone và CHỈ LƯU LỊCH SỬ NẾU HỢP LỆ
    if (board.placeStone(row, col, currentPlayer)) {
        moveHistory.push(board.getBoardState()); // Lưu trạng thái sau nước đi hợp lệ
        currentPlayer = (currentPlayer == Stone::Black) ? Stone::White : Stone::Black;
        while (!redoHistory.empty()) redoHistory.pop(); // Xóa redo stack khi có nước đi mới hợp lệ
        notificationText.setString(""); // Xóa thông báo nếu nước đi hợp lệ
    }
    else {
        // Nước đi không hợp lệ (ví dụ: ô đã có quân, nước tự sát)
        notificationText.setString("Invalid move: spot taken or suicide move.");
    }
}

void Game::startNewGame(GameMode mode, Difficulty diff) {
    currentMode = mode;
    currentDifficulty = diff;
    if (mode == GameMode::PlayerVsAI) {
        ai = std::make_unique<AI>(diff);
    }
    board.reset();
    currentPlayer = Stone::Black; // Đen đi trước
    isGameOver = false;
    while (!moveHistory.empty()) moveHistory.pop();
    while (!redoHistory.empty()) redoHistory.pop();
}

void Game::saveGame() { // [cite: 92]
    std::ofstream saveFile("savegame.txt");
    if (saveFile.is_open()) {
        saveFile << static_cast<int>(currentPlayer) << "\n";
        const auto& state = board.getBoardState();
        for (int r = 0; r < board.getSize(); ++r) {
            for (int c = 0; c < board.getSize(); ++c) {
                saveFile << static_cast<int>(state[r][c]) << " ";
            }
            saveFile << "\n";
        }
        saveFile.close();
        std::cout << "Game saved!\n";
    }
}

void Game::loadGame() { // [cite: 93]
    std::ifstream loadFile("savegame.txt");
    if (loadFile.is_open()) {
        int player;
        loadFile >> player;
        currentPlayer = static_cast<Stone>(player);

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
        std::cout << "Game loaded!\n";
    }
}

void Game::undoMove() { // [cite: 84]
    if (!moveHistory.empty()) {
        redoHistory.push(board.getBoardState());
        board.setBoardState(moveHistory.top());
        moveHistory.pop();
        currentPlayer = (currentPlayer == Stone::Black) ? Stone::White : Stone::Black;
    }
}

void Game::redoMove() { // [cite: 86]
    if (!redoHistory.empty()) {
        moveHistory.push(board.getBoardState());
        board.setBoardState(redoHistory.top());
        redoHistory.pop();
        currentPlayer = (currentPlayer == Stone::Black) ? Stone::White : Stone::Black;
    }
}