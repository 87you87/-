#include <winsock2.h>
#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <thread>
#include <string>

#include "gameUI.h"
#include "game.h"
#include "rep.h"
#include "connect.h"
#include "endingUI.h"

using std::vector;
using std::pair;
using std::string;
using std::wstring;

bool gameUIIsRegistered = false;

bool paintBack_gameui = true;

vector<pair<int,int>> starPoints = {
    {3,3}, {3,11}, {11,3}, {11,11}, {7,7}
};

float leftMargin;
float topMargin;
float rightMargin;
float bottomMargin;

float cellWidth;
float cellHeight;

float drawWidth;
float drawHeight;

int out_row, out_col;
int sendPlayerID = 1;
bool sendClick = false; 

bool mousePosValid = false;

Game gaming::game = Game();

HWND hPrintEdit;
int moveNCounter = 1;

HWND hExitButton;

LRESULT CALLBACK GameUIProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){

    case WM_CREATE:
    {
        gaming::game = Game();
        hPrintEdit = CreateWindowExW(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            L"",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            (int)(rep::clientWidth * 0.78f),
            (int)(rep::clientHeight * 0.15f),
            (int)(rep::clientWidth * 0.18f),
            (int)(rep::clientHeight * 0.75f),
            hWnd,
            (HMENU)1001,
            rep::hInst,
            NULL
        );

        hExitButton = CreateWindowW(
            L"BUTTON",                                  // 按鈕類別
            L"退出",                                    // 按鈕文字
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,   // 樣式
            (int)(rep::clientWidth  * 0.03f),
            (int)(rep::clientHeight * 0.90f),
            (int)(rep::clientWidth  * 0.06f),
            (int)(rep::clientHeight * 0.06f),
            hWnd,                                       // 父視窗
            (HMENU)1002,                                // 控制項 ID（用來辨識按下的是哪個按鈕）
            rep::hInst,
            NULL
        );
        
    }
        return 0;

    case WM_COMMAND:
    {
        WORD notificationCode = HIWORD(wParam); // 高 16 位 → 按鈕通知
        WORD controlID        = LOWORD(wParam); // 低 16 位 → 控制項 ID

        if(notificationCode == BN_CLICKED && controlID == 1002){ 
            if(serverConnecthWnd)   DestroyWindow(serverConnecthWnd);
            if(clientConnecthWnd)   DestroyWindow(clientConnecthWnd);
            serverConnecthWnd = nullptr;
            clientConnecthWnd = nullptr;
            DestroyWindow(hWnd);
            hWnd = nullptr;
        }
    }
        return 0;

    case WM_SETCURSOR:
    {
        // 只處理「客戶區」
        if (LOWORD(lParam) == HTCLIENT)
        {
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return TRUE;  // ★非常重要
        }
    }
        return 0;

    case WM_LBUTTONDOWN:
    {
        if(endingUIhWnd){
            ShowWindow(endingUIhWnd, SW_SHOW);
            return 0;
        }
        if(gaming::isTurnToMe){
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            mousePosValid = HitTestIntersection((float)x, (float)y, out_row, out_col);
            if(gaming::game.getValue(out_row, out_col) != 0)
                return 0;
            if(mousePosValid){
                gaming::game.placePiece(out_row, out_col, gaming::player);
                //重繪棋子區域
                float rc_r = float(out_row);
                float rc_c = float(out_col);
                RECT rc_ = {
                    (LONG)(leftMargin + (rc_c - 0.4) * cellWidth),
                    (LONG)(topMargin  + (rc_r - 0.4) * cellHeight),
                    (LONG)(leftMargin + (rc_c + 0.4) * cellWidth),
                    (LONG)(topMargin  + (rc_r + 0.4) * cellHeight)
                };
                InvalidateRect(gameUIWnd, &rc_, true);

                //傳遞消息
                string s = sendPlacePiaceString(out_row, out_col);
                if(ConnectT::connect_c){
                    ConnectT::connect_c->sendData(s.c_str());
                }
                else if(ConnectT::connect_s){
                    ConnectT::connect_s->sendData(s.c_str());
                }
                gaming::isTurnToMe = false;
                updataUpArea();

                //勝利
                if(gaming::game.checkWin(out_row, out_col, gaming::player)){
                    if(ConnectT::connect_c) 
                        ConnectT::connect_c->sendData("HereVectory");
                    else if(ConnectT::connect_s)
                        ConnectT::connect_s->sendData("HereVectory");
                    //MessageBoxW(hWnd, L"Vectory!", L"Debug", MB_OK);
                    wstring ws;
                    if(gaming::player == 1) ws = L"黑方取得勝利\r\n";
                    if(gaming::player == 2) ws = L"白方取得勝利\r\n";
                    std::thread([ws](){
                        Sleep(1000);
                        AppendText_GameUIPrintEdit(hPrintEdit, ws.c_str());
                    }).detach();
                    gaming::isWin = true;
                    openEndingUIWindow(hWnd,rep::hInst);
                }

            } // if(mousePosValid)
            

        } // isTurnToMe
    }
        return 0;

    case WM_SIZE:
    {
        // 計算邊界留白
        leftMargin   = rep::clientWidth  * 0.15f;  // 左 15%
        topMargin    = rep::clientHeight * 0.15f;  // 上 15%

        rightMargin  = rep::clientWidth  * 0.25f;  // 右 25%
        bottomMargin = rep::clientHeight * 0.10f;  // 下 10%

        // 可用繪圖區域
        drawWidth  = rep::clientWidth  - leftMargin - rightMargin;
        drawHeight = rep::clientHeight - topMargin - bottomMargin;

        MoveWindow(hPrintEdit,
            (int)(rep::clientWidth * 0.78f),
            (int)(rep::clientHeight * 0.15f),
            (int)(rep::clientWidth * 0.18f),
            (int)(rep::clientHeight * 0.75f), TRUE);

        if (endingUIhWnd)  
            MoveWindow(endingUIhWnd, 
                rep::clientWidth  / 4, 
                rep::clientHeight / 4, 
                rep::clientWidth  / 2, 
                rep::clientHeight / 2, TRUE);
        MoveWindow( hExitButton,
                    (int)(rep::clientWidth  * 0.03f),
                    (int)(rep::clientHeight * 0.90f),
                    (int)(rep::clientWidth  * 0.06f),
                    (int)(rep::clientHeight * 0.06f), TRUE);

    }
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Gdiplus::Graphics graphics(hdc);

        if(paintBack_gameui){
            graphics.DrawImage(rep::imgBackGround, 0, 0, rep::clientWidth, rep::clientHeight);
            paintBack_gameui = false;
        }

        Gdiplus::FontFamily fontFamily(L"Arial");
        Gdiplus::REAL fontSize = 30 * (rep::scaleX + rep::scaleY) / 2; 
        Gdiplus::Font font(&fontFamily, fontSize, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::SolidBrush brush_text_black(Gdiplus::Color(255,  0,  0,  0));    //ARGB
        Gdiplus::SolidBrush brush_text_gray (Gdiplus::Color(255,100,100,100));
        Gdiplus::SolidBrush brush_text_white(Gdiplus::Color(255,255,255,255));
        Gdiplus::SolidBrush brush_text_green(Gdiplus::Color(255,  0,255,  0));
        // 設定文字顏色
        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);                // 背景透明
        
        Gdiplus::RectF textRect_bw(        //x y w h
            (rep::clientWidth  * 0.15f), 
            (rep::clientHeight * 0.05f), 
            (rep::clientWidth  * 0.25f), 
            (rep::clientHeight * 0.08f)
        );
        std::wstring ws_bw;
        if (gaming::player == 1){
            ws_bw = L"你是黑方";

            graphics.DrawString(
                ws_bw.c_str(), 
                -1, 
                &font, 
                textRect_bw, 
                NULL,
                &brush_text_black
            );
        }
        if (gaming::player == 2){ 
            ws_bw = L"你是白方";

            graphics.DrawString(
                ws_bw.c_str(), 
                -1, 
                &font, 
                textRect_bw, 
                NULL,
                &brush_text_white
            );
        }
        
        Gdiplus::RectF textRect_turn(        //x y w h
            (rep::clientWidth  * 0.40f), 
            (rep::clientHeight * 0.05f), 
            (rep::clientWidth  * 0.25f), 
            (rep::clientHeight * 0.08f)
        );
        std::wstring ws_turn;
        if(gaming::isTurnToMe){ 
            ws_turn = L"輪到你了";

            graphics.DrawString(
                ws_turn.c_str(), 
                -1, 
                &font, 
                textRect_turn, 
                NULL,
                &brush_text_green
            );
        }
        if(!gaming::isTurnToMe){ 
            ws_turn = L"等待對方";

            graphics.DrawString(
                ws_turn.c_str(), 
                -1, 
                &font, 
                textRect_turn, 
                NULL,
                &brush_text_gray
            );
        }
        

        Gdiplus::Pen pen(Gdiplus::Color(0, 0, 0), 1.5f);
        cellWidth  = drawWidth  / 14.0f;
        cellHeight = drawHeight / 14.0f;

        Gdiplus::Font font_pos(L"Arial", 12 * (rep::scaleX + rep::scaleY) / 2);
        //Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0, 0)); // 黑色

        Gdiplus::StringFormat format_posid;
        format_posid.SetAlignment(Gdiplus::StringAlignmentCenter);

        float textOffsetX = cellWidth * 1.2f;
        for (int i = 0; i < 15; i++){   //標示左邊數字
            int number = i + 1;
            float y = topMargin + (14 - i) * cellHeight;

            Gdiplus::RectF layoutRect(
                leftMargin - textOffsetX,
                y - cellHeight / 4,
                cellWidth,
                cellHeight
            );
            wstring text = std::to_wstring(number);
            graphics.DrawString(
                text.c_str(),
                -1,
                &font_pos,
                layoutRect,
                &format_posid,
                &brush_text_black
            );
        }

        float textOffsetY = cellHeight * 0.3f;  // 文字顯示在最下方交點「下方」的距離
        float bottomY = topMargin + drawHeight; // 最底部的 Y 座標
        for (int j = 0; j < 15; j++){           // 標示下面英文
            float x = leftMargin + j * cellWidth;

            Gdiplus::RectF layoutRect(
                x - cellWidth / 4,
                bottomY + textOffsetY,
                cellWidth,
                cellHeight
            );
            
            wchar_t text = j + 'A';
            wstring wst;
            wst += text;
            graphics.DrawString(
                wst.c_str(),
                -1,
                &font_pos,
                layoutRect,
                nullptr,
                &brush_text_black
            );
        }

        // 繪製水平線
        for (int i = 0; i <= 14; i++)
        {
            float y = topMargin + i * cellHeight;
            graphics.DrawLine(&pen, leftMargin, y, leftMargin + drawWidth, y);
        }

        // 繪製垂直線
        for (int j = 0; j <= 14; j++)
        {
            float x = leftMargin + j * cellWidth;
            graphics.DrawLine(&pen, x, topMargin, x, topMargin + drawHeight);
        }

        Gdiplus::SolidBrush brush_black(Gdiplus::Color(  0,  0,  0));
        Gdiplus::SolidBrush brush_white(Gdiplus::Color(255,255,255));
        Gdiplus::SolidBrush brush_red(Gdiplus::Color(255, 0, 0));
        // 繪製星位
        for (auto [row, col] : starPoints)
        {
            float x = leftMargin + col * cellWidth;
            float y = topMargin  + row * cellHeight;

            // FillEllipse 的座標是左上角，所以要減半徑
            graphics.FillEllipse(&brush_black, x - 3.0f, y - 3.0f, 6.0f, 6.0f);   //x,y,width,height
        }
        
        //繪製棋子
        
        const vector<vector<int>> &board = gaming::game.getBoard();
        //float a = cellWidth * 0.35f;
        //float b = cellHeight * 0.35f;
        float r = (cellWidth < cellHeight) ?  (cellWidth * 0.65f) : (cellHeight * 0.65f);
        for(int row = 0; row < 15; row++){
            for(int col = 0; col < 15; col++){
                float x = leftMargin + col * cellWidth;
                float y = topMargin  + row * cellHeight;
                if(board[row][col] == 1){
                    //MessageBoxA(NULL, "draw black piece", "debug", MB_OK);
                    graphics.FillEllipse(&brush_black, x - r/2.0f, y - r/2.0f, r, r);
                }
                if(board[row][col] == 2){
                    graphics.FillEllipse(&brush_white, x - r/2.0f, y - r/2.0f, r, r);
                }
            }
        }
        
        EndPaint(hWnd, &ps);
    }
        return 0;

    case WM_ERASEBKGND:
    {
        paintBack_gameui = true;
    }
        return 0;

    case WM_APP_TCP_MSG:
    {
        //MessageBoxW(hWnd, L"收到消息(gameui)", L"Debug", MB_OK);
        char* cstr = (char*)lParam;
        int a = (cstr[0] - '0') * 10 + (cstr[1] - '0');
        int b = (cstr[2] - '0') * 10 + (cstr[3] - '0');
        delete[] cstr;
        gaming::game.placePiece(a, b, gaming::player%2+1 );
        
        float rc_r = float(a);
        float rc_c = float(b);
        RECT rc_ = {
            (LONG)(leftMargin + (rc_c - 0.4) * cellWidth),
            (LONG)(topMargin  + (rc_r - 0.4) * cellHeight),
            (LONG)(leftMargin + (rc_c + 0.4) * cellWidth),
            (LONG)(topMargin  + (rc_r + 0.4) * cellHeight)
        };
        InvalidateRect(gameUIWnd, &rc_, true);
        
        updataUpArea();
    }
        return 0;

    case WM_APP_ADDTEXT:
    {
        unsigned int p = (unsigned int)lParam;

        int t = (int)wParam;
        if(t == 10000){
            wstring tws;
            if(p == 1) tws = L"黑方取得勝利\r\n";
            if(p == 2) tws = L"白方取得勝利\r\n";
            std::thread([tws](){
                Sleep(1000);
                AppendText_GameUIPrintEdit(hPrintEdit, tws.c_str());
            }).detach();
            openEndingUIWindow(hWnd,rep::hInst);
            return 0;
        }
        //預計: "1. 黑方 (C , 10)" 換行
        int x = (int)wParam / 100;
        int y = (int)wParam % 100;
        char cy = y + 'A';
        wstring ws = std::to_wstring(moveNCounter);
        if(p == 1)  ws += L". 黑方 (";
        if(p == 2)  ws += L". 白方 (";
        //ws += std::to_wstring(15-x) + wstring(L", ") + wchar_t(cy) + wstring(L")");
        ws += wchar_t(cy) + wstring(L" , ") + std::to_wstring(15-x) + wstring(L")");
        ws += wstring(L"\r\n");
        
        moveNCounter++;
        AppendText_GameUIPrintEdit(hPrintEdit,ws.c_str());
    }
        return 0;

    case WM_DESTROY:
    {
        PostQuitMessage(0);
    }
        return 0;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

