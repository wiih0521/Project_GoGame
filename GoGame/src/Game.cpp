#include "../include/Game.h"
#include "../include/UI.h"
#include "../include/AI.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread> 
#include <chrono> 

Game::Game() : board(19), isGameOver(false), currentMode(GameMode::PlayerVsPlayer), currentDifficulty(Difficulty::Easy), currentPlayer(Stone::Black) {
    ai = nullptr;
}

void Game::NewGame() {
    board.reset();
    currentPlayer = Stone::Black;
    isGameOver = false;

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

        const auto& state = board.getBoardState().first;
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

        currentPlayer = static_cast<Stone>(playerInt);

        std::vector<std::vector<Stone>> loadedState(board.getSize(), std::vector<Stone>(board.getSize()));
        for (int r = 0; r < board.getSize(); ++r) {
            for (int c = 0; c < board.getSize(); ++c) {
                int stoneVal;
                loadFile >> stoneVal;
                loadedState[r][c] = static_cast<Stone>(stoneVal);
            }
        }

        if (currentMode == GameMode::PlayerVsAI && currentDifficulty == Difficulty::Hard) {
            AIHard::startNewGame();
            for (int r = 0; r < board.getSize(); ++r) {
                for (int c = 0; c < board.getSize(); ++c) {
                    if (loadedState[r][c] != Stone::None) {
                        AIHard::makeMove(r, c, loadedState[r][c]);
                    }
                }
			}
		}

        board.setBoardState(std::make_pair(loadedState, std::make_pair(whiteCaptrue, blackCapture)));
        loadFile.close();
        return true;
    }
    else {
        return false;
    }
}

bool Game::undoMove() {
    if (currentDifficulty == Difficulty::Hard && currentMode == GameMode::PlayerVsAI) {
        AIHard::undo();
	}

    if (!moveHistory.empty()) {
        if (currentMode == GameMode::PlayerVsAI && currentPlayer == Stone::Black) {
            // Undo hai lần để trở về lượt của người chơi
            if (moveHistory.size() >= 2) {
                redoHistory.push(board);
                moveHistory.pop();
                board.setBoardState(moveHistory.top());

                redoHistory.push(board);
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
                redoHistory.push(board);
                moveHistory.pop();
                board.reset();
            }
        }
        else {
            redoHistory.push(board);
            moveHistory.pop();

            if (!moveHistory.empty()) {
                board.setBoardState(moveHistory.top());
            }
            else {
                board.reset();
            }

            currentPlayer = (currentPlayer == Stone::Black) ? Stone::White : Stone::Black;
        }
        
        return true;
    }
    else {
        return false;
    }
}

bool Game::redoMove() {
    if (currentDifficulty == Difficulty::Hard && currentMode == GameMode::PlayerVsAI) {
        AIHard::redo();
    }

    if (!redoHistory.empty()) {
        if (currentMode == GameMode::PlayerVsAI && currentPlayer == Stone::Black) {
            // Redo hai lần để trở về lượt của người chơi
            if (redoHistory.size() >= 2) {
                board.setBoardState(redoHistory.top());
                moveHistory.push(board);
                redoHistory.pop();

                board.setBoardState(redoHistory.top());
                moveHistory.push(board);
                redoHistory.pop();
            }
            else {
				// Không đủ nước để redo hai lần, không làm gì cả
                return false;
            }
        }
        else {
            board.setBoardState(redoHistory.top());
            moveHistory.push(board);
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
	std::cerr << "consecutivePasses = " << board.consecutivePasses << std::endl;
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

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    ss << "Black Score: " << blackScore << "\n"
        << "White Score: " << whiteScore;
    std::cerr << (ss.str()) << std::endl;
}

void Game::passTurn() {
    if (isGameOver) {
        return; 
    }

    if (currentMode == GameMode::PlayerVsAI && !isGameOver && currentPlayer == Stone::Black && currentDifficulty == Difficulty::Hard) {
        AIHard::reportPlayerMove(-1, -1, Stone::Black);
    }

    currentPlayer = (currentPlayer == Stone::Black) ? Stone::White : Stone::Black;
    board.consecutivePasses++; 
    std::cerr << "accessed passTurn()\n";

    std::cout << "Turn passed. Consecutive passes: " << board.consecutivePasses << std::endl;

    moveHistory.push(board);
    while (!redoHistory.empty()) redoHistory.pop();

    // Nếu cả hai người chơi đều bỏ lượt, kết thúc game
    if (board.consecutivePasses == 2) {
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
        if (currentMode == GameMode::PlayerVsAI && !isGameOver && currentPlayer == Stone::Black && currentDifficulty == Difficulty::Hard) {
            AIHard::reportPlayerMove(row, col, Stone::Black);
        }

        moveHistory.push(board);
        while (!redoHistory.empty()) redoHistory.pop();
        currentPlayer = (currentPlayer == Stone::Black) ? Stone::White : Stone::Black;
        board.consecutivePasses = 0;

        return true;
    }
    else {
        return false;
	}
}

std::pair<float, float> Game::calculateFinalScores() const {
    const float Komi = 6.5;
    std::pair<int, int> territoryScores = board.calculateScores();
    int blackTerritory = territoryScores.first;
    int whiteTerritory = territoryScores.second;

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
        if (currentDifficulty == Difficulty::Easy) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}

        if (currentDifficulty == Difficulty::Medium) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
        }

		return true;
    }
    else {
        passTurn(); // Nếu AI không thể di chuyển, coi như nó pass
        return false;
    }
}

int Game::getBlackCaptured() { return board.blackCapture; }
int Game::getWhiteCaptured() { return board.whiteCapture; }
int Game::consecutivePasses() const { return board.consecutivePasses; }