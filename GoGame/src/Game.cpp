#include "../include/Game.h"
#include "../include/UI.h"
#include "../include/AI.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread> 
#include <chrono> 

Game::Game() : board(19), isGameOver(false), currentMode(GameMode::PlayerVsPlayer), currentDifficulty(Difficulty::Easy), currentPlayer(Stone::Black), consecutivePasses(0) {
    ai = nullptr;
}

void Game::NewGame() {
    board.reset();
    currentPlayer = Stone::Black;
    isGameOver = false;
    consecutivePasses = 0;

    if (currentMode == GameMode::PlayerVsAI && currentDifficulty == Difficulty::Hard) {
        AIHard::startNewGame();
    }

    while (!moveHistory.empty()) moveHistory.pop();
    while (!redoHistory.empty()) redoHistory.pop();
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
    
    NewGame();
}

void Game::startNewGame() {
    if (currentMode == GameMode::PlayerVsAI) {
        ai = std::make_unique<AI>(currentDifficulty);
    }
    else {
        ai.reset();
    }
    
    NewGame();
}

bool Game::saveGame() {
    std::ofstream saveFile("savegame.txt");
    if (saveFile.is_open()) {
        saveFile << static_cast<int>(currentMode) << "\n";
        saveFile << static_cast<int>(currentDifficulty) << "\n";
        saveFile << static_cast<int>(currentPlayer) << "\n";

        saveFile << board.blackCapture << "\n";
        saveFile << board.whiteCapture << "\n";

        const auto& state = board.getBoardState();
        for (int r = 0; r < board.getSize(); ++r) {
            for (int c = 0; c < board.getSize(); ++c) {
                saveFile << static_cast<int>(state[r][c]) << " ";
            }
            saveFile << "\n";
        }
        saveFile.close();
        return true;
    }
    else {
        return false;
    }
}

bool Game::loadGame() {
    std::ifstream loadFile("savegame.txt");
    if (loadFile.is_open()) {
        int modeInt, diffInt, playerInt;
        loadFile >> modeInt >> diffInt >> playerInt;

        currentMode = static_cast<GameMode>(modeInt);
        currentDifficulty = static_cast<Difficulty>(diffInt);
        startNewGame(currentMode, currentDifficulty); 

        float blackCapture, whiteCaptrue;
        loadFile >> blackCapture >> whiteCaptrue;

        board.whiteCapture = whiteCaptrue;
        board.blackCapture = blackCapture;

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
        return true;
    }
    else {
        return false;
    }
}

bool Game::undoMove() {
    if (!moveHistory.empty()) {
        if (currentMode == GameMode::PlayerVsAI && currentPlayer == Stone::Black) {
            // Undo hai lần để trở về lượt của người chơi
            if (moveHistory.size() >= 2) {
                redoHistory.push(board.getBoardState());
                moveHistory.pop();
                board.setBoardState(moveHistory.top());

                redoHistory.push(board.getBoardState());
                moveHistory.pop();
                if (!moveHistory.empty()) {
                    board.setBoardState(moveHistory.top());
                }
                else {
                    board.reset();
                }
            }
            else {
                // Không đủ nước để undo hai lần, chỉ reset bàn cờ
                redoHistory.push(board.getBoardState());
                moveHistory.pop();
                board.reset();
            }
        }
        else {
            redoHistory.push(board.getBoardState());
            moveHistory.pop();

            if (!moveHistory.empty()) {
                board.setBoardState(moveHistory.top());
            }
            else {
                board.reset();
            }

            currentPlayer = (currentPlayer == Stone::Black) ? Stone::White : Stone::Black;
        }
        
		updateLastGrid();
        return true;
    }
    else {
        return false;
    }
}

bool Game::redoMove() {
    if (!redoHistory.empty()) {
        if (currentMode == GameMode::PlayerVsAI && currentPlayer == Stone::Black) {
            // Redo hai lần để trở về lượt của người chơi
            if (redoHistory.size() >= 2) {
                board.setBoardState(redoHistory.top());
                moveHistory.push(board.getBoardState());
                redoHistory.pop();

                board.setBoardState(redoHistory.top());
                moveHistory.push(board.getBoardState());
                redoHistory.pop();
            }
            else {
				// Không đủ nước để redo hai lần, không làm gì cả
                return false;
            }
        }
        else {
            board.setBoardState(redoHistory.top());
            moveHistory.push(board.getBoardState());
            redoHistory.pop();

            currentPlayer = (currentPlayer == Stone::Black) ? Stone::White : Stone::Black;
		}


        return true;
    }
    else {
        return false;
    }
}

