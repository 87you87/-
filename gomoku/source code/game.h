#pragma once

#include <vector>

#define WM_APP_ADDTEXT (WM_APP + 3)

using std::vector;

class Game {
private:
    vector<vector<int>> board;

public:
    Game();
    ~Game();

    void placePiece(int x, int y, unsigned char player);
    bool inBoard(int x, int y);
    bool checkWin(int x, int y, unsigned char player);

    int getValue(int, int);
    vector<vector<int>> getBoard() const;
};

namespace gaming{
    extern unsigned char player;
    extern bool isTurnToMe;
    int getRandInt();
    extern Game game;
    extern bool isWin;
};