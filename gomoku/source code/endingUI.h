#pragma once
#include <Windows.h>


extern HWND endingUIhWnd;
LRESULT CALLBACK endingUIWndProc(HWND, UINT, WPARAM, LPARAM);
bool RegisterEndingUIClass(HINSTANCE);
void openEndingUIWindow(HWND, HINSTANCE);
