# ⚪ Go Game Project ⚫

> **Course Project: "Introduction to Programming"**
>
> A digital Go board game developed using C++ and SFML, featuring an intelligent AI opponent.

[![GitHub Repo](https://img.shields.io/badge/GitHub-Repository-black?logo=github)](https://github.com/wiih0521/Project_GoGame)

## 🚀 Features

- **Versatile Game Modes:**
  - 👤 **PvP:** Player vs. Player (Hotseat mode).
  - 🤖 **PvE:** Player vs. Computer (Powered by the **Fuego** AI Engine).
- **Smart AI:** Adaptive difficulty levels suitable for both beginners and intermediate players.
- **Game Utilities:**
  - 💾 **Save/Load:** Save your progress and resume anytime.
  - ↩️ **Undo/Redo:** Review your moves and correct mistakes.
- **Immersive UI:** Clean graphical interface with sound effects and minimal aesthetics.

## 🎮 Controls

The game supports both mouse and keyboard interactions:

| Key / Action | Function |
| :--- | :--- |
| **Left Click** | Place Stone / Interact with UI |
| **S** | Save Game |
| **L** | Load Game |
| **Z** | Undo Move |
| **Y** | Redo Move |
| **P** | Pass Turn |
| **R** | Restart Game |
| **Esc** | Return to Main Menu / Cancel |

## 📦 Installation & How to Play

1. Visit the **[Releases](../../releases)** section of this repository.
2. Download the latest `GoGame_v1.3.zip` file.
3. Extract the downloaded file to a folder.
4. Run `GoGame.exe` to start playing.

> ⚠️ **Important:** Ensure `GoGame.exe` remains in the same folder as the `assets` directory, `fuego.exe` (inside include folder), and the `.dll` files (like `openal32.dll`, `sfml-graphics-2.dll`, etc.) for the game to function correctly.

## 🛠️ Build Instructions (For Developers)

This project was built using **Visual Studio 2022** (x64).

### Prerequisites:
- Visual Studio 2022 (Desktop development with C++ workload).
- **SFML 2.6.x** Library (64-bit).
- **Fuego** AI Engine (executables included in the source).

### Steps:
1. Clone the repository: `git clone https://github.com/wiih0521/Project_GoGame`
2. Open `GoGame.sln` in Visual Studio.
3. Ensure the Include and Library directories point to your SFML installation.
4. Build in **Release x64** mode.
5. Copy the `assets` folder and necessary `.dll` files to the output directory (`x64/Release`).

## 👨‍💻 Authors

- **Faculty of Information Technology - VNU-HCM University of Science**
- *Introduction to Programming - Semester 1, Academic Year 2025-2026*

| Student Name | Student ID | Class |
| :--- | :--- | :--- |
| **Đỗ Gia Huy** | 25125013 | 25A02 |
| **Võ Thanh Hải** | 25125011 | 25A02 |

---
*Made with ❤️ and C++*