#ifndef AI_HARD_H
#define AI_HARD_H

#include "../include/Board.h"
#include <string>
#include <utility>
#include <windows.h> // Thư viện cần thiết để gọi file .exe trên Windows

class AIHard {
public:
    // 1. Khởi động Fuego (Gọi hàm này 1 lần khi bắt đầu Game)
    // path: đường dẫn đến file "fuego.exe"
    // size: kích thước bàn cờ (9, 13, 19)
    static bool startEngine(std::string path, int size);

    // 2. Đóng Fuego (Gọi khi tắt game)
    static void stopEngine();

    // 3. Hàm chính: Bảo Fuego suy nghĩ và đi
    static Move getBestMove(Stone playerColor);

    // 4. Quan trọng: Báo cho Fuego biết người chơi vừa đi đâu
    // (Nếu không báo, Fuego sẽ không biết bàn cờ đã thay đổi)
    static void reportMove(int row, int col, Stone playerColor);

    static void startNewGame();

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
};

#endif