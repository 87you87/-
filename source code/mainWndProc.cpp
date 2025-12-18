#include <WinSock2.h>
#include <windows.h>
#include <gdiplus.h>
#include <string>

#include "mainWndProc.h"
#include "rep.h"
#include "connectUI.h"
#include "gameUI.h"
//#pragma comment(lib, "gdiplus.lib")

HINSTANCE rep::hInst;
int rep::nCmdShow;

bool paintBack = true;
bool gameUIRegistered = false;
 
std::wstring path;

Gdiplus::Image* rep::imgBackGround;
Gdiplus::Image* rep::imgButtonSelected;
Gdiplus::Image* temp;
Gdiplus::Image* imgButton1;
Gdiplus::Image* imgButton2;
Gdiplus::Image* imgButton3;

bool isButton1Hovered = false;
bool isButton2Hovered = false;
bool isButton3Hovered = false;

RECT buttonRect1_Ori = { 300, 180, 500, 240 };  //左上右下
RECT buttonRect2_Ori = { 300, 280, 500, 340 };
RECT buttonRect3_Ori = { 300, 380, 500, 440 };

RECT buttonRect1;
RECT buttonRect2;
RECT buttonRect3;

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){
    case WM_CREATE:
    {
        path = rep::GetExeDirectory();

        rep::imgBackGround = rep::LoadImageGDIPlus(path + std::wstring(L"\\res\\images\\background.jpg"));
        imgButton1 = rep::LoadImageGDIPlus(path + std::wstring(L"\\res\\images\\button1.png"));
        imgButton2 = rep::LoadImageGDIPlus(path + std::wstring(L"\\res\\images\\button2.png"));
        imgButton3 = rep::LoadImageGDIPlus(path + std::wstring(L"\\res\\images\\button3.png"));
        rep::imgButtonSelected = rep::LoadImageGDIPlus(path + std::wstring(L"\\res\\images\\buttonSelected.png"));

        rep::hInst = ((LPCREATESTRUCT)lParam)->hInstance;                //取得WinMain的hInstance
        rep::nCmdShow = static_cast<int>(reinterpret_cast<intptr_t>(((LPCREATESTRUCT)lParam)->lpCreateParams));   
        //取得WinMain的nCmdShow

    }
        return 0;
    case WM_MOUSEMOVE:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        buttonRect1 = rep::reSizedRect(buttonRect1_Ori);
        buttonRect2 = rep::reSizedRect(buttonRect2_Ori);
        buttonRect3 = rep::reSizedRect(buttonRect3_Ori);

        bool inside1 = rep::pointInRect(buttonRect1, x, y);
        bool inside2 = rep::pointInRect(buttonRect2, x, y);
        bool inside3 = rep::pointInRect(buttonRect3, x, y);

        if (inside1 && !isButton1Hovered)
        {
            isButton1Hovered = true;
            isButton2Hovered = false;
            isButton3Hovered = false;
            InvalidateRect(hWnd, &buttonRect1, FALSE); // 只重畫按鈕區域
            InvalidateRect(hWnd, &buttonRect2, FALSE);
            InvalidateRect(hWnd, &buttonRect3, FALSE);
        }
        else if (inside2 && !isButton2Hovered)
        {
            isButton1Hovered = false;
            isButton2Hovered = true;
            isButton3Hovered = false;
            InvalidateRect(hWnd, &buttonRect1, FALSE);
            InvalidateRect(hWnd, &buttonRect2, FALSE);
            InvalidateRect(hWnd, &buttonRect3, FALSE);
        } else if (inside3 && !isButton3Hovered)
        {
            isButton1Hovered = false;
            isButton2Hovered = false;
            isButton3Hovered = true;
            InvalidateRect(hWnd, &buttonRect1, FALSE);
            InvalidateRect(hWnd, &buttonRect2, FALSE);
            InvalidateRect(hWnd, &buttonRect3, FALSE);
        } else if(!inside1 && !inside2 && !inside3 &&
                   (isButton1Hovered || isButton2Hovered || isButton3Hovered)){
            isButton1Hovered = false;
            isButton2Hovered = false;
            isButton3Hovered = false;
            InvalidateRect(hWnd, &buttonRect1, FALSE);
            InvalidateRect(hWnd, &buttonRect2, FALSE);
            InvalidateRect(hWnd, &buttonRect3, FALSE);
        }
    }
        return 0;

    case WM_LBUTTONDOWN:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        if (rep::pointInRect(buttonRect1, x, y)) {
            mainW::buttonClicked(1, hWnd);
        } else if (rep::pointInRect(buttonRect2, x, y)) {
            mainW::buttonClicked(2, hWnd);
        } else if (rep::pointInRect(buttonRect3, x, y)) {
            mainW::buttonClicked(3, hWnd);
        }
    }
        return 0;

    case WM_SIZE:
    {
        rep::clientWidth  = LOWORD(lParam);
        rep::clientHeight = HIWORD(lParam);

        rep::scaleX = rep::clientWidth  / 800.0f;
        rep::scaleY = rep::clientHeight / 600.0f;
        
        if (gameUIWnd && ::IsWindow(gameUIWnd)) {
            MoveWindow(gameUIWnd, 0, 0, rep::clientWidth, rep::clientHeight, TRUE);
            InvalidateRect(gameUIWnd, NULL, TRUE);
        }

        if (serverConnecthWnd)  
            MoveWindow(serverConnecthWnd, rep::clientWidth/4, rep::clientHeight/4, 
                                          rep::clientWidth/2, rep::clientHeight/2, TRUE);

        if (clientConnecthWnd)  
            MoveWindow(clientConnecthWnd, rep::clientWidth/4, rep::clientHeight/4, 
                                          rep::clientWidth/2, rep::clientHeight/2, TRUE);

        InvalidateRect(hWnd, NULL, TRUE); // 重畫整個視窗
    }
        return 0;

    case WM_GETMINMAXINFO:  
    {
        LPMINMAXINFO lpMinMaxInfo = (LPMINMAXINFO)lParam;
        lpMinMaxInfo->ptMinTrackSize.x = 600; // 最小寬度
        lpMinMaxInfo->ptMinTrackSize.y = 450; // 最小高度
        
        lpMinMaxInfo->ptMaxTrackSize.x = 1600; // 最大寬度
        lpMinMaxInfo->ptMaxTrackSize.y = 900; // 最大高度
    }
        return 0;

    case WM_ERASEBKGND:
    {
        paintBack = true;
    }
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Gdiplus::Graphics graphics(hdc);
        if(paintBack){
            graphics.DrawImage(rep::imgBackGround, 0, 0, rep::clientWidth, rep::clientHeight);
            rep::drawImageResized(graphics, imgButton1, 300, 180, 200, 60);
            rep::drawImageResized(graphics, imgButton2, 300, 280, 200, 60);
            rep::drawImageResized(graphics, imgButton3, 300, 380, 200, 60);
            paintBack = false;
        }
        
        temp = isButton1Hovered ? rep::imgButtonSelected : imgButton1;
        rep::drawImageResized(graphics, temp, 300, 180, 200, 60); // 繪製到 (300,80) 並縮放到 200x60
        temp = isButton2Hovered ? rep::imgButtonSelected : imgButton2;
        rep::drawImageResized(graphics, temp, 300, 280, 200, 60);
        temp = isButton3Hovered ? rep::imgButtonSelected : imgButton3;
        rep::drawImageResized(graphics, temp, 300, 380, 200, 60);

        Gdiplus::FontFamily fontFamily(L"Arial");
        Gdiplus::REAL fontSize = 45 * (rep::scaleX + rep::scaleY) / 2; 
        Gdiplus::Font font(&fontFamily, fontSize, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0, 0));    //ARGB

        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);                // 背景透明
        Gdiplus::StringFormat str_format;
        str_format.SetAlignment(Gdiplus::StringAlignmentCenter);        // 水平置中
        str_format.SetLineAlignment(Gdiplus::StringAlignmentCenter);    // 垂直置中
        Gdiplus::RectF textRect(        //x y w h
            (rep::clientWidth  * 0.20f), 
            (rep::clientHeight * 0.08f), 
            (rep::clientWidth  * 0.60f), 
            (rep::clientHeight * 0.15f)
        );
        graphics.DrawString(
            L"五子棋", 
            -1, 
            &font, 
            textRect, 
            &str_format,
            &brush
        );
        
        EndPaint(hWnd, &ps);
    }
        return 0;

    case WM_DESTROY:                // 視窗關閉時
    {
        if(rep::imgBackGround) delete rep::imgBackGround;
        if(imgButton1) delete imgButton1;
        if(imgButton2) delete imgButton2;
        if(imgButton3) delete imgButton3;
        if(rep::imgButtonSelected) delete rep::imgButtonSelected;
        PostQuitMessage(0);         //終止消息循環 如果沒有 視窗也能關閉 但是程式還在執行
    }
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}