HWND gameUIWnd = NULL;
bool RegisterGameUIClass()
{
    WNDCLASSW wc = {};
    wc.lpfnWndProc = GameUIProc;
    wc.hInstance = rep::hInst;
    wc.lpszClassName = L"GameUIWindowClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    return RegisterClass(&wc) != 0;
}

void openGameUIWnd(){
    if(!gameUIIsRegistered){
        gameUIIsRegistered = RegisterGameUIClass();
        if(!gameUIIsRegistered)
            MessageBoxW(NULL, L"Failed to register Game UI class", L"Debug", MB_OK);
    }

    gameUIWnd = CreateWindowExW(
        0,                          // Extended style
        L"GameUIWindowClass",       // 子視窗類別（事先要註冊）
        NULL,                       // 標題列，子視窗不需要
        WS_CHILD | WS_VISIBLE,      // 標準子視窗樣式
        0, 0,                       // x, y
        rep::clientWidth,           // width
        rep::clientHeight,          // height
        rep::mainWindowProc,        // 父視窗
        NULL,                       // menu
        rep::hInst,                 // instance handle
        NULL
    );

    if (!gameUIWnd){
        DWORD err = GetLastError();
        std::wstring ws = L"Failed to create game UI window\nError Code: " + 
                            std::to_wstring(err);
        MessageBoxW(rep::mainWindowProc, ws.c_str(), L"Debug", MB_OK);
    }   

    ShowWindow(gameUIWnd, rep::nCmdShow);
    UpdateWindow(gameUIWnd);
    
    return;
}


