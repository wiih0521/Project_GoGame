# Go Game Project

Đây là dự án game Cờ vây được viết bằng C++ và thư viện SFML, là bài tập cuối kỳ cho môn học "Introduction to Programming".

## Tính năng

- Chế độ chơi: Player vs Player, Player vs AI.
- AI có 3 cấp độ: Dễ, Trung bình, Khó (cần hoàn thiện).
- Lưu và tải game.
- Undo/Redo nước đi.
- Giao diện đồ họa đơn giản.

## Hướng dẫn biên dịch

1.  **Cài đặt SFML:** Tải và cài đặt thư viện SFML phù hợp với hệ điều hành và trình biên dịch của bạn từ [trang chủ SFML](https://www.sfml-dev.org/download.php).
2.  **Cấu hình Compiler:** Thiết lập Include Paths và Library Paths trong IDE/trình biên dịch của bạn để trỏ đến thư mục `include` và `lib` của SFML.
3.  **Link Libraries:** Thêm các thư viện SFML cần thiết vào trình liên kết (linker). Thường là: `sfml-graphics`, `sfml-window`, `sfml-system`, `sfml-audio`.
4.  **Biên dịch:** Biên dịch tất cả các tệp `.cpp` trong thư mục `src`.

Ví dụ lệnh biên dịch với g++:
`g++ src/*.cpp -o GoGame -I./include -lsfml-graphics -lsfml-window -lsfml-system`

## Cách chơi

- **Click chuột trái:** Đặt quân cờ.
- **Phím S:** Lưu game.
- **Phím L:** Tải game.
- **Phím Z:** Undo.
- **Phím Y:** Redo.
- **Phím R:** Chơi lại game mới.