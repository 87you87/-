#include <vector>
#include <ctime>
#include <cstdlib>
#include <Windows.h>

#include "game.h"
#include "gameUI.h"

using std::vector;
bool gaming::isWin = false;

int Game::getValue(int x, int y){
    return board[x][y];
}

Game::Game(){
    board = vector<vector<int>>(15, vector<int>(15, 0)); // 初始化15x15的棋盤，所有位置設為0
}
Game::~Game(){}

void Game::placePiece(int x, int y, unsigned char player){
    if (x >= 0 && x < 15 && y >= 0 && y < 15) {
        board[x][y] = player; // player是 1 = black 或 2 = white
        // x = row, y = col
        //(0,0) (0,1) (0,2)
        //(1,0)
        //(2,0)
        //(3,0)
        PostMessageA(gameUIWnd, WM_APP_ADDTEXT, x * 100 + y, player);
    }
}

bool Game::inBoard(int x, int y){
    return (x >= 0 && x < 15 && y >= 0 && y < 15);
}

bool Game::checkWin(int x, int y, unsigned char player){
    int n = 1, count = 1;
    bool flag1 = true, flag2 = true;
    while(flag1 || flag2){      // 左-右
        if(inBoard(x+n,y) && flag1){
            if(board[x+n][y] == player)
                count++;
            else
                flag1 = false;
        }else
            flag1 = false;

        if(inBoard(x-n,y) && flag2){
            if(board[x-n][y] == player)
                count++;
            else
                flag2 = false;
        }else
            flag2 = false;

        if(count >= 5){
            return true;
        }
        n++;
    } // while
    
    n = 1;  count = 1;
    flag1 = true;   flag2 = true;
    while(flag1 || flag2){      // 上-下
        if(inBoard(x,y+n) && flag1){
            if(board[x][y+n] == player)
                count++;
            else
                flag1 = false;
        }else
            flag1 = false;

        if(inBoard(x,y-n) && flag2){
            if(board[x][y-n] == player)
                count++;
            else
                flag2 = false;
        }else
            flag2 = false;

        if(count >= 5){
            return true;
        }
        n++;
    } // while

    n = 1;  count = 1;
    flag1 = true;   flag2 = true;
    while(flag1 || flag2){      // 左上-右下
        if(inBoard(x+n,y+n) && flag1){
            if(board[x+n][y+n] == player)
                count++;
            else
                flag1 = false;
        }else
            flag1 = false;

        if(inBoard(x-n,y-n) && flag2){
            if(board[x-n][y-n] == player)
                count++;
            else
                flag2 = false;
        }else
            flag2 = false;

        if(count >= 5){
            return true;
        }
        n++;
    } // while

    n = 1;  count = 1;
    flag1 = true;   flag2 = true;
    while(flag1 || flag2){      // 右上-左下
        if(inBoard(x-n,y+n) && flag1){
            if(board[x-n][y+n] == player)
                count++;
            else
                flag1 = false;
        }else
            flag1 = false;

        if(inBoard(x+n,y-n) && flag2){
            if(board[x+n][y-n] == player)
                count++;
            else
                flag2 = false;
        }else
            flag2 = false;

        if(count >= 5){
            return true;
        }
        n++;
    } // while

    return false;
}

vector<vector<int>> Game::getBoard() const { 
    return board; 
}


bool gaming::isTurnToMe = false;
unsigned char gaming::player = 0;
int gaming::getRandInt(){
    srand(time(nullptr));
    int r = rand();

    return r;
}