void Game::handleEndGame() {
    std::cerr << "Game Over triggered.\n";
	std::cerr << "consecutivePasses = " << consecutivePasses << std::endl;
    isGameOver = true;

    return;

    std::pair<float, float> finalScores = calculateFinalScores();
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
    std::cerr << (ss.str()) << std::endl;
}

void Game::passTurn() {
    if (isGameOver) {
        return; // Không cho phép bỏ lượt khi game đã kết thúc
    }

    if (currentMode == GameMode::PlayerVsAI && !isGameOver && currentPlayer == Stone::Black && currentDifficulty == Difficulty::Hard) {
        AIHard::reportMove(-1, -1, Stone::Black);
    }

    // Đổi lượt người chơi
    currentPlayer = (currentPlayer == Stone::Black) ? Stone::White : Stone::Black;
    consecutivePasses++; // Tăng biến đếm
    std::cerr << "accessed passTurn()\n";

    std::cout << "Turn passed. Consecutive passes: " << consecutivePasses << std::endl;

    // Lưu lại trạng thái để có thể undo
    moveHistory.push(board.getBoardState());
    while (!redoHistory.empty()) redoHistory.pop();

    // Nếu cả hai người chơi đều bỏ lượt, kết thúc game
    if (consecutivePasses == 2) {
        isGameOver = true;
        handleEndGame();
    }
}

bool Game::placeStone(int row, int col, Stone player) {
    if (row == -1 && col == -1) {
        passTurn();
        return true;
    }

	if (board.placeStone(row, col, player)) {
        // Lưu lại trạng thái để có thể undo

        if (currentMode == GameMode::PlayerVsAI && !isGameOver && currentPlayer == Stone::Black && currentDifficulty == Difficulty::Hard) {
            AIHard::reportMove(row, col, Stone::Black);
        }

        moveHistory.push(board.getBoardState());
        while (!redoHistory.empty()) redoHistory.pop();
        currentPlayer = (currentPlayer == Stone::Black) ? Stone::White : Stone::Black;
        consecutivePasses = 0; // Reset lượt pass liên tiếp khi có nước đi

        return true;
    }
    else {
        return false;
	}
}

std::pair<float, float> Game::calculateFinalScores() const {
    // 1. Lấy điểm lãnh thổ từ đối tượng Board
    const float Komi = 6.5;
    std::pair<int, int> territoryScores = board.calculateScores();
    int blackTerritory = territoryScores.first;
    int whiteTerritory = territoryScores.second;

    // 2. Tính điểm cuối cùng
    // Điểm của Đen = (Lãnh thổ của Đen) + (Số quân Trắng bắt được)
    float finalBlackScore = static_cast<float>(blackTerritory + board.blackCapture);

    // Điểm của Trắng = (Lãnh thổ của Trắng) + (Số quân Đen bắt được) + Komi
    float finalWhiteScore = static_cast<float>(whiteTerritory + board.whiteCapture) + Komi;

	// std::cerr << "Final territory Calculated: Black = " << blackTerritory << ", White = " << whiteTerritory << std::endl;

    return { finalBlackScore, finalWhiteScore };
}

bool Game::AI_move() {
    std::cerr << "AI is thinking...\n" << std::endl;
    Move aiMove = ai->findBestMove(board, Stone::White);
    std::cerr << "foundBestMove...\n" << std::endl;

    if (aiMove.row != -1 && placeStone(aiMove.row, aiMove.col, aiMove.player)) {
        // Giả lập độ trễ cho AI
        
        if (currentDifficulty == Difficulty::Easy) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Độ trễ 500ms
		}

        if (currentDifficulty == Difficulty::Medium) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Độ trễ 500ms
        }

		return true;
    }
    else {
        passTurn(); // Nếu AI không thể di chuyển, coi như nó pass
        return false;
    }
}

void Game::updateLastGrid() {
    if (!moveHistory.empty()) {
        board.lastGrid = moveHistory.top();
	}
}