bool HitTestIntersection(float mouseX, float mouseY, int& _out_row, int& _out_col)
{

    float temp = (cellWidth < cellHeight) ? cellWidth : cellHeight;
    float tolerance = temp * 0.35f; // 25% 格子寬
    //(檢測距離)


    // 1. 不在棋盤可繪區域
    if (mouseX < leftMargin - temp ||
        mouseY < topMargin - temp ||
        mouseX > rep::clientWidth*1.0f - rightMargin + temp ||
        mouseY > rep::clientHeight*1.0f - bottomMargin + temp)
        return false;

    // 2. 座標轉換
    float localX = mouseX - leftMargin;
    float localY = mouseY - topMargin;

    // 3. 四捨五入找交點(0-14)
    int col = (int)floor((localX + cellWidth  * 0.5f) / cellWidth);
    int row = (int)floor((localY + cellHeight * 0.5f) / cellHeight);

    // 4. 合法?
    if (col < 0 || col > 14 || row < 0 || row > 14)
        return false;

    // 5. 距離檢查（避免誤判）
    float pointX = leftMargin + col * cellWidth;
    float pointY = topMargin + row * cellHeight;

    float dx = mouseX - pointX;
    float dy = mouseY - pointY;

    if (dx*dx + dy*dy > tolerance * tolerance)
        return false;

    // 6. 輸出
    _out_row = row;
    _out_col = col;

    return true;
}


void updataUpArea(){
    RECT rc = {
        0,
        0,
        (long)(rep::clientWidth * 0.8),
        (long)(rep::clientHeight * 0.12)
    };
    InvalidateRect(gameUIWnd, &rc, TRUE);
}

string sendPlacePiaceString(int row, int col){
    //範例: "PlaceBlack0506"
    string s = "Place";
    if (gaming::player == 1) s += "Black";
    else if (gaming::player == 2) s += "White";

    string s_r = std::to_string(row);
    string s_c = std::to_string(col);

    if (s_r.length() == 1)    s_r = "0" + s_r;
    if (s_c.length() == 1)    s_c = "0" + s_c;

    s += s_r + s_c;

    return s;
}

void AppendText_GameUIPrintEdit(HWND _hEdit, const wchar_t* text)
{
    int len = wcslen(text)+1;
    wchar_t* str= new wchar_t[len];
    wcsncpy_s(str, len, text, len);
    
    SendMessageW(hPrintEdit, EM_SETSEL, -1, -1);
    SendMessageW(hPrintEdit, EM_REPLACESEL, FALSE, (LPARAM)str);

    delete[] str;
    return;
}
