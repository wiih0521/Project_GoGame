#include "../include/UI.h"
#include "../include/AI_Hard.h"
#include <iostream>

int main() {
    if (!AIHard::startEngine("include/Fuego/fuego.exe", 19)) {
        std::cerr << "Khong tim thay fuego.exe! Hay tai ve va dat vao thu muc.";
        exit(1);
    }
    else {
        std::cerr << "Da khoi dong fuego" << "\n";
    }

    /*if (!AIHard::startEngine("include/Pachi/pachi.exe", 19)) {
        std::cerr << "Khong tim thay pachi.exe! Hay tai ve va dat vao thu muc.";
        exit(1);
    }
    else {
        std::cerr << "Da khoi dong pachi" << "\n";
    }*/

    UI goGame;
    goGame.run();

    AIHard::stopEngine();

    return 0;
}