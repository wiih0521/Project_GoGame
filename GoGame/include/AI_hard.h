#ifndef AI_HARD_H
#define AI_HARD_H

#include "../include/Board.h"
#include <string>
#include <utility>
#include <stack>
#include <windows.h> // Thư viện cần thiết để gọi file .exe trên Windows

class AIHard {
public:
    static bool startEngine(std::string path, int size);
    static void stopEngine();

    static void startNewGame();
    static Move getBestMove(Stone playerColor);
    static void reportPlayerMove(int row, int col, Stone playerColor);
    static void makeMove(int row, int col, Stone playerColor);

    static void undo();
    static void redo();

private:
    static HANDLE g_hChildStd_IN_Wr; // Đường ghi lệnh vào Fuego
    static HANDLE g_hChildStd_OUT_Rd; // Đường đọc kết quả từ Fuego
    static HANDLE g_hChildProcess;

    // Gửi lệnh GTP (Go Text Protocol)
    static std::string sendCommand(std::string cmd);

    // Chuyển đổi tọa độ (0,0) <-> "A19"
    static std::string coordsToString(int row, int col, int boardSize);
    static std::pair<int, int> stringToCoords(std::string gtpCoord, int boardSize);

    static int currentBoardSize;

    static std::stack<Move> moveHistory;
    static std::stack<Move> redoHistory;
};

#endif