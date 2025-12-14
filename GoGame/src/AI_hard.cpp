#include "../include/AI_hard.h"
#include <iostream>
#include <vector>
#include <sstream>

HANDLE AIHard::g_hChildStd_IN_Wr = NULL;
HANDLE AIHard::g_hChildStd_OUT_Rd = NULL;
HANDLE AIHard::g_hChildProcess = NULL;
int AIHard::currentBoardSize = 19;
std::stack<Move> AIHard::moveHistory;
std::stack<Move> AIHard::redoHistory;

bool AIHard::startEngine(std::string path, int size) {
    currentBoardSize = size;
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE hChildStd_IN_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;

    if (!CreatePipe(&g_hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0)) return false;
    if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) return false;

    if (!CreatePipe(&hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) return false;
    if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) return false;

    HANDLE hNullFile = CreateFileA("NUL", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
        &saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA siStartInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);

    siStartInfo.hStdInput = hChildStd_IN_Rd;
    siStartInfo.hStdOutput = hChildStd_OUT_Wr; 
    siStartInfo.hStdError = (hNullFile != INVALID_HANDLE_VALUE) ? hNullFile : NULL;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    std::string cmdLine = "\"" + path + "\"";
    std::vector<char> cmdVec(cmdLine.begin(), cmdLine.end());
    cmdVec.push_back(0);

    BOOL bSuccess = CreateProcessA(NULL, cmdVec.data(), NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);

    CloseHandle(hChildStd_OUT_Wr);
    CloseHandle(hChildStd_IN_Rd);
    if (hNullFile != INVALID_HANDLE_VALUE) CloseHandle(hNullFile); 

    if (!bSuccess) {
        std::cerr << "Khong the khoi dong bot! Hay kiem tra duong dan." << std::endl;
        return false;
    }

    g_hChildProcess = piProcInfo.hProcess;
    CloseHandle(piProcInfo.hThread);

    std::cerr << "--- Dang khoi dong bot (Che do im lang) ---" << std::endl;

    Sleep(100); 

    std::string handshake = sendCommand("name");

    if (handshake.length() > 0 && handshake[0] == '=') {
        std::cerr << ">>> Ket noi thanh cong! Bot da san sang." << std::endl;

        sendCommand("boardsize " + std::to_string(size));
        // sendCommand("komi 6.5");
        return true;
    }
    else {
        std::cerr << ">>> Ket noi THAT BAI. Bot khong phan hoi." << std::endl;
        return false;
    }
}

void AIHard::startNewGame() {
    sendCommand("clear_board");
    sendCommand("komi 6.5");
}

void AIHard::stopEngine() {
    sendCommand("quit");
    if (g_hChildProcess) {
        WaitForSingleObject(g_hChildProcess, 1000); 
        CloseHandle(g_hChildProcess);
        CloseHandle(g_hChildStd_IN_Wr);
        CloseHandle(g_hChildStd_OUT_Rd);
        g_hChildProcess = NULL;
    }
}

std::string AIHard::sendCommand(std::string cmd) {
    if (!g_hChildStd_IN_Wr) return "";

    std::string cmdToSend = cmd + "\n";
    std::cout << "[Send]: " << cmd << std::endl;

    DWORD dwWritten;
    if (!WriteFile(g_hChildStd_IN_Wr, cmdToSend.c_str(), cmdToSend.length(), &dwWritten, NULL)) {
        std::cerr << "[Error] Khong ghi duoc vao Pipe." << std::endl;
        return "";
    }

    FlushFileBuffers(g_hChildStd_IN_Wr);

    std::string output = "";
    char buffer[1024];
    DWORD dwRead;
    
    bool finished = false;
    int timeoutCount = 0;
    output = "";

    while (!finished) {
        DWORD dwAvail = 0;

        DWORD exitCode;
        if (GetExitCodeProcess(g_hChildProcess, &exitCode) && exitCode != STILL_ACTIVE) {
            std::cerr << "[Error] Bot process da chet! Exit code: " << exitCode << std::endl;
            break;
        }

        if (!PeekNamedPipe(g_hChildStd_OUT_Rd, NULL, 0, NULL, &dwAvail, NULL)) {
            break; // Lỗi Pipe
        }

        if (dwAvail > 0) {
            if (!ReadFile(g_hChildStd_OUT_Rd, buffer, sizeof(buffer) - 1, &dwRead, NULL) || dwRead == 0) {
                break;
            }
            buffer[dwRead] = '\0'; 
            output += buffer;

            if (output.find("\n\n") != std::string::npos || output.find("\r\n\r\n") != std::string::npos) {
                finished = true;
            }

            timeoutCount = 0; 
        }
        else {
            Sleep(10);
            timeoutCount++;

            if (timeoutCount > 1000) {
                std::cerr << "[TIMEOUT] Bot khong phan hoi sau 1 giay." << std::endl;
                break;
            }
        }
    }
    
    std::cout << "[Receive]: " << output << " --- " << "received" << std::endl;

	int inPos = output.find("=");
    if (inPos != std::string::npos) {
        output = output.substr(inPos);
	}

    while (!output.empty() && (output.back() == '\n' || output.back() == '\r')) {
        output.pop_back();
    }

    return output;
}

