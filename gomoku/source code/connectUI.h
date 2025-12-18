#pragma once
#include <windows.h>
 
extern HWND serverConnecthWnd;
LRESULT CALLBACK ServerConnectWndProc(HWND, UINT, WPARAM, LPARAM);
bool RegisterserverConnectClass(HINSTANCE);



extern HWND clientConnecthWnd;
LRESULT CALLBACK ClientConnectWndProc(HWND, UINT, WPARAM, LPARAM);
bool RegisterclientConnectClass(HINSTANCE);

void openServerConnectWindow(HWND, HINSTANCE, int);
void openClientConnectWindow(HWND, HINSTANCE, int);