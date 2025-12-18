#include <WinSock2.h>
#include <windows.h>
#include <string>
#include <gdiplus.h>
#include <thread>

#include "connect.h"
#include "connectUI.h"
#include "rep.h"
#include "gameUI.h"
#include "game.h"
#include "endingUI.h"

using std::thread;
using std::wstring;
using std::string;

int connectPort = 36890;

HWND serverConnecthWnd;
HWND clientConnecthWnd;

HWND hCancelButton1;

HWND hEdit2;
HWND hEnterButton2;
HWND hCancelButton2;

bool clientConnectRegistered = false;
bool serverConnectRegistered = false;
RECT clientRect;

bool isCurrectConnect = false;

LRESULT CALLBACK ServerConnectWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){

    case WM_CREATE:
    {
        ConnectT::connect_s = new Connect_Server();
        if(ConnectT::connect_s->bindAndListen(connectPort)){
            thread([hWnd](){
                if(ConnectT::connect_s->acceptClient()){
                    ConnectT::connect_s->closeServer();
                    ConnectT::connect_s->sendData("ThisIsServer");
                    thread(ConnectT::startSvRecv,std::ref(*ConnectT::connect_s)).detach();
                    thread([hWnd](){
                        Sleep(2000);
                        if(!isCurrectConnect){
                            ConnectT::recv_flag = false;
                            {
                                int len = strlen("ErrorConnect") + 1;
                                char* str = new char[len];
                                strcpy(str, "ErrorConnect");
                                str[len] = '\0';
                                PostMessageA(hWnd, WM_APP_TCP_ERROR, 0, (LPARAM)str);
                            }
                        }
                    }).detach();
                }
            }).detach();
        }else{  //bindAndListen
            int len = strlen("ListeningDeny") + 1;
            char* str = new char[len];
            strcpy(str, "ListeningDeny");
            str[len] = '\0';
            PostMessageA(hWnd, WM_APP_TCP_ERROR, 0, (LPARAM)str);
        }

        GetClientRect(hWnd, &clientRect);
        long width = clientRect.right - clientRect.left;
        long height = clientRect.bottom - clientRect.top;

        hCancelButton1 = CreateWindowW(
            L"BUTTON",                                  // 按鈕類別
            L"取消",                                    // 按鈕文字
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,   // 樣式
            (int)(width  * 0.40),                       // x        左0.40
            (int)(height * 0.75),                       // y        上0.75
            (int)(width  * 0.20),                       // width    右0.40
            (int)(height * 0.15),                       // height   下0.10
            hWnd,                                       // 父視窗
            (HMENU)1001,                                // 控制項 ID（用來辨識按下的是哪個按鈕）
            rep::hInst,
            NULL
        );

    }
        return 0;

    case WM_COMMAND:
    {
        WORD notificationCode = HIWORD(wParam); // 高 16 位 → 按鈕通知
        WORD controlID        = LOWORD(wParam); // 低 16 位 → 控制項 ID

        // 分析控制項選取項目:(按鈕)(ID:1001)
        if(notificationCode == BN_CLICKED && controlID == 1001){ // Cancel button
            //MessageBox(hWnd, L"按鈕被按下了!", L"debug", MB_OK);
            DestroyWindow(hWnd);
            hWnd = NULL;
        }
    }
        return 0;

    case WM_APP_TCP_MSG:
    {   //Server
        char* buf = (char*)lParam;
        string s = buf;
        delete[] buf;
        //thread([&](){MessageBoxA(hWnd, s.c_str(), "Received", MB_OK);}).detach();
        if(s == "ThisIsClient_12345"){
            isCurrectConnect = true;
            gaming::player = (unsigned char)(gaming::getRandInt() % 2 + 1);
            if(gaming::player == 1) {   // 1 = Black
                gaming::isTurnToMe = true;
                ConnectT::connect_s->sendData("YouAreWhite");
            }
            else {
                gaming::isTurnToMe = false;
                ConnectT::connect_s->sendData("YouAreBlack");
            }
            //MessageBoxA(hWnd, s.c_str(), "Received", MB_OK);
            openGameUIWnd();
            ShowWindow(hWnd, SW_HIDE);
        }else if(s == "ClientClosed"){
            MessageBoxW(hWnd, L"連線中斷", L"Info", MB_OK);
            if(!endingUIhWnd){
                if(gameUIWnd)   DestroyWindow(gameUIWnd);
                DestroyWindow(hWnd);
            }
        }else if(s == "ErrorConnect"){
            DestroyWindow(hWnd);
        }else if(s.substr(0,5) == "Place"){
            //MessageBoxA(hWnd, s.c_str(), "Info", MB_OK);
            if(s.substr(5,5) == "Black"){
                if(gaming::player == 1){    //對面黑 而我方也黑
                    MessageBoxW(NULL, L"封包資訊錯誤", L"Error", MB_OK);
                    if(gameUIWnd) DestroyWindow(gameUIWnd);
                    DestroyWindow(hWnd);
                    return 0;
                }
            }

            int r = std::stoi(s.substr(10,2));
            int c = std::stoi(s.substr(12,2));
            //gaming::game.placePiece(r, c, gaming::player%2+1 );

            char* cstr = new char[5];
            cstr[0] = s[10];
            cstr[1] = s[11];
            cstr[2] = s[12];
            cstr[3] = s[13];
            cstr[4] = '\0';
            PostMessageA(gameUIWnd, WM_APP_TCP_MSG, 0, (LPARAM)cstr);

            gaming::isTurnToMe = true;
        }else if(s == "HereVectory"){
            gaming::isWin = false;
            PostMessageA(gameUIWnd, WM_APP_ADDTEXT, 10000, gaming::player % 2 + 1);
        }
    }
        return 0;
    
    case WM_APP_TCP_ERROR:
    {
        char* buf = (char*)lParam;
        string s = buf;
        delete[] buf;
        if(s == "ErrorConnect"){
            MessageBoxW(hWnd, L"連線錯誤", L"Error", MB_OK);
        }else if(s == "ListeningDeny"){
            MessageBoxW(hWnd, L"無法聆聽端口", L"Error", MB_OK);
        }
        
        DestroyWindow(hWnd);
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
        Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0, 0));    //ARGB

        // 設定文字顏色
        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);                // 背景透明

        Gdiplus::RectF textRect(        //x y w h
            (clientRect.right  * 0.15f), 
            (clientRect.bottom * 0.30f), 
            (clientRect.right  * 0.80f), 
            (clientRect.bottom * 0.40f)
        );

        graphics.DrawString(
            L"等待其他玩家．．．", 
            -1, 
            &font, 
            textRect, 
            NULL,
            &brush
        );

        EndPaint(hWnd, &ps);
    }
        return 0;

    case WM_SIZE:
    {
        GetClientRect(hWnd, &clientRect);
        MoveWindow(hCancelButton1, (int)(clientRect.right * 0.40), 
                                   (int)(clientRect.bottom * 0.75), 
                                   (int)(clientRect.right * 0.20), 
                                   (int)(clientRect.bottom * 0.15), TRUE);
        InvalidateRect(hWnd, NULL, TRUE);
    }
        return 0;
    
    case WM_DESTROY:
    {
        if(ConnectT::connect_s){
            delete ConnectT::connect_s;
            ConnectT::connect_s = nullptr;
        }
        PostQuitMessage(0);
    }
        return 0;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);

    }
}

LRESULT CALLBACK ClientConnectWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){

    case WM_CREATE:
    {
        ConnectT::connect_c = new Connect_Client();

        GetClientRect(hWnd, &clientRect);
        long width = clientRect.right - clientRect.left;
        long height = clientRect.bottom - clientRect.top;

        hEdit2 = CreateWindowW(
            L"EDIT",                // 輸入框類別
            L"127.0.0.1",           // 初始文字
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
            (int)(width  * 0.30),                       // x
            (int)(height * 0.55),                       // y
            (int)(width  * 0.30),                       // width
            (int)(height * 0.08),                       // height
            hWnd,                   // 父視窗
            (HMENU)1001,            // 控制項 ID
            rep::hInst,
            NULL
        );

        hEnterButton2 = CreateWindowW(
            L"BUTTON",                                  // 按鈕類別
            L"確認",                                    // 按鈕文字
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,   // 樣式
            (int)(width  * 0.65),                       // x    0.30 + 0.30 (+ 0.05)
            (int)(height * 0.55),                       // y    0.55 (與EDIT同高)
            (int)(width  * 0.10),                       // width
            (int)(height * 0.08),                       // height
            hWnd,                                       // 父視窗
            (HMENU)1002,                                // 控制項 ID（用來辨識按下的是哪個按鈕）
            rep::hInst,
            NULL
        );

        hCancelButton2 = CreateWindowW(
            L"BUTTON",                                  // 按鈕類別
            L"取消",                                    // 按鈕文字
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,   // 樣式
            (int)(width  * 0.40),                       // x        左0.40
            (int)(height * 0.75),                       // y        上0.75
            (int)(width  * 0.20),                       // width    右0.40
            (int)(height * 0.15),                       // height   下0.10
            hWnd,                                       // 父視窗
            (HMENU)1003,                                // 控制項 ID（用來辨識按下的是哪個按鈕）
            rep::hInst,
            NULL
        );
    }
        return 0;

    case WM_COMMAND:
    {
        WORD notificationCode = HIWORD(wParam); // 高 16 位 → 按鈕通知
        WORD controlID        = LOWORD(wParam); // 低 16 位 → 控制項 ID

        if(notificationCode == BN_CLICKED && controlID == 1002){    // Enter button
            //MessageBox(hWnd, L"確認按鈕被按下了!", L"debug", MB_OK);
            int len = GetWindowTextLength(hEdit2);
            char* editText = new char[len+1];
            GetWindowTextA(hEdit2, editText, len + 1);

            thread([editText,hWnd](){
                EnableWindow(hEnterButton2, FALSE);
                EnableWindow(hEdit2, FALSE);
                ConnectT::connect_c->initializeServerAddr(editText, connectPort);
                int connectCounter = 5;
                while(--connectCounter){
                    if(ConnectT::connect_c->connectToServer()){
                        ConnectT::connect_c->sendData("ThisIsClient_12345");
                        thread(ConnectT::startClRecv,std::ref(*ConnectT::connect_c)).detach();
                        thread([hWnd](){
                            Sleep(2000);
                            if(!isCurrectConnect){
                                ConnectT::recv_flag = false;
                                {
                                    int len = strlen("ErrorConnect") + 1;
                                    char* str = new char[len];
                                    strcpy(str, "ErrorConnect");
                                    str[len] = '\0';
                                    PostMessageA(hWnd, WM_APP_TCP_ERROR, 0, (LPARAM)str);
                                }
                            }
                        }).detach();
                        break;
                    } // end-if
                    Sleep(400);
                } // end-while
                if(connectCounter == 0){
                    EnableWindow(hEnterButton2, TRUE);
                    EnableWindow(hEdit2, TRUE);
                    int err = WSAGetLastError();
                    wstring ws = wstring(L"連線失敗\nError code:") + std::to_wstring(err);
                    MessageBoxW(clientConnecthWnd, ws.c_str(), L"Error", MB_OK);
                    //MessageBoxW(hWnd, L"連線失敗", L"debug", MB_OK);
                }
                

            }).detach();
            

            delete[] editText;
        }
        if(notificationCode == BN_CLICKED && controlID == 1003){    // Cancel button
            //MessageBox(hWnd, L"取消按鈕被按下了!", L"debug", MB_OK);
            DestroyWindow(hWnd);
            hWnd = NULL;
        }
    }
        return 0;

    case WM_APP_TCP_MSG:
    {   //client
        char* buf = (char*)lParam;
        string s = buf;
        delete[] buf;
        //thread([&](){MessageBoxA(hWnd, s.c_str(), "Received", MB_OK);}).  detach();
        if (s == "ThisIsServer"){
            isCurrectConnect = true;
            //MessageBoxA(hWnd, s.c_str(), "Received", MB_OK);
            openGameUIWnd();
            ShowWindow(hWnd, SW_HIDE);
        }else if(s == "ServerClosed"){
            MessageBoxW(hWnd, L"連線中斷", L"Info", MB_OK);
            if(!endingUIhWnd){
                if(gameUIWnd)   DestroyWindow(gameUIWnd);
                DestroyWindow(hWnd);
            }
        }else if(s == "YouAreBlack"){
            gaming::player = 1;
            gaming::isTurnToMe = true;
            updataUpArea();
            //MessageBoxA(NULL, s.c_str(), "Received_", MB_OK);
        }else if(s == "YouAreWhite"){
            gaming::player = 2;
            gaming::isTurnToMe = false;
            updataUpArea();
            //MessageBoxA(NULL, s.c_str(), "Received_", MB_OK);
        }else if(s.substr(0,5) == "Place"){
            if(s.substr(5,5) == "Black"){
                if(gaming::player == 1){
                    MessageBoxW(NULL, L"封包資訊錯誤", L"Error", MB_OK);
                    if(gameUIWnd) DestroyWindow(gameUIWnd);
                    DestroyWindow(hWnd);
                    return 0;
                }
            }
            int r = std::stoi(s.substr(10,2));
            int c = std::stoi(s.substr(12,2));
            //gaming::game.placePiece(r, c, gaming::player%2+1 );

            char* cstr = new char[5];
            cstr[0] = s[10];
            cstr[1] = s[11];
            cstr[2] = s[12];
            cstr[3] = s[13];
            cstr[4] = '\0';
            PostMessageA(gameUIWnd, WM_APP_TCP_MSG, 0, (LPARAM)cstr);

            gaming::isTurnToMe = true;
        }else if(s == "HereVectory"){
            gaming::isWin = false;
            PostMessageA(gameUIWnd, WM_APP_ADDTEXT, 10000, gaming::player % 2 + 1);
        }
    }
        return 0;

    case WM_APP_TCP_ERROR:
    {
        char* buf = (char*)lParam;
        string s = buf;
        delete[] buf;
        if(s == "ErrorConnect"){
            MessageBoxW(hWnd, L"連線錯誤", L"Error", MB_OK);
        }
        DestroyWindow(hWnd);
    }
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Gdiplus::Graphics graphics(hdc);

        Gdiplus::FontFamily fontFamily(L"Arial");
        Gdiplus::REAL fontSize = 27 * (rep::scaleX + rep::scaleY) / 2; 
        Gdiplus::Font font(&fontFamily, fontSize, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0, 0));    //ARGB

        // 設定文字顏色
        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);                // 背景透明

        Gdiplus::RectF textRect(        //x y w h
            (clientRect.right * 0.15f), 
            (clientRect.bottom * 0.20f), 
            (clientRect.right  * 0.80f), 
            (clientRect.bottom * 0.40f)
        );

        graphics.DrawString(
            L"請輸入伺服器 IP 位址", 
            -1, 
            &font, 
            textRect, 
            NULL,
            &brush
        );

        EndPaint(hWnd, &ps);
    }
        return 0;

    case WM_SIZE:
    {
        GetClientRect(hWnd, &clientRect);
        MoveWindow(hEdit2,  (int)(clientRect.right  * 0.30), 
                            (int)(clientRect.bottom * 0.55), 
                            (int)(clientRect.right  * 0.30), 
                            (int)(clientRect.bottom * 0.08), TRUE);

        MoveWindow(hEnterButton2,   (int)(clientRect.right  * 0.65), 
                                    (int)(clientRect.bottom * 0.55), 
                                    (int)(clientRect.right  * 0.10), 
                                    (int)(clientRect.bottom * 0.08), TRUE);

        MoveWindow(hCancelButton2,  (int)(clientRect.right  * 0.40), 
                                    (int)(clientRect.bottom * 0.75), 
                                    (int)(clientRect.right  * 0.20), 
                                    (int)(clientRect.bottom * 0.15), TRUE);
        InvalidateRect(hWnd, NULL, TRUE);
    }
        return 0;

    case WM_DESTROY:
    {
        if(ConnectT::connect_c) {
            delete ConnectT::connect_c;
            ConnectT::connect_c = nullptr;
        }
        PostQuitMessage(0);
    }
        return 0;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);

    }
}


bool RegisterserverConnectClass(HINSTANCE hInstance){
    WNDCLASSW wc = {};
    wc.lpfnWndProc   = ServerConnectWndProc;         // 指定視窗處理函式
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"ServerConnectClass";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW); // 預設滑鼠游標
    //wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 視窗背景色(預設)
    wc.hbrBackground = CreateSolidBrush(RGB(225, 192, 225));

    return RegisterClassW(&wc) != 0;
}
bool RegisterclientConnectClass(HINSTANCE hInstance){
    WNDCLASSW wc = {};
    wc.lpfnWndProc   = ClientConnectWndProc;         // 指定視窗處理函式
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"ClientConnectClass";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW); // 預設滑鼠游標
    //wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 視窗背景色(預設)
    wc.hbrBackground = CreateSolidBrush(RGB(225, 192, 225));

    return RegisterClassW(&wc) != 0;
}


void openServerConnectWindow(HWND hWnd, HINSTANCE hInst, int nCmdShow){
    if(!serverConnectRegistered){
        serverConnectRegistered = RegisterserverConnectClass(hInst);
        if(!serverConnectRegistered){
            MessageBoxW(NULL, L"Failed to register Server Connect class", L"Debug", MB_OK);
            return;
        }
    }
    
    // 建立子視窗
    serverConnecthWnd = CreateWindowExW(
        0,                          // Extended style
        L"ServerConnectClass",      // 子視窗類別（事先要註冊）
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

    if (!serverConnecthWnd){
        wstring wstr =  L"Failed to create game UI window";
        DWORD err = GetLastError();
        wstr = wstr + L"\nError code: " + std::to_wstring(err);
        MessageBoxW(hWnd, wstr.c_str(), L"Debug", MB_OK);
    } 

    ShowWindow(serverConnecthWnd, rep::nCmdShow);
    UpdateWindow(serverConnecthWnd);

    return;

}
void openClientConnectWindow(HWND hWnd, HINSTANCE hInst, int nCmdShow){
    if(!clientConnectRegistered){
        clientConnectRegistered = RegisterclientConnectClass(hInst);
        if(!clientConnectRegistered){
            MessageBoxW(NULL, L"Failed to register Client Connect class", L"Debug", MB_OK);
            return;
        }
    }

    
    // 建立子視窗
    clientConnecthWnd = CreateWindowExW(
        0,                          // Extended style
        L"ClientConnectClass",      // 子視窗類別（事先要註冊）
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

    if (!clientConnecthWnd){
        wstring wstr =  L"Failed to create game UI window";
        DWORD err = GetLastError();
        wstr = wstr + L"\nError code: " + std::to_wstring(err);
        MessageBoxW(hWnd, wstr.c_str(), L"Debug", MB_OK);
    } 

    ShowWindow(clientConnecthWnd, rep::nCmdShow);
    UpdateWindow(clientConnecthWnd);

    return;
}