std::string AIHard::coordsToString(int row, int col, int boardSize) {
    if (row == -1) return "pass";

    char colChar = 'A' + col;
    if (colChar >= 'I') colChar++; // GTP bỏ qua chữ I để tránh nhầm với số 1

    int rowGTP = boardSize - row; // Đảo ngược trục Y
    return std::string(1, colChar) + std::to_string(rowGTP);
}

std::pair<int, int> AIHard::stringToCoords(std::string gtpCoord, int boardSize) {
    if (gtpCoord.empty()) {
        std::cerr << "[CRITICAL ERROR] Bot tra ve chuoi rong! Co the Engine da bi tat.\n";
        return { -1, -1 };
    }

    size_t eqPos = gtpCoord.find("= ");
    if (eqPos != std::string::npos) {
        gtpCoord = gtpCoord.substr(eqPos + 2);
    }

    while (!gtpCoord.empty() && (gtpCoord.back() == '\n' || gtpCoord.back() == ' ' || gtpCoord.back() == '\r')) {
        gtpCoord.pop_back();
    }

    if (gtpCoord.empty()) return { -1, -1 };
    if (gtpCoord == "PASS" || gtpCoord == "pass") return { -1, -1 };

    char colChar = gtpCoord[0];
    if (gtpCoord.length() < 2) return { -1, -1 };

    std::string rowStr = gtpCoord.substr(1);

    if (colChar >= 'a') colChar -= 32;
    int col = colChar - 'A';
    if (colChar > 'I') col--;

    try {
        int rowGTP = std::stoi(rowStr);
        int row = boardSize - rowGTP;
        return { row, col };
    }
    catch (...) {
        return { -1, -1 }; 
    }
}

void AIHard::reportPlayerMove(int row, int col, Stone playerColor) {
    std::string colorStr = (playerColor == Stone::Black) ? "b" : "w";

    if (row == -1 && col == -1) {
        std::string cmd = "play " + colorStr + " " + "pass";
        sendCommand(cmd);
    }
    else {
        std::string coordStr = coordsToString(row, col, currentBoardSize);
        std::string cmd = "play " + colorStr + " " + coordStr;
        sendCommand(cmd);
    }

	moveHistory.push({ row, col, playerColor });
	while (!redoHistory.empty()) redoHistory.pop();
}

Move AIHard::getBestMove(Stone playerColor) {
    std::string colorStr = (playerColor == Stone::Black) ? "b" : "w";
    std::string cmd = "genmove " + colorStr;

    std::string response = sendCommand(cmd);

    std::pair<int, int> bestMove = stringToCoords(response, currentBoardSize);

	moveHistory.push({ bestMove.first, bestMove.second, playerColor });
    while (!redoHistory.empty()) redoHistory.pop();

	return { bestMove.first, bestMove.second, playerColor };
}

void AIHard::undo() {
    if (moveHistory.size() < 2) return;

    for (int i = 0; i < 2; ++i) {
        redoHistory.push(moveHistory.top());
        moveHistory.pop();

        sendCommand("undo");
    }
}

void AIHard::makeMove(int row, int col, Stone playerColor) {
    std::string colorStr = (playerColor == Stone::Black) ? "b" : "w";

    if (row == -1 && col == -1) {
        std::string cmd = "play " + colorStr + " " + "pass";
        sendCommand(cmd);
    }
    else {
        std::string coordStr = coordsToString(row, col, currentBoardSize);
        std::string cmd = "play " + colorStr + " " + coordStr;
        sendCommand(cmd);
    }
}


void AIHard::redo() {
    if (redoHistory.size() < 2) return;

    for (int i = 0; i < 2; ++i) {
        makeMove(redoHistory.top().row, redoHistory.top().col, redoHistory.top().player);

        moveHistory.push(redoHistory.top());
        redoHistory.pop();
    }
}