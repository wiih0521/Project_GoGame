#include "../include/AI_hard.h"
#include <iostream>
#include <vector>
#include <sstream>

// Khai báo các biến tĩnh
HANDLE AIHard::g_hChildStd_IN_Wr = NULL;
HANDLE AIHard::g_hChildStd_OUT_Rd = NULL;
HANDLE AIHard::g_hChildProcess = NULL;
int AIHard::currentBoardSize = 19;

// --- PHẦN GIAO TIẾP WINDOWS PIPE (Hơi phức tạp, dùng để nối dây với .exe) ---

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

    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA siStartInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = hChildStd_OUT_Wr;
    siStartInfo.hStdInput = hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // --- SỬA Ở ĐÂY ---
    // Bỏ --gtp và thêm ngoặc kép cho path
    //std::string cmdLine = "\"" + path + "\"";
    std::string cmdLine = "\"" + path + "\" -d 0";
    // ----------------

    std::vector<char> cmdVec(cmdLine.begin(), cmdLine.end());
    cmdVec.push_back(0);

    // Lưu ý: CreateProcessA tham số thứ 2 (lpCommandLine) sẽ bị chỉnh sửa, nên dùng vector char là đúng rồi
    BOOL bSuccess = CreateProcessA(NULL, cmdVec.data(), NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);

    CloseHandle(hChildStd_OUT_Wr);
    CloseHandle(hChildStd_IN_Rd);

    if (!bSuccess) {
        std::cerr << "Khong the khoi dong bot! Hay kiem tra duong dan." << std::endl;
        return false;
    }

    g_hChildProcess = piProcInfo.hProcess;
    CloseHandle(piProcInfo.hThread);

    std::cout << "--- Dang khoi dong bot ---" << std::endl;

    // Vòng lặp: Đọc liên tục cho đến khi Fuego im lặng quá 500ms
    char buffer[4096];
    DWORD dwRead;
    DWORD dwAvail = 0;
    int silenceTime = 0;

    while (silenceTime < 50) { // 50 * 10ms = 500ms im lặng
        // Kiểm tra xem có dữ liệu mới không
        if (PeekNamedPipe(g_hChildStd_OUT_Rd, NULL, 0, NULL, &dwAvail, NULL) && dwAvail > 0) {
            // Có dữ liệu -> Đọc hết sạch
            if (ReadFile(g_hChildStd_OUT_Rd, buffer, sizeof(buffer) - 1, &dwRead, NULL) && dwRead > 0) {
                buffer[dwRead] = '\0';
                std::cout << buffer; // In ra để debug xem nó nói gì
                silenceTime = 0; // Reset thời gian chờ vì vừa nhận được dữ liệu
            }
        }
        else {
            // Không có dữ liệu -> Ngủ 1 tí
            Sleep(10);
            silenceTime++;
        }

        // Kiểm tra lỡ Fuego chết giữa chừng
        DWORD exitCode;
        if (GetExitCodeProcess(g_hChildProcess, &exitCode) && exitCode != STILL_ACTIVE) {
            std::cerr << "bot da tat dot ngot khi dang khoi dong!" << std::endl;
            return false;
        }
    }

    std::cout << "het intro roi day" << "\n";

    std::cout << "\n--- bot da san sang ---" << std::endl;

    // 3. Gửi lệnh "BẮT TAY" (Handshake)
// Lệnh 'name' là lệnh chuẩn GTP, engine nào cũng phải trả lời
    std::string handshake = sendCommand("name");

    // 4. Kiểm tra xem nó có trả lời đàng hoàng không
    // Phản hồi đúng phải bắt đầu bằng dấu "="
    if (handshake.length() > 0 && handshake[0] == '=') {
        std::cout << ">>> Ket noi thanh cong! bot da san sang." << std::endl;

        // Bây giờ mới gửi các lệnh cài đặt
        sendCommand("boardsize " + std::to_string(size));
        sendCommand("komi 6.5");
        // sendCommand("clear_board");
        return true;
    }
    else {
        std::cerr << ">>> Ket noi THAT BAI. bot khong phan hoi lenh handshake." << std::endl;
        return false;
    }

    return true;
}

void AIHard::startNewGame() {
    sendCommand("clear_board");
    sendCommand("komi 6.5");
}

void AIHard::stopEngine() {
    sendCommand("quit");
    if (g_hChildProcess) {
        WaitForSingleObject(g_hChildProcess, 1000); // Đợi 1s để tắt
        CloseHandle(g_hChildProcess);
        CloseHandle(g_hChildStd_IN_Wr);
        CloseHandle(g_hChildStd_OUT_Rd);
        g_hChildProcess = NULL;
    }
}

