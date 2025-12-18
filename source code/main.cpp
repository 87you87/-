// g++ -o gomoku main.cpp mainWndProc.cpp rep.cpp gameUI.cpp game.cpp connectUI.cpp connect.cpp endingUI.cpp -mwindows -municode -lgdiplus -lws2_32
// g++ -o gomoku *.cpp -mwindows -municode -lgdiplus -lws2_32 
#include <windows.h>
#include <gdiplus.h>

#include "mainWndProc.h"
#include "rep.h"
//#pragma comment(lib, "gdiplus.lib")
//#pragma comment(lib, "ws2_32.lib")

HWND rep::mainWindowProc;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow){

    rep::InitGDIPlus();             //初始化 GDI+

    // 1. 註冊視窗類別
    const wchar_t CLASS_NAME[] = L"MyWindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc   = MainWndProc;         // 指定視窗處理函式
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW); // 預設滑鼠游標
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 視窗背景色

    RegisterClass(&wc);

    // 2. 建立視窗
    rep::mainWindowProc = CreateWindowExW(                        // 只比CreatWindowW多一個exStyle (否則預設為0)
        0,                                              // 擴充樣式
        CLASS_NAME,                                     // 視窗類別名稱
        L"Gokomu",                                      // 視窗標題
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,          // 視窗樣式
        CW_USEDEFAULT, CW_USEDEFAULT,                   // 視窗位置
        800, 600,                                       // 視窗大小 (寬, 高)
        NULL, NULL, hInstance, 
        //NULL  //不傳遞參數給WM_CREATE
        reinterpret_cast<LPVOID>(static_cast<intptr_t>(nCmdShow))
        // 傳遞wWinMain的nCmdShow給WM_CREATE
    );

    if (rep::mainWindowProc == NULL) return 0;

    // 3. 顯示視窗
    ShowWindow(rep::mainWindowProc, nCmdShow);
    UpdateWindow(rep::mainWindowProc);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // 處理鍵盤訊息
        DispatchMessage(&msg);  // 派發訊息到 WndProc
    }


    rep::ShutdownGDIPlus();         //關閉 GDI+

    return (int) msg.wParam;
}
