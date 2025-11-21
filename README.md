# ⚪ Go Game Project ⚫

> **Đồ án môn "Introduction to Programming"**
>
> Một tựa game Cờ Vây được viết bằng C++ và thư viện SFML, tích hợp AI thông minh.

## 🚀 Tính năng

- **Chế độ chơi đa dạng:**
  - 👤 **PvP:** Hai người chơi đấu với nhau trên cùng một máy.
  - 🤖 **PvE:** Người đấu với Máy (Sử dụng Engine AI Pachi mạnh mẽ).
- **Hệ thống AI:** Hỗ trợ các cấp độ từ Dễ đến Khó.
- **Tiện ích:**
  - 💾 **Save/Load:** Lưu và tải lại ván cờ đang đánh dở.
  - ↩️ **Undo/Redo:** Hoàn tác hoặc làm lại nước đi nếu đánh nhầm.
- **Giao diện:** Đồ họa SFML đơn giản, trực quan, có âm thanh.

## 🎮 Hướng dẫn điều khiển

Game sử dụng chuột và bàn phím để thao tác:

| Phím / Thao tác | Chức năng |
| :--- | :--- |
| **Chuột trái** | Đặt quân cờ |
| **Phím S** | Lưu game (Save) |
| **Phím L** | Tải game (Load) |
| **Phím Z** | Hoàn tác nước đi (Undo) |
| **Phím Y** | Làm lại nước đi (Redo) |
| **Phím R** | Chơi lại ván mới (Restart) |

## 📦 Hướng dẫn Cài đặt & Chơi 

1. Truy cập mục **[Releases](../../releases)** của repository này.
2. Tải file nén `GoGame_v1.0.zip` mới nhất.
3. Giải nén ra thư mục.
4. Chạy file `GoGame.exe` để chơi ngay.
   > *Lưu ý: Không tách rời file `.exe` khỏi thư mục chứa `assets`, `pachi.exe` và các file `.dll`.*

## 🛠️ Hướng dẫn Biên dịch (Dành cho Dev)

Dự án được phát triển trên **Visual Studio 2022** (x64).

### Yêu cầu:
- Visual Studio 2022 (Workload: Desktop development with C++).
- Thư viện **SFML 2.6.x** (64-bit).
- Engine **Pachi** (đã bao gồm trong source).

## 👨‍💻 Thông tin tác giả

- **Họ và tên:** Đỗ Gia Huy
- **MSSV:** 25125013
- **Lớp:** 25A02
- **Trường:** Trường Đại học Khoa học Tự Nhiên

- **Họ và tên:** Võ Thanh Hải
- **MSSV:** 25125011
- **Lớp:** 25A02
- **Trường:** Trường Đại học Khoa học Tự Nhiên

*Đồ án môn Introduction to Programming - Học kỳ 1 Năm học [2025-2026]*