std::string AIHard::sendCommand(std::string cmd) {
    // 1. Kiểm tra Pipe đầu vào có hợp lệ không
    if (!g_hChildStd_IN_Wr) return "";

    // 2. Chuẩn bị lệnh: GTP bắt buộc phải có ký tự xuống dòng (\n)
    std::string cmdToSend = cmd + "\n";

    // Log kiểm tra
    std::cout << "[Gui]: " << cmd << std::endl;

    // 3. Gửi lệnh
    DWORD dwWritten;
    if (!WriteFile(g_hChildStd_IN_Wr, cmdToSend.c_str(), cmdToSend.length(), &dwWritten, NULL)) {
        std::cerr << "[LOI] Khong ghi duoc vao Pipe." << std::endl;
        return "";
    }

    // 4. QUAN TRỌNG NHẤT: Đẩy dữ liệu đi ngay lập tức
    // Nếu thiếu dòng này, lệnh sẽ nằm trong bộ đệm và Fuego không nhận được -> Gây treo
    FlushFileBuffers(g_hChildStd_IN_Wr);

    // 5. Vòng lặp đọc phản hồi
    std::string output = "";
    char buffer[1024];
    DWORD dwRead;
    
    bool finished = false;
    int timeoutCount = 0;
    output = "";

    while (!finished) {
        DWORD dwAvail = 0;

        // A. Kiểm tra xem Fuego còn sống không? (Tránh treo vĩnh viễn nếu Engine crash)
        DWORD exitCode;
        if (GetExitCodeProcess(g_hChildProcess, &exitCode) && exitCode != STILL_ACTIVE) {
            std::cerr << "[LOI] Bot process da chet! Exit code: " << exitCode << std::endl;
            break;
        }

        // B. Kiểm tra xem có dữ liệu trong ống không
        if (!PeekNamedPipe(g_hChildStd_OUT_Rd, NULL, 0, NULL, &dwAvail, NULL)) {
            break; // Lỗi Pipe
        }

        if (dwAvail > 0) {
            // Có dữ liệu -> Đọc ngay
            if (!ReadFile(g_hChildStd_OUT_Rd, buffer, sizeof(buffer) - 1, &dwRead, NULL) || dwRead == 0) {
                break;
            }
            buffer[dwRead] = '\0'; // Ngắt chuỗi
            output += buffer;

            // C. Kiểm tra dấu hiệu kết thúc của giao thức GTP
            // Theo chuẩn GTP, câu trả lời kết thúc bằng 2 dòng trống (\n\n)
            if (output.find("\n\n") != std::string::npos || output.find("\r\n\r\n") != std::string::npos) {
                finished = true;
            }

            timeoutCount = 0; // Reset timeout nếu đọc được dữ liệu
        }
        else {
            // D. Nếu chưa có dữ liệu -> Ngủ 10ms để giảm tải CPU
            Sleep(10);
            timeoutCount++;

            // E. Timeout: Nếu đợi quá 10 giây (1000 * 10ms) mà không xong -> Thoát
            if (timeoutCount > 1000) {
                std::cerr << "[TIMEOUT] Bot khong phan hoi sau 4 giay." << std::endl;
                break;
            }
        }
    }

    // Xử lý chuỗi kết quả (Cắt bỏ khoảng trắng thừa)
    // Xóa \n ở cuối
    // Xóa đoạn đọc IN: ...
    
    std::cout << "[Nhan]: " << output << " --- " << std::endl;

	int inPos = output.find("=");
    if (inPos != std::string::npos) {
        output = output.substr(inPos);
	}

    while (!output.empty() && (output.back() == '\n' || output.back() == '\r')) {
        output.pop_back();
    }

    return output;
}
// --- PHẦN XỬ LÝ LOGIC CỜ VÂY ---

// Chuyển đổi tọa độ: Fuego dùng A19 (Góc trên trái), Board dùng (0,0) (Góc trên trái)
// Lưu ý: GTP cột là A-T (bỏ chữ I), Hàng từ dưới đếm lên (1 ở dưới, 19 ở trên)
std::string AIHard::coordsToString(int row, int col, int boardSize) {
    if (row == -1) return "pass";

    char colChar = 'A' + col;
    if (colChar >= 'I') colChar++; // GTP bỏ qua chữ I để tránh nhầm với số 1

    int rowGTP = boardSize - row; // Đảo ngược trục Y
    return std::string(1, colChar) + std::to_string(rowGTP);
}

std::pair<int, int> AIHard::stringToCoords(std::string gtpCoord, int boardSize) {
    // --- [FIX] THÊM ĐOẠN NÀY ĐỂ CHỐNG CRASH ---
    if (gtpCoord.empty()) {
        std::cerr << "[CRITICAL ERROR] Bot tra ve chuoi rong! Co the Engine da bi tat.\n";
        return { -1, -1 };
    }
    // ------------------------------------------

    size_t eqPos = gtpCoord.find("= ");
    if (eqPos != std::string::npos) {
        gtpCoord = gtpCoord.substr(eqPos + 2);
    }

    // Xóa ký tự xuống dòng/khoảng trắng
    while (!gtpCoord.empty() && (gtpCoord.back() == '\n' || gtpCoord.back() == ' ' || gtpCoord.back() == '\r')) {
        gtpCoord.pop_back();
    }

    // --- [FIX] KIỂM TRA LẠI SAU KHI CẮT ---
    if (gtpCoord.empty()) return { -1, -1 };
    if (gtpCoord == "PASS" || gtpCoord == "pass") return { -1, -1 };
    // --------------------------------------

    char colChar = gtpCoord[0];
    // Kiểm tra độ dài để tránh crash khi đọc rowStr
    if (gtpCoord.length() < 2) return { -1, -1 };

    std::string rowStr = gtpCoord.substr(1);

    // ... (phần còn lại giữ nguyên)
    if (colChar >= 'a') colChar -= 32;
    int col = colChar - 'A';
    if (colChar > 'I') col--;

    try {
        int rowGTP = std::stoi(rowStr);
        int row = boardSize - rowGTP;
        return { row, col };
    }
    catch (...) {
        return { -1, -1 }; // Bắt lỗi nếu stoi thất bại
    }
}

// --- CÁC HÀM PUBLIC MÀ GAME SẼ GỌI ---

void AIHard::reportMove(int row, int col, Stone playerColor) {
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

Move AIHard::getBestMove(Stone playerColor) {
    // Gửi lệnh: "genmove w" (Trắng đi đi)
    std::string colorStr = (playerColor == Stone::Black) ? "b" : "w";
    std::string cmd = "genmove " + colorStr;

    std::string response = sendCommand(cmd);

    // Phân tích phản hồi từ Fuego
    std::pair<int, int> bestMove = stringToCoords(response, currentBoardSize);
	return { bestMove.first, bestMove.second, playerColor };
}