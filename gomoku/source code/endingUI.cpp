#include <Windows.h>
#include <string>

#include "endingUI.h"
#include "rep.h"
#include "game.h"
#include "gameUI.h"
#include "connectUI.h"

HWND endingUIhWnd;
bool endingUIRegistered = false;
RECT clientRect_endingui;

HWND hCancelBTN;
HWND hCheckBoardBTN;
HWND hAgainBtn;


LRESULT CALLBACK endingUIWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){
    case WM_CREATE:
    {
        GetClientRect(hWnd, &clientRect_endingui);
        long width = clientRect_endingui.right - clientRect_endingui.left;
        long height = clientRect_endingui.bottom - clientRect_endingui.top;

        hCancelBTN = CreateWindowW(
            L"BUTTON",                                  // 按鈕類別
            L"退出",                                    // 按鈕文字
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,   // 樣式
            (int)(width  * 0.15),
            (int)(height * 0.75),
            (int)(width  * 0.20),
            (int)(height * 0.15),
            hWnd,                                       // 父視窗
            (HMENU)1001,                                // 控制項 ID（用來辨識按下的是哪個按鈕）
            rep::hInst,
            NULL
        );
        hCheckBoardBTN = CreateWindowW(
            L"BUTTON",                                  // 按鈕類別
            L"查看棋盤",                                    // 按鈕文字
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,   // 樣式
            (int)(width  * 0.40),
            (int)(height * 0.75),
            (int)(width  * 0.20),
            (int)(height * 0.15),
            hWnd,                                       // 父視窗
            (HMENU)1002,                                // 控制項 ID（用來辨識按下的是哪個按鈕）
            rep::hInst,
            NULL
        );
        /* 再來一局
        hAgainBtn = CreateWindowW(
            L"BUTTON",                                  // 按鈕類別
            L"再來一局",                                    // 按鈕文字
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,   // 樣式
            (int)(width  * 0.65),                       // x
            (int)(height * 0.75),                       // y
            (int)(width  * 0.20),                       // width
            (int)(height * 0.15),                       // height
            hWnd,                                       // 父視窗
            (HMENU)1003,                                // 控制項 ID（用來辨識按下的是哪個按鈕）
            rep::hInst,
            NULL
        );*/

    }
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Gdiplus::Graphics graphics(hdc);

        Gdiplus::FontFamily fontFamily(L"Arial");
        Gdiplus::REAL fontSize = 30 * (rep::scaleX + rep::scaleY) / 2; 
        Gdiplus::Font font(&fontFamily, fontSize, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::SolidBrush brush_yellow_1(Gdiplus::Color(255, 0,205,205));    //ARGB
        Gdiplus::SolidBrush brush_gray(Gdiplus::Color(255, 90, 90, 90));

        // 設定文字顏色
        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);                // 背景透明

        Gdiplus::RectF textRect(        //x y w h
            (clientRect_endingui.right  * 0.30f), 
            (clientRect_endingui.bottom * 0.15f), 
            (clientRect_endingui.right  * 0.40f), 
            (clientRect_endingui.bottom * 0.40f)
        );

        Gdiplus::StringFormat str_format;
        str_format.SetAlignment(Gdiplus::StringAlignmentCenter);        // 水平置中
        str_format.SetLineAlignment(Gdiplus::StringAlignmentCenter);    // 垂直置中

        if(gaming::isWin){
            graphics.DrawString(
                L"VECTORY", 
                -1, 
                &font, 
                textRect, 
                &str_format,
                &brush_yellow_1
            );
        }
        if(!gaming::isWin){
            graphics.DrawString(
                L"LOSE", 
                -1, 
                &font, 
                textRect, 
                &str_format,
                &brush_gray
            );
        }

        EndPaint(hWnd, &ps);
    }
        return 0;

    case WM_SIZE:
    {
        GetClientRect(hWnd, &clientRect_endingui);
        long width = clientRect_endingui.right - clientRect_endingui.left;
        long height = clientRect_endingui.bottom - clientRect_endingui.top;
        MoveWindow(hCancelBTN,  (int)(width  * 0.15),
                                (int)(height * 0.75),
                                (int)(width  * 0.20),
                                (int)(height * 0.15), 
                                    TRUE);
        MoveWindow(hCheckBoardBTN,  (int)(width  * 0.40),
                                (int)(height * 0.75),
                                (int)(width  * 0.20),
                                (int)(height * 0.15), 
                                    TRUE);
        /*
        MoveWindow(hAgainBtn,   (int)(width  * 0.65),
                                (int)(height * 0.75),
                                (int)(width  * 0.20),
                                (int)(height * 0.15), 
                                    TRUE);*/
    }
        return 0;

    case WM_COMMAND:
    {
        WORD notificationCode = HIWORD(wParam); // 高 16 位 → 按鈕通知
        WORD controlID        = LOWORD(wParam); // 低 16 位 → 控制項 ID

        // 分析控制項選取項目:(按鈕)(ID:1001)
        if(notificationCode == BN_CLICKED && controlID == 1001){ // Cancel button
            //MessageBox(hWnd, L"按鈕被按下了!", L"debug", MB_OK);
            if(serverConnecthWnd)   DestroyWindow(serverConnecthWnd);
            if(clientConnecthWnd)   DestroyWindow(clientConnecthWnd);
            serverConnecthWnd = nullptr;
            clientConnecthWnd = nullptr;
            DestroyWindow(gameUIWnd);
            gameUIWnd = nullptr;
            DestroyWindow(hWnd);
            hWnd = nullptr;
        }
        if(notificationCode == BN_CLICKED && controlID == 1002){
            ShowWindow(hWnd,SW_HIDE);
        }

    }
        return 0;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

bool RegisterEndingUIClass(HINSTANCE hInstance){
    WNDCLASSW wc = {};
    wc.lpfnWndProc   = endingUIWndProc;         // 指定視窗處理函式
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"EndingUIClass";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW); // 預設滑鼠游標
    //wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 視窗背景色(預設)
    wc.hbrBackground = CreateSolidBrush(RGB(255,255,255));

    return RegisterClassW(&wc) != 0;
}

void openEndingUIWindow(HWND hWnd, HINSTANCE hInst){
    if(!endingUIRegistered){
        endingUIRegistered = RegisterEndingUIClass(hInst);
        if(!endingUIRegistered){
            MessageBoxW(NULL, L"Failed to register Ending UI class", L"Debug", MB_OK);
            return;
        }
    }

    // 建立子視窗
    endingUIhWnd = CreateWindowExW(
        0,                          // Extended style
        L"EndingUIClass",           // 子視窗類別（事先要註冊）
        NULL,                       // 標題列，子視窗不需要
        WS_CHILD | WS_VISIBLE,      // 標準子視窗樣式
        rep::clientWidth  / 4,      // x
        rep::clientHeight / 4,      // y
        rep::clientWidth  / 2,      // width
        rep::clientHeight / 2,      // height
        hWnd,                       // 父視窗
        NULL,                       // menu
        hInst,                      // instance handle
        NULL
    );

    if (!endingUIhWnd){
        std::wstring wstr =  L"Failed to create game UI window";
        DWORD err = GetLastError();
        wstr = wstr + L"\nError code: " + std::to_wstring(err);
        MessageBoxW(hWnd, wstr.c_str(), L"Debug", MB_OK);
    } 

    UpdateWindow(endingUIhWnd);
    return;
}