void mainW::buttonClicked(int buttonID, HWND hWnd){
    /*if(buttonID == 1){*
        
        std::wstring str = L"Button" + std::to_wstring(buttonID);
        MessageBoxW(hWnd, str.c_str(), L"Info", MB_OK);
        
        if (!gameUIRegistered){
            gameUIRegistered = RegisterGameUIClass(rep::hInst);
            if(!gameUIRegistered)
                MessageBoxW(NULL, L"Failed to register Game UI class", L"Debug", MB_OK);
            else
                MessageBoxW(NULL, L"Game UI class registered", L"Debug", MB_OK);
        }

        RECT rc;
        GetClientRect(hWnd, &rc);

        // 建立子視窗
        gameUIWnd = CreateWindowExW(
            0,                          // Extended style
            L"GameUIWindowClass",       // 子視窗類別（事先要註冊）
            NULL,                       // 標題列，子視窗不需要
            WS_CHILD | WS_VISIBLE,      // 標準子視窗樣式
            0, 0,                       // x, y
            rc.right - rc.left,         // width
            rc.bottom - rc.top,         // height
            hWnd,                       // 父視窗
            NULL,                       // menu
            rep::hInst,                 // instance handle
            NULL
        );

        if (!gameUIWnd){
            MessageBoxW(hWnd, L"Failed to create game UI window", L"Debug", MB_OK);
            DWORD err = GetLastError();
            MessageBoxW(hWnd, std::to_wstring(err).c_str(), L"Debug", MB_OK);
        }        

        ShowWindow(gameUIWnd, rep::nCmdShow);
        UpdateWindow(gameUIWnd);
        
        return;
        


    } //end of if(buttonID == 1)*/
    if(buttonID == 1){
        openServerConnectWindow(hWnd, rep::hInst, rep::nCmdShow);
    }
    if(buttonID == 2){
        openClientConnectWindow(hWnd, rep::hInst, rep::nCmdShow);
    }
    if(buttonID == 3){
        PostMessage(hWnd, WM_CLOSE, 0, 0);
    }
}
