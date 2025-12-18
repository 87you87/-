#pragma once
#include <windows.h>
#include <string>

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

namespace mainW{
    void buttonClicked(int, HWND);